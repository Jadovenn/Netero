/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include "WASAPI.hpp"

WASAPI_device::~WASAPI_device() {
	if (audioSession) {
		audioSession->UnregisterAudioSessionNotification(this);
	}
	if (renderingThread) {
		renderingState.store(state::OFF, std::memory_order_release);
		if (renderingThread->joinable()) {
			renderingThread->join();
		}
		else {
			renderingThread->detach();
		}
		renderingThread.reset();
	}
	if (capturingThread) {
		capturingState.store(state::OFF, std::memory_order_release);
		if (capturingThread->joinable()) {
			capturingThread->join();
		}
		else {
			capturingThread->detach();
		}
		capturingThread.reset();
	}
	captureStreamSig.reset();
	renderStreamSig.reset();
	deviceStreamFormatChangeSig.reset();
	deviceErrorSig.reset();
	release<IMMDevice>(&device);
	release<IAudioSessionControl>(&audioSession);
	release<IAudioClient>(&audio_client);
	release<IAudioRenderClient>(&render_client);
	release<IAudioCaptureClient>(&capture_client);
	if (wfx) {
		CoTaskMemFree(wfx);
		wfx = nullptr;
		wfx_ext = nullptr;
	}
}

ULONG	WASAPI_device::AddRef() {
	return InterlockedIncrement(&_cRef);
}

ULONG	WASAPI_device::Release() {
	ULONG ref = InterlockedDecrement(&_cRef);
	if (ref == 0) {
		delete this;
	}
	return ref;
}

HRESULT	WASAPI_device::QueryInterface(REFIID riid, void** pInterface) {
	if (IID_IUnknown == riid) {
		AddRef();
		*pInterface = dynamic_cast<IUnknown*>(this);
	}
	else if (__uuidof(IAudioSessionEvents) == riid) {
		AddRef();
		*pInterface = dynamic_cast<IAudioSessionControl*>(this);
	}
	else {
		*pInterface = nullptr;
		return E_NOINTERFACE;
	}
	return S_OK;
}

HRESULT	WASAPI_device::OnDisplayNameChanged(LPCWSTR newDisplayName, LPCGUID EventContext) {
	clientDevice.name = wstring_to_string(newDisplayName);
	return S_OK;
}

HRESULT WASAPI_device::OnIconPathChanged(LPCWSTR, LPCGUID) {
	return S_OK;
}

// Compute volume in percent -> volume * 100 + 0.5
HRESULT WASAPI_device::OnSimpleVolumeChanged(float volume, BOOL isMuted, LPCGUID eventContext) {
	return S_OK;
}

HRESULT WASAPI_device::OnChannelVolumeChanged(DWORD /*channelCount*/,
	float * /*channelsArray*/,
	DWORD /*changedChannelIndex*/,
	LPCGUID /*eventContext*/) {
	return S_OK;
}

HRESULT WASAPI_device::OnGroupingParamChanged(LPCGUID /*newGroupSetting*/, LPCGUID /*eventContext*/) {
	return S_OK;
}

HRESULT WASAPI_device::OnStateChanged(AudioSessionState state) {
	switch (state) {
	case AudioSessionStateActive:
		isSessionActive.store(true, std::memory_order_release);
		std::cout << "Active session" << std::endl;
		break;
	case AudioSessionStateInactive:
		isSessionActive.store(false, std::memory_order_release);
		std::cout << "Inactive session" << std::endl;
		break;
	}
	return S_OK;
}

HRESULT WASAPI_device::OnSessionDisconnected(AudioSessionDisconnectReason reason) {
	// Stop the rendering thread
	capturingState.store(WASAPI_device::state::OFF, std::memory_order_release);
	// Set device for collection
	backendDisconnectCallback(clientDevice);
	// Notify devices slots
	switch (reason) {
	case DisconnectReasonDeviceRemoval:
		deviceDisconnectedSig("device removed");
		break;
	case DisconnectReasonServerShutdown:
		deviceDisconnectedSig("server shutdown");
		break;
	case DisconnectReasonFormatChanged:
		deviceDisconnectedSig("format changed");
		break;
	case DisconnectReasonSessionLogoff:
		deviceDisconnectedSig("user logout");
		break;
	case DisconnectReasonSessionDisconnected:
		deviceDisconnectedSig("user terminate his session");
		break;
	case DisconnectReasonExclusiveModeOverride:
		deviceDisconnectedSig("exclusive mode override");
		break;
	}
	return S_OK;
}

