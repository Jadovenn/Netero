/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

 /**
  * @file engine.hpp
  * @brief Audio engine class
  */

#include <vector>
#include <netero/observer/signal.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/device.hpp>
#include <netero/audio/backend.hpp>

namespace netero::audio {

	/**
	 * @brief Audio engine interface exposing OS specific implementation.
     * The engine class is a singleton, non default constructable, copy constructable
     * move constructable, copy assignable, move assignable.
     * @details The engine class allow you to access the audio devices.
     *  Read theirs configuration and start/stop them.
     * @see netero::audio::device
	 */
    class engine {
    public:
    	/**
    	 * @brief Allow you to access audio engine instance
    	 * @return A reference to an engine container.
    	 */
        static engine& getInstance() noexcept;
        engine(const engine&) = delete;
        engine(engine&&) = delete;
        engine& operator=(const engine &) = delete;
        engine& operator=(engine &&) = delete;
        ~engine() = default;

    	/**
    	 * @brief Return a vector of available devices available for rendering.
    	 * @return A vector of devices
    	 * @see netero::audio::device
    	 */
        [[nodiscard]] std::vector<netero::audio::device>	getRenderDevices() const;

    	/**
    	 * @brief Return a vector of available devices available for capturing.
    	 * @return A vector of devices
    	 * @see netero::audio::device
    	 */
        [[nodiscard]] std::vector<netero::audio::device>	getCaptureDevices() const;

    	/**
    	 * @brief Return the default device available for rendering.
    	 * @return The default rendering device.
    	 * @see netero::audio::device
    	 */
        [[nodiscard]] const netero::audio::device& getDefaultRenderDevice() const;

    	/**
    	 * @brief Return the default device available for capturing.
    	 * @return The default rendering device.
    	 * @see netero::audio::device
    	 */
        [[nodiscard]] const netero::audio::device& getDefaultCaptureDevice() const;

        netero::audio::device::events& getDeviceEvents(const netero::audio::device&); // may throw

    	/**
    	 * @brief Start a device for rendering.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
        RtCode  deviceStartRendering(const netero::audio::device&);

    	/**
    	 * @brief Stop a device that was processing a render task.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
        RtCode  deviceStopRendering(const netero::audio::device&);

    	/**
    	 * @brief Start a device for capturing.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
        RtCode  deviceStartRecording(const netero::audio::device&);

    	/**
    	 * @brief Stop a device that was processing a capture task.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
        RtCode  deviceStopRecording(const netero::audio::device&);

    	/**
    	 * 
    	 */
        netero::signal<void(const netero::audio::device&)>  deviceDisconnectedSig;
    private:
        engine() noexcept; /**< Private default constructor.*/
        backend& _backend; /**< Backend */
    };
}

