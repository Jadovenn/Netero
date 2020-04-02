/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
#include "WASAPI.hpp"

std::unique_ptr<WASAPI_device>  netero::audio::backend::impl::WASAPI_init_device(EDataFlow dataFlow, IMMDevice* device, bool isLoopback) {
    HRESULT     result;
    std::unique_ptr<WASAPI_device> wDevice = std::make_unique<WASAPI_device>();

    if (!_device_enumerator || !device) {
        return nullptr;
    }
    wDevice->device = device;
    wDevice->isLoopBackDevice = isLoopback;

    result = wDevice->device->Activate(IID_IAudioClient,
        CLSCTX_ALL,
        nullptr,
        reinterpret_cast<void**>(&wDevice->audio_client));
    if (FAILED(result)) { return nullptr; }
    result = wDevice->audio_client->GetMixFormat(&wDevice->wfx);
    if (FAILED(result)) { return nullptr; }
    if (wDevice->wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        wDevice->wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wDevice->wfx);
    }
    result = wDevice->audio_client->GetDevicePeriod(nullptr, &wDevice->latency);
    if (FAILED(result)) { return nullptr; }
    if (wDevice->isLoopBackDevice) {
		result = wDevice->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_LOOPBACK,
			0,
			0,
			wDevice->wfx,
			nullptr);
		if (FAILED(result)) { return nullptr; }
    }
    else {
        result = wDevice->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
            0,
            0,
            0,
            wDevice->wfx,
            nullptr);
        if (FAILED(result)) { return nullptr; }
    }
    result = wDevice->audio_client->GetBufferSize(&wDevice->framesCount);
    if (FAILED(result)) { return nullptr; }
    if (dataFlow == eRender) {
        result = wDevice->audio_client->GetService(IID_IAudioRenderClient,
            reinterpret_cast<void**>(&wDevice->render_client));
        if (FAILED(result)) { return nullptr; }
    }
    else if (dataFlow == eCapture) {
        result = wDevice->audio_client->GetService(IID_IAudioCaptureClient,
            reinterpret_cast<void**>(&wDevice->capture_client));
        if (FAILED(result)) { return nullptr; }
    }
    return std::move(wDevice);
}

IMMDevice* netero::audio::backend::impl::WASAPI_get_device(EDataFlow flow, const netero::audio::device& device) {
    HRESULT result;
    IMMDeviceCollection* pCollection = nullptr;
    IMMDevice* pEndpoint = nullptr;

    result = _device_enumerator->EnumAudioEndpoints(flow,
        DEVICE_STATE_ACTIVE,
        &pCollection);
    if (FAILED(result)) {
        return nullptr;
    }
    UINT  count;
    result = pCollection->GetCount(&count);
    if (FAILED(result) || count == 0) {
        goto exit_error;
    }
    for (ULONG idx = 0; idx < count; idx++) {
        netero::audio::device search_device;
        result = pCollection->Item(idx, &pEndpoint);
        if (FAILED(result)) {
            goto exit_error;
        }
        RtCode code = WASAPI_get_struct_Device(pEndpoint, search_device);
        if (code != RtCode::OK) {
            goto exit_error;
        }
        if (search_device.id == device.id) {
            WASAPI_release<IMMDeviceCollection>(&pCollection);
            return pEndpoint;
        }
        WASAPI_release<IMMDevice>(&pEndpoint);
    }
exit_error:
    WASAPI_release<IMMDevice>(&pEndpoint);
    WASAPI_release<IMMDeviceCollection>(&pCollection);
    return nullptr;
}

netero::audio::RtCode   netero::audio::backend::impl::setRenderDevice(const netero::audio::device& device) {
    HRESULT result;
    IMMDevice* endpoint = nullptr;

    endpoint = WASAPI_get_device(eRender, device);
    if (!endpoint) {
        return RtCode::ERR_NO_SUCH_DEVICE;
    }
    std::unique_ptr<WASAPI_device> new_device = WASAPI_init_device(eRender, endpoint);
    if (!new_device) {
        return RtCode::ERR_NATIVE;
    }
    _render_device.reset();
    _render_device = std::move(new_device);
    return RtCode::OK;
}

netero::audio::RtCode   netero::audio::backend::impl::setCaptureDevice(const netero::audio::device& device) {
    HRESULT     result;
    IMMDevice*  endpoint = nullptr;
    bool        isLoopback = false;

    endpoint = WASAPI_get_device(eCapture, device);
    if (!endpoint) {
        endpoint = WASAPI_get_device(eRender, device);
        if (!endpoint) {
            return RtCode::ERR_NO_SUCH_DEVICE;
        }
        isLoopback = true;
    }
    std::unique_ptr<WASAPI_device> new_device = WASAPI_init_device(eCapture, endpoint, isLoopback);
    if (!new_device) {
        return RtCode::ERR_NATIVE;
    }
    _capture_device.reset();
    _capture_device = std::move(new_device);
    return RtCode::OK;
}

