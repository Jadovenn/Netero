/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <vector>
#include <netero/audio/audio.hpp>
#include <netero/audio/backend.hpp>
#include <netero/observer/signals.hpp>

namespace netero::audio {

    enum class InitStrategy {
        DEFAULT,
        NO_DEVICE
    };

    class engine {
    public:
        explicit engine(const InitStrategy strategy = InitStrategy::DEFAULT);
        ~engine();

        netero::signals<void(const WaveFormat&)> formatChangeSig;
        const std::vector<device>   &getRenderDevices();
		RtCode						setRenderCallback(const backend::RenderCallback&);
        RtCode						startRender();
        RtCode						stopRender();

        netero::signals<void(const WaveFormat&)>            captureFormatChangeSig;
        netero::signals<void(const std::string&)>           captureErrorSig;
        netero::signals<void(const float*, const size_t)>   captureStreamSig;
        RtCode                      startCapture();
        RtCode                      stopCapture();
        RtCode                      setCaptureCallback(const backend::CaptureCallback&);
        const std::vector<device>   &getCaptureDevices();
        RtCode                      setCaptureDevice(const device&);
		WaveFormat					getRenderFormat();
		WaveFormat					getCaptureFormat();


    private:
        void    captureErrorHandler(const std::string &);
        void    captureHandler(const float*, const size_t);
        backend& _backend;
    };
}

