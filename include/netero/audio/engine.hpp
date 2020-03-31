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
        const std::vector<device>   &getOutDevices();
        const std::vector<device>   &getInDevices();
		WaveFormat					getOutputFormat();
        RtCode                      setOutputDevice(const device&);
		RtCode						setRenderCallback(const backend::RenderCallback&);
        RtCode						startRender();
        RtCode						stopRender();

    private:
        backend& _backend;
    };
}

