/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "WASAPI.hpp"

std::unique_ptr<WASAPI_device>  netero::audio::backend::impl::WASAPI_get_default_device(EDataFlow dataFlow) {
    HRESULT     result;
    std::unique_ptr<WASAPI_device> device = std::make_unique<WASAPI_device>();

    if (!_device_enumerator) {
        return nullptr;
    }
    result = _device_enumerator->GetDefaultAudioEndpoint(dataFlow,
        eConsole,
        &device->device);
    if (FAILED(result)) { return nullptr; }

    result = device->device->Activate(IID_IAudioClient,
        CLSCTX_ALL,
        nullptr,
        reinterpret_cast<void**>(&device->audio_client));
    if (FAILED(result)) { return nullptr; }
    result = device->audio_client->GetMixFormat(&device->wfx);
    if (FAILED(result)) { return nullptr; }
    if (device->wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        device->wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(device->wfx);
    }
    result = device->audio_client->GetDevicePeriod(nullptr, &device->latency);
    if (FAILED(result)) { return nullptr; }
    result = device->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
        0,
        0,
        0,
        device->wfx,
        nullptr);
    if (FAILED(result)) { return nullptr; }
    result = device->audio_client->GetBufferSize(&device->framesCount);
    if (FAILED(result)) { return nullptr; }
    if (dataFlow == eRender) {
        result = device->audio_client->GetService(IID_IAudioRenderClient,
            reinterpret_cast<void**>(&device->render_client));
        if (FAILED(result)) { return nullptr; }
    }
    else if (dataFlow == eCapture) {
        result = device->audio_client->GetService(IID_IAudioCaptureClient,
            reinterpret_cast<void**>(&device->capture_client));
        if (FAILED(result)) { return nullptr; }
    }
    return std::move(device);
}

