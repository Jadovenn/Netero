/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceImpl.hpp"
#include "WasapiHelpers.hpp"

#include <audiopolicy.h>
#include <avrt.h>

Netero::Audio::Device::RtCode DeviceImpl::RenderingNativeCallback(DeviceImpl* aContext)
{
    REFERENCE_TIME        latency;
    UINT32                bufferFramesCount;
    UINT32                padding;
    size_t                availableFrames;
    HANDLE                task = nullptr;
    IAudioSessionControl* audioSession = nullptr;
    IAudioRenderClient*   audioRenderClient = nullptr;
    BYTE*                 buffer = nullptr;

    HRESULT result = aContext->_audioClient->GetDevicePeriod(nullptr, &latency);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }

    result = aContext->_audioClient
                 ->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, aContext->_wfx, nullptr);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }

    result = aContext->_audioClient->GetBufferSize(&bufferFramesCount);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }

    result = aContext->_audioClient->GetService(wasapi::IID_IAudioSessionControl,
                                                reinterpret_cast<void**>(&audioSession));
    if (FAILED(result)) {
        goto exit_on_error;
    }

    result = aContext->_audioClient->GetService(wasapi::IID_IAudioRenderClient,
                                                reinterpret_cast<void**>(&audioRenderClient));
    if (FAILED(result)) {
        goto exit_on_error;
    }

    const size_t bytePerFrames =
        aContext->_wfx->nChannels * (aContext->_wfx->wBitsPerSample / static_cast<size_t>(8));

    DWORD taskIndex = 0;
    task = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
    if (!task) {
        goto exit_on_error;
    }

    result = aContext->_audioClient->Start();
    if (FAILED(result)) {
        goto exit_on_error;
    }

    while (aContext->_renderingAsyncState == AsyncState::RUN) {
        result = aContext->_audioClient->GetCurrentPadding(&padding);
        if (FAILED(result)) {
            goto exit_on_error;
        }
        else if (padding != bufferFramesCount) {
            availableFrames = bufferFramesCount - padding;
            result = audioRenderClient->GetBuffer(availableFrames, &buffer);
            if (FAILED(result)) {
                goto exit_on_error;
            }
            std::memset(buffer, 0, availableFrames * bytePerFrames);
            aContext->_processingCallback(reinterpret_cast<float*>(buffer), availableFrames);
            result = audioRenderClient->ReleaseBuffer(availableFrames, 0);
            if (FAILED(result)) {
                goto exit_on_error;
            }
        }
        else {
            std::this_thread::yield();
        }
    }

    result = aContext->_audioClient->Stop();
    if (FAILED(result)) {
        goto exit_on_error;
    }

    if (task) {
        AvRevertMmThreadCharacteristics(task);
    }
    wasapi::release<IAudioSessionControl>(&audioSession);
    wasapi::release<IAudioRenderClient>(&audioRenderClient);
    return RtCode::SUCCESS;

exit_on_error:
    if (task) {
        AvRevertMmThreadCharacteristics(task);
    }
    wasapi::release<IAudioSessionControl>(&audioSession);
    wasapi::release<IAudioRenderClient>(&audioRenderClient);
    return RtCode::SYSTEM_ERROR;
}

Netero::Audio::Device::RtCode DeviceImpl::openForRendering()
{
    this->_renderingAsyncState = AsyncState::RUN;
    this->_renderingFuture =
        std::async(std::launch::async, std::bind(DeviceImpl::RenderingNativeCallback, this));
    if (!this->_renderingFuture.valid()) {
        this->_renderingAsyncState = AsyncState::STOP;
        return RtCode::SYSTEM_ERROR;
    }
    this->_isOpen = true;
    return RtCode::SUCCESS;
}

Netero::Audio::Device::RtCode DeviceImpl::closeAfterRendering()
{
    if (!this->_renderingFuture.valid()) {
        return RtCode::SYSTEM_ERROR;
    }
    this->_renderingAsyncState = AsyncState::STOP;
    this->_renderingFuture.wait();
    this->_isOpen = false;
    return this->_renderingFuture.get();
}
