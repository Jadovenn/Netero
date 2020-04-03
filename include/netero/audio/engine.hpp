/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

 /**
  * @file engine.hpp
  * @brief Audio engine class
  */

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

        netero::signals<void(const StreamFormat&)>          renderFormatChangeSig;
        netero::signals<void(const std::string&)>           renderErrorSig;
        netero::signals<void(const StreamFormat&)>          captureFormatChangeSig;
        netero::signals<void(const std::string&)>           captureErrorSig;
        netero::signals<void(const float*, const size_t)>   captureStreamSig;

        const std::vector<device>   &getRenderDevices();
		StreamFormat			    getRenderFormat();
        RtCode                      setRenderDevice(const device&);
		RtCode						setRenderCallback(const RenderCallback&);
        RtCode						startRender();
        RtCode						stopRender();

        const std::vector<device>   &getCaptureDevices();
		StreamFormat				getCaptureFormat();
        RtCode                      setCaptureDevice(const device&);
        RtCode                      setCaptureCallback(const CaptureCallback&);
        RtCode                      startCapture();
        RtCode                      stopCapture();

    private:
        void    renderErrorHandler(const std::string&);
        void    captureErrorHandler(const std::string &);
        void    captureHandler(const float*, const size_t);
        backend& _backend;
    };
}

