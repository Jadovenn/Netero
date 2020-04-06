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
#include <netero/audio/device.hpp>
#include <netero/audio/backend.hpp>

namespace netero::audio {

    enum class InitStrategy {
        DEFAULT,
        NO_DEVICE
    };

    class engine {
    public:
        explicit engine(const InitStrategy strategy = InitStrategy::DEFAULT);
        ~engine();

        const std::vector<device>	getRenderDevices();
        const std::vector<device>	getCaptureDevices();
        const device& getDefaultRenderDevice();
        const device& getDefaultCaptureDevice();
        netero::audio::device::events& getDeviceEvents(const netero::audio::device&); // may throw
        RtCode  deviceStartRendering(const device&);
        RtCode  deviceStopRendering(const device&);

        RtCode  deviceStartRecording(const device&);
        RtCode  deviceStopRecording(const device&);
    private:
        backend& _backend;
    };
}

