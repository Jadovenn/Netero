/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
  * @file engine.hpp
  * @brief Audio device manager.
  */

#include <vector>

#include <netero/audio/audio.hpp>
#include <netero/audio/backend.hpp>
#include <netero/audio/device.hpp>
#include <netero/signal.hpp>

namespace netero::audio {

/**
	 * @brief DeviceManager container exposing OS specific implementation.
     * The engine class is a singleton, non default constructible, copy constructible
     * move constructible, copy assignable, move assignable.
     * @details The engine class allow you to access the audio devices.
     *  Read theirs configuration and start/stop them.
     * @see netero::audio::device
	 */
class DeviceManager {
    public:
    /**
    	 * @brief Allow you to access audio engine instance
    	 * @return A reference to an engine container.
    	 */
    static DeviceManager& getInstance() noexcept;
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager(DeviceManager&&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
    DeviceManager& operator=(DeviceManager&&) = delete;
    ~DeviceManager() = default;

    /**
    	 * @brief Return a vector of available devices available for rendering.
    	 * @return A vector of devices
    	 * @see netero::audio::device
    	 */
    [[nodiscard]] std::vector<netero::audio::device> getRenderDevices() const;

    /**
    	 * @brief Return a vector of available devices available for capturing.
    	 * @return A vector of devices
    	 * @see netero::audio::device
    	 */
    [[nodiscard]] std::vector<netero::audio::device> getCaptureDevices() const;

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

    /**
    	 * @brief Start a device for rendering.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
    RtCode deviceStartRendering(const netero::audio::device&);

    /**
    	 * @brief Stop a device that was processing a render task.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
    RtCode deviceStopRendering(const netero::audio::device&);

    /**
    	 * @brief Start a device for capturing.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
    RtCode deviceStartRecording(const netero::audio::device&);

    /**
    	 * @brief Stop a device that was processing a capture task.
    	 * @warning The device should be a rendering device or this
    	 *  function will return with error.
    	 * @return An audio error code.
    	 * @see netero::audio::RtCode
    	 */
    RtCode deviceStopRecording(const netero::audio::device&);

    /**
    	 * A signal that emit each time a physical device is disconnected/become unavailable.
    	 */
    netero::signal<void(const netero::audio::device&)> deviceDisconnectedSig;

    private:
    DeviceManager() noexcept; /**< Private default constructor.*/
    backend& _backend;        /**< Backend implementation. Specific OS implementation. */
};
} // namespace netero::audio
