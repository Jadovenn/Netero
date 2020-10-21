/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceImpl.hpp"
#include "WasapiHelpers.hpp"

#include <audiopolicy.h>
#include <avrt.h>

Netero::Audio::Device::RtCode DeviceImpl::AcquisitionNativeCallback(DeviceImpl* aContext)
{
    REFERENCE_TIME        latency;
    UINT32                bufferFramesCount;
    UINT32                padding;
    DWORD                 flags;
    size_t                availableFrames;
    HANDLE                task = nullptr;
    IAudioSessionControl* audioSession = nullptr;
    IAudioCaptureClient*  audioCaptureClient = nullptr;
    BYTE*                 buffer = nullptr;

    HRESULT result = aContext->_audioClient->GetDevicePeriod(nullptr, &latency);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }

    flags = 0;
    if (aContext->_dataflow == EDataFlow::eAll) {
        flags = AUDCLNT_STREAMFLAGS_LOOPBACK;
    }
    result = aContext->_audioClient
                 ->Initialize(AUDCLNT_SHAREMODE_SHARED, flags, 0, 0, aContext->_wfx, nullptr);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }
    flags = 0;

    result = aContext->_audioClient->GetBufferSize(&bufferFramesCount);
    if (FAILED(result)) {
        return RtCode::SYSTEM_ERROR;
    }

    result = aContext->_audioClient->GetService(wasapi::IID_IAudioSessionControl,
                                                reinterpret_cast<void**>(&audioSession));
    if (FAILED(result)) {
        goto exit_on_error;
    }

    result = aContext->_audioClient->GetService(wasapi::IID_IAudioCaptureClient,
                                                reinterpret_cast<void**>(&audioCaptureClient));
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

    while (aContext->_acquisitionAsyncState == AsyncState::RUN) {
        result = audioCaptureClient->GetNextPacketSize(&padding);
        if (FAILED(result)) {
            goto exit_on_error;
        }
        else if (padding != 0) {
            result = audioCaptureClient->GetBuffer(&buffer, &padding, &flags, nullptr, nullptr);
            if (FAILED(result) || buffer == nullptr) {
                goto exit_on_error;
            }
            aContext->_acquisitionCallback(reinterpret_cast<const float*>(buffer), padding);
            result = audioCaptureClient->ReleaseBuffer(padding);
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
    wasapi::release<IAudioCaptureClient>(&audioCaptureClient);
    return RtCode::SUCCESS;

exit_on_error:
    if (task) {
        AvRevertMmThreadCharacteristics(task);
    }
    wasapi::release<IAudioSessionControl>(&audioSession);
    wasapi::release<IAudioCaptureClient>(&audioCaptureClient);
    return RtCode::SYSTEM_ERROR;

    return RtCode::SUCCESS;
}

Netero::Audio::Device::RtCode DeviceImpl::openForAcquisition()
{
    this->_acquisitionAsyncState = AsyncState::RUN;
    this->_acquisitionFuture =
        std::async(std::launch::async, std::bind(DeviceImpl::AcquisitionNativeCallback, this));
    if (!this->_acquisitionFuture.valid()) {
        this->_acquisitionAsyncState = AsyncState::STOP;
        return RtCode::SYSTEM_ERROR;
    }
    return RtCode::SUCCESS;
}

Netero::Audio::Device::RtCode DeviceImpl::closeAfterAcquisition()
{
    if (!this->_acquisitionFuture.valid()) {
        return RtCode::SYSTEM_ERROR;
    }
    this->_acquisitionAsyncState = AsyncState::STOP;
    auto status = this->_acquisitionFuture.wait_for(std::chrono::seconds(1));
    if (status != std::future_status::ready) {
        return RtCode::TIME_OUT;
    }
    return this->_acquisitionFuture.get();
}
