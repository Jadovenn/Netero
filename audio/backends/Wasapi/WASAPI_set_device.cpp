/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
#include "WASAPI.hpp"

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

netero::audio::RtCode   netero::audio::backend::impl::setInputDevice(const netero::audio::device& device) {
    HRESULT result;

    auto item = std::find_if(_outDevices.begin(), _outDevices.end(), [device] (netero::audio::device &listed_device) {
        return device.id == listed_device.id;
    });
    if (item == _outDevices.end()) {
        return RtCode::ERR_NO_SUCH_DEVICE;
    }
    IMMDevice* endpoint = nullptr;
    endpoint = WASAPI_get_device(eRender, device);
    if (!endpoint) {
        return RtCode::ERR_NO_SUCH_DEVICE;
    }
    _render_client->Reset();
    WASAPI_release<IAudioClient>(&_render_client);
	WASAPI_release<IAudioRenderClient>(&_audio_rendering);
    WASAPI_release<IMMDevice>(&_render_device);
    if (_wfx) {
        CoTaskMemFree(_wfx);
        _wfx = nullptr;
        _wfx_ext = nullptr;
    }
    _render_device = endpoint;
    result = _render_device->Activate(IID_IAudioClient,
        CLSCTX_ALL,
        nullptr,
        reinterpret_cast<void**>(&_render_client));
    if (FAILED(result)) {
        lastError = "Could not activate selected device";
        goto exit_error;
    }
    result = _render_client->GetMixFormat(&_wfx);
    if (FAILED(result)) {
        lastError = "Could not retrieve new device format";
        goto exit_error;
    }
    if (_wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        _wfx_ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_wfx);
    }
    result = _render_client->GetDevicePeriod(nullptr, &_latency);
    test_result(result);
    if (FAILED(result)) {
        lastError = "Could not retrieve new device periode";
        goto exit_error;
    }
    result = _render_client->Initialize(AUDCLNT_SHAREMODE_SHARED,
        0,
        0,
        0,
        _wfx,
        nullptr);
    if (FAILED(result)) {
        lastError = "Could not Initialize new device.";
        goto exit_error;
    }
    result = _render_client->GetDevicePeriod(nullptr, &_latency);
    if (FAILED(result)) {
        lastError = "Could not retrieve new device frames count.";
        goto exit_error;
    }
    result = _render_client->GetService(IID_IAudioRenderClient,
        reinterpret_cast<void**>(&_audio_rendering));
    if (FAILED(result)) {
        lastError = "Could not retrieve rendering service for new device.";
        goto exit_error;
    }
    return RtCode::OK;
exit_error:
    if (_wfx) {
        CoTaskMemFree(_wfx);
        _wfx = nullptr;
        _wfx_ext = nullptr;
    }
    WASAPI_release<IAudioClient>(&_render_client);
	WASAPI_release<IAudioRenderClient>(&_audio_rendering);
    WASAPI_release<IAudioRenderClient>(&_audio_rendering);
    return RtCode::ERR_NATIVE;
}

netero::audio::RtCode   netero::audio::backend::impl::setOutputDevice(const netero::audio::device& device) {
    return RtCode::OK;
}

