/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file engine.hpp
 * @brief A very simple and light audio engine.
 */

#include <vector>
#include <memory>
#include <type_traits>
#include <netero/audio/device.hpp>
#include <netero/audio/audioStream.hpp>
#include <netero/audio/captureStream.hpp>
#include <netero/audio/mixer.hpp>

namespace  netero::audio {

	/**
	 * @brief An audio engine.
	 * The audio engine allow you to allocate audio entities and
	 * manage theirs life duration. It also make sure that it is registered
	 * correctly to devices and automatically propagate devices event to them.
	 * You can used audio devices and audio entities by yourself without the engine, but
	 * then make sure that you never instantiate (call at least getInstance one time) the engine.
	 * @details Engine is a singleton.
	 * If the engine is deleted any associated audio entity will be automatically released.
	 */
	class engine {
	public:
		/**
		 * @brief Return the engine instance for the process.
		 */
		static netero::audio::engine&	getInstance();
		engine(const engine&) = delete;
		engine(engine&&) = delete;
		engine& operator=(const engine&) = delete;
		engine& operator=(engine&&) = delete;
		~engine() = default;

		/**
		 * Set the given device as the rendering device.
		 * @param[in] device to use for rendering.
		 * @return An audio return code.
		 * @see netero::audio::RtCode
		 */
		RtCode	setRenderDevice(const netero::audio::device &device);

		/**
		 * Set the given device as the active capture device.
		 * @param[in] device to use for capture.
		 * @return An audio return code.
		 * @see netero::audio::RtCode
		 */
		RtCode	setCaptureDevice(const netero::audio::device &device);

		/**
		 * This will stop the rendering and set the current rendering
		 * device to the null device. This will have for effect to pause any
		 * ongoing rendering process.
		 */
		RtCode	disconnectRenderDevice();

		/**
		 * This will stop the capture and set the current capture device to
		 * the null device. This will have for effect to pause the capture.
		 */
		RtCode	disconnectCaptureDevice();

		/**
		 * Construct and register and audio render entity.
		 * @tparam T Render entity type based on RenderStream.
		 * @tparam Args Variadic template types of T's constructor param.
		 * @see netero::audio::RenderStream
		 * @param args are the argument to forward to the T's constructor.
		 * @return A pointer to a T object, nullptr if alloc failed or register process failed.
		 * You may create multiple audio render entity.
		 */
		template<class T, typename ...Args,
			typename = std::enable_if<std::is_base_of<T, RenderStream>::value>>
		[[nodiscard]] T* createRenderEntity(Args... args) {
			try {
				auto entity = std::make_unique<T>(std::forward<Args>(args)...);
				T* requestEntity = entity.get();
				this->_mixer.addRenderStream(requestEntity);
				this->_renderEntities.push_back(std::move(entity));
				return requestEntity;
			}
			catch (const std::exception&) {
				throw;
			}
			catch(...) {
				return nullptr;
			}
		}

		/**
		 * Construct and register and audio capture entity.
		 * @tparam T Render entity type based on CaptureStream.
		 * @tparam Args Variadic template types of T's constructor param.
		 * @see netero::audio::CaptureStream
		 * @param args are the argument to forward to the T's constructor.
		 * @return A pointer to a T object, nullptr if alloc failed or register process failed.
		 * You can only create and use one audio capture entity at a time.
		 */
		template<class T, typename ...Args,
			typename = std::enable_if<std::is_base_of<T, CaptureStream>::value>>
		[[nodiscard]] T* createCaptureEntity(Args... args) {
			if (this->_captureEntity) { return nullptr; }
			try {
				std::unique_ptr<T> entity = std::make_unique<T>(std::forward<Args>(args)...);
				T* rawData = entity.get();
				rawData->setFormat(this->_captureDevice.format);
				this->_captureDevice.signals.captureStreamSig->connect(&entity->captureSlot);
				this->_captureDevice.signals.deviceDisconnectedSig->connect(&entity->deviceDisconnectedSlot);
				const RtCode result = audio::DeviceManager::getInstance().deviceStartRecording(this->_captureDevice);
				if (result != RtCode::OK) {
					this->_captureDevice.signals.captureStreamSig->disconnect(&this->_captureEntity->captureSlot);
					this->_captureEntity.reset();
					return nullptr;
				}
				this->_captureEntity = std::move(entity);
				if (this->_captureDevice) {
					netero::audio::DeviceManager::getInstance().deviceStartRecording(this->_captureDevice);
				}
				return rawData;
			}
			catch (const std::exception&) {
				return nullptr;
			}
			catch (...) {
				return nullptr;
			}
		}

		/**
		 * @brief Release an audio render entity.
		 * @tparam T Audio entity type based on RenderStream
		 * @param[in] entity to delete
		 * This will cleanup any connection with the audio device/mixer and release resources
		 * used by the container.
		 */
		template<class T, typename = std::enable_if<std::is_base_of<T, RenderStream>::value>>
		void	releaseRenderEntity(T* entity) {
			if (!entity) { return; }
			const auto it = std::find_if(this->_renderEntities.begin(),
				this->_renderEntities.end(), [entity] (std::unique_ptr<RenderStream> &stream) -> bool {
				return stream.get() == entity;
			});
			if (it == this->_renderEntities.end()) { return; }
			this->_mixer.removeRenderStream(entity);
			this->_renderEntities.erase(it);
		}

		/**
		 * @brief Release an audio render entity.
		 * @tparam T Audio entity type based on CaptureStream
		 * @param[in] entity to delete
		 * This will cleanup any connection with the audio capture device and release resources
		 * used by the container.
		 */
		template<class T, typename = std::enable_if<std::is_base_of<T, CaptureStream>::value>>
		void	releaseCaptureEntity(T* entity) {
			if (!entity) { return; }
			if (!this->_captureEntity) { return; }
			if (entity == this->_captureEntity.get()) {
				if (this->_captureDevice) {
					audio::DeviceManager::getInstance().deviceStopRecording(this->_captureDevice);
					this->_captureDevice.signals.captureStreamSig->disconnect(&this->_captureEntity->captureSlot);
					this->_captureDevice.signals.deviceDisconnectedSig->disconnect(&this->_captureEntity->deviceDisconnectedSlot);
				}
				this->_captureEntity.reset();
			}
		}

		[[nodiscard]] const netero::audio::device& getRenderDevice() const { return _renderDevice; }
		[[nodiscard]] const netero::audio::device& getCaptureDevice() const { return _captureDevice; }

	private:
		/**
		 * @brief Init the engine with the default audio rendering device.
		 */
		engine();
		netero::audio::device	_renderDevice; /**< Device used for rendering purpose. */
		netero::audio::device	_captureDevice; /**< Device used for capture purpose. */
		netero::audio::mixer	_mixer; /**< Mixer used for mixing signals from render entities. */
		std::vector<std::unique_ptr<RenderStream>>	_renderEntities; /**< The render entities vector. */
		std::unique_ptr<CaptureStream>				_captureEntity = nullptr; /**< Pointer to the capture entity. */
	};	
}

