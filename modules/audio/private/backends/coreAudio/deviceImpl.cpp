/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceImpl.hpp"

#include <chrono>
#include <thread>

netero::audio::Device::~Device() = default;

static std::string cf_string_to_std_string(CFStringRef &cf_str)
{
    int   length = CFStringGetLength(cf_str);
    char *c_str = new (std::nothrow) char[(length * 3 + 1)];
    if (!c_str) {
        return "";
    }
#if defined(UNICODE) || defined(_UNICODE)
    CFStringGetCString(cf_str, c_str, length * 3 + 1, kCFStringEncodingUTF8);
#else
    CFStringGetCString(cf_str, c_str, length * 3 + 1, CFStringGetSystemEncoding());
#endif
    std::string str(c_str);
    delete[] c_str;
    return str;
}

DeviceImpl::DeviceImpl(UInt32 id, AudioObjectPropertyScope scope)
    : _isValid(true), _isOpen(false), _id(id), _scope(scope), _bufferIdx(0), _callbackID(nullptr)
{
    assert(_id);

    UInt32                     size = sizeof(CFStringRef);
    AudioObjectPropertyAddress property = { kAudioObjectPropertyName,
                                            this->_scope,
                                            kAudioObjectPropertyElementMaster };
    CFStringRef                cf_str;
    OSStatus result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &cf_str);
    if (result != noErr) {
        this->_isValid = false;
        return;
    }
    this->_name = cf_string_to_std_string(cf_str);
    CFRelease(cf_str);

    property.mSelector = kAudioObjectPropertyManufacturer;
    result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &cf_str);
    if (result != noErr) {
        this->_isValid = false;
        return;
    }
    this->_manufacturer = cf_string_to_std_string(cf_str);
    CFRelease(cf_str);

    AudioBufferList *bufferList = nullptr;
    property.mSelector = kAudioDevicePropertyStreamConfiguration;
    size = 0;
    result = AudioObjectGetPropertyDataSize(id, &property, 0, nullptr, &size);
    if (result != noErr || size == 0) {
        this->_isValid = false;
        return;
    }
    bufferList = new (std::nothrow) AudioBufferList[size];
    if (!bufferList) {
        this->_isValid = false;
        return;
    }
    result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, bufferList);
    if (result != noErr || size == 0) {
        delete[] bufferList;
        this->_isValid = false;
        return;
    }
    unsigned nStreams = bufferList->mNumberBuffers;
    for (unsigned idx = 0; idx < nStreams; idx++) {
        this->_format.channels = bufferList->mBuffers[idx].mNumberChannels;
        this->_format.framesCount = bufferList->mBuffers[idx].mDataByteSize;
    }
    this->_format.bytesPerSample = sizeof(float);
    this->_format.bytesPerFrame = sizeof(float) * this->_format.channels;
    delete[] bufferList;

    //======================================
    // Retrieve supported sampling rate
    // - Retrieve the supported min and max rate,
    //   then translate to a uniform list of
    //   supported rate, then choose 48000Hz rate
    //   or the higher rate by default
    //======================================
    property.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
    result = AudioObjectGetPropertyDataSize(id, &property, 0, nullptr, &size);
    if (result != kAudioHardwareNoError || size == 0) {
        this->_isValid = false;
    }
    UInt32          nRanges = size / sizeof(AudioValueRange);
    AudioValueRange rangeList[nRanges];
    result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &rangeList);
    if (result != kAudioHardwareNoError) {
        this->_isValid = false;
    }
    Float64            minimumRate = 1.0, maximumRate = 10000000000.0;
    bool               haveValueRange = false;
    std::vector<float> sampleRates;
    for (unsigned idx = 0; idx < nRanges; idx++) {
        if (rangeList[idx].mMinimum == rangeList[idx].mMaximum) {
            auto samplingRate = (float)rangeList[idx].mMinimum;
            this->_format.supportedSamplingRate.push_back(samplingRate);
            this->_format.samplingFrequency = samplingRate;
        }
        else {
            haveValueRange = true;
            if (rangeList[idx].mMinimum > minimumRate) {
                minimumRate = rangeList[idx].mMinimum;
            }
            if (rangeList[idx].mMaximum < maximumRate) {
                maximumRate = rangeList[idx].mMaximum;
            }
        }
    }
    if (haveValueRange) {
        for (auto samplingRate : SamplingRates) {
            if (samplingRate >= (unsigned)minimumRate && samplingRate <= (unsigned)maximumRate) {
                this->_format.supportedSamplingRate.push_back(samplingRate);
            }
        }
    }
    Float64 nominalRate;
    size = sizeof(Float64);
    property.mSelector = kAudioDevicePropertyNominalSampleRate;
    result = AudioObjectGetPropertyData(id, &property, 0, nullptr, &size, &nominalRate);
    if (result != noErr) {
        this->_isValid = false;
        return;
    }
    this->_format.samplingFrequency = nominalRate;

    std::sort(this->_format.supportedSamplingRate.begin(),
              this->_format.supportedSamplingRate.end());
    this->_format.supportedSamplingRate.erase(
        std::unique(this->_format.supportedSamplingRate.begin(),
                    this->_format.supportedSamplingRate.end()),
        this->_format.supportedSamplingRate.end());
    if (this->_format.supportedSamplingRate.empty()) {
        this->_isValid = false;
    }
    this->_format.supportedSamplingRate.shrink_to_fit();
}

DeviceImpl::~DeviceImpl() noexcept = default;

OSStatus DeviceImpl::NativeCallbackHandler(AudioDeviceID inDevice,
                                           const AudioTimeStamp *,
                                           const AudioBufferList *inInputData,
                                           const AudioTimeStamp *,
                                           AudioBufferList *outOutputData,
                                           const AudioTimeStamp *,
                                           void *context)
{
    auto *device = reinterpret_cast<DeviceImpl *>(context);
    if (device->_id == inDevice) {
        if (device->_scope == kAudioObjectPropertyScopeOutput) {
            auto *buffer =
                reinterpret_cast<float *>(outOutputData->mBuffers[device->_bufferIdx].mData);
            size_t frames = outOutputData->mBuffers[device->_bufferIdx].mDataByteSize /
                device->_format.bytesPerFrame;
            std::memset(buffer, 0, outOutputData->mBuffers[device->_bufferIdx].mDataByteSize);
            device->_processingCallback(buffer, frames);
        }
        else if (device->_scope == kAudioObjectPropertyScopeInput) {
            const auto *buffer =
                reinterpret_cast<const float *>(inInputData->mBuffers[device->_bufferIdx].mData);
            size_t frames = inInputData->mBuffers[device->_bufferIdx].mDataByteSize /
                device->_format.bytesPerFrame;
            device->_acquisitionCallback(buffer, frames);
        }
        else {
            return 1;
        }
    }
    return kAudioHardwareNoError;
}

static OSStatus
RateListener(AudioObjectID inDevice, UInt32, const AudioObjectPropertyAddress *, void *ratePointer)
{
    auto *                     rate = reinterpret_cast<Float64 *>(ratePointer);
    UInt32                     data_size = sizeof(Float64);
    AudioObjectPropertyAddress property = { kAudioDevicePropertyNominalSampleRate,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    AudioObjectGetPropertyData(inDevice, &property, 0, nullptr, &data_size, rate);
    return kAudioHardwareNoError;
}

netero::audio::Device::RtCode DeviceImpl::open()
{
    if (this->_isOpen) {
        return RtCode::ALREADY_OPEN;
    }
    if (this->_scope == kAudioObjectPropertyScopeOutput && !this->_processingCallback) {
        return RtCode::NO_REGISTERED_CALLBACK;
    }
    if (this->_scope == kAudioObjectPropertyScopeInput && !this->_acquisitionCallback) {
        return RtCode::NO_REGISTERED_CALLBACK;
    }

    AudioObjectPropertyAddress property = { kAudioDevicePropertyStreamConfiguration,
                                            this->_scope,
                                            kAudioObjectPropertyElementMaster };
    AudioBufferList *          bufferList = nullptr;
    UInt32                     dataSize = 0;
    OSStatus result = AudioObjectGetPropertyDataSize(this->_id, &property, 0, nullptr, &dataSize);
    if (result != noErr || dataSize == 0) {
        this->_isOpen = false;
        return RtCode::SYSTEM_ERROR;
    }
    bufferList = new (std::nothrow) AudioBufferList[dataSize];
    if (!bufferList) {
        return RtCode::SYSTEM_ERROR;
    }
    result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, bufferList);
    if (result != noErr || dataSize == 0) {
        delete[] bufferList;
        return RtCode::SYSTEM_ERROR;
    }
    int channel_index = -1;
    for (unsigned idx = 0; idx < bufferList->mNumberBuffers; idx++) {
        if (bufferList->mBuffers[idx].mNumberChannels >= this->_format.channels) {
            channel_index = (int)idx;
            break;
        }
    }

    if (channel_index < 0) {
        delete[] bufferList;
        return RtCode::SYSTEM_ERROR;
    }
    delete[] bufferList;
    this->_bufferIdx = channel_index;

    //==================================
    // Determinate buffer size
    //==================================
    AudioValueRange bufferRange;
    dataSize = sizeof(AudioValueRange);
    property.mSelector = kAudioDevicePropertyBufferFrameSizeRange;
    result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, &bufferRange);
    if (result != noErr) {
        return RtCode::SYSTEM_ERROR;
    }
    auto buffer_frame_size = static_cast<UInt32>(bufferRange.mMaximum);
    dataSize = sizeof(UInt32);
    property.mSelector = kAudioDevicePropertyBufferFrameSize;
    result =
        AudioObjectSetPropertyData(this->_id, &property, 0, nullptr, dataSize, &buffer_frame_size);
    if (result != noErr) { // error while setting the buffer size
        return RtCode::SYSTEM_ERROR;
    }

    //==================================
    // Set the sampling rate
    //==================================
    Float64 nominalRate;
    dataSize = sizeof(Float64);
    property.mSelector = kAudioDevicePropertyNominalSampleRate;
    result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, &nominalRate);
    if (result != noErr) { // Error while getting the current sample rate
        return RtCode::SYSTEM_ERROR;
    }
    if (nominalRate != this->_format.samplingFrequency) {
        // if device sampling rate different, reconfigure it
        // here start the tricky thing, the rate is setup by a callback
        // So we register a call back, ask to change the rate, wait until the rate change
        // And finally unregister the callback
        Float64                    reportedRate = 0.0;
        AudioObjectPropertyAddress cb_properties = { kAudioDevicePropertyNominalSampleRate,
                                                     kAudioObjectPropertyScopeGlobal,
                                                     kAudioObjectPropertyElementMaster };
        result = AudioObjectAddPropertyListener(this->_id,
                                                &cb_properties,
                                                RateListener,
                                                reinterpret_cast<void *>(&reportedRate));
        if (result != noErr) { // Error could not register the callback
            return RtCode::SYSTEM_ERROR;
        }
        nominalRate = (Float64)this->_format.samplingFrequency;
        result =
            AudioObjectSetPropertyData(this->_id, &property, 0, nullptr, dataSize, &nominalRate);
        if (result != noErr) { // Error could not set new sampling rate
            AudioObjectRemovePropertyListener(this->_id,
                                              &cb_properties,
                                              RateListener,
                                              reinterpret_cast<void *>(&reportedRate));
            return RtCode::SYSTEM_ERROR;
        }
        std::chrono::time_point start = std::chrono::system_clock::now();
        std::chrono::time_point ref = start;
        while (reportedRate != nominalRate) {
            ref = std::chrono::system_clock::now();
            if (std::chrono::duration_cast<std::chrono::microseconds>(ref - start).count() < 5000) {
                break;
            }
            else {
                std::this_thread::yield();
            }
        }
        AudioObjectRemovePropertyListener(this->_id,
                                          &cb_properties,
                                          RateListener,
                                          reinterpret_cast<void *>(&reportedRate));
        if (std::chrono::duration_cast<std::chrono::microseconds>(ref - start).count() < 5000) {
            return RtCode::TIME_OUT;
        }
    }

    AudioStreamBasicDescription description;
    dataSize = sizeof(AudioStreamBasicDescription);
    property.mSelector = kAudioStreamPropertyVirtualFormat;
    result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, &description);
    if (result != noErr) { // Error getting stream format
        return RtCode::SYSTEM_ERROR;
    }
    description.mSampleRate = (Float64)this->_format.samplingFrequency;
    description.mFormatID = kAudioFormatLinearPCM;
    result = AudioObjectSetPropertyData(this->_id, &property, 0, nullptr, dataSize, &description);
    if (result != noErr) { // Error while setting stream data rate and format
        return RtCode::SYSTEM_ERROR;
    }

    property.mSelector = kAudioStreamPropertyPhysicalFormat;
    result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, &description);
    if (result != noErr) { // Error while getting physical device format
        return RtCode::SYSTEM_ERROR;
    }

    //==============================================
    // Check that the physical format match our need
    //==============================================

    if (description.mFormatID != kAudioFormatLinearPCM || description.mBitsPerChannel < 16) {
        description.mFormatID = kAudioFormatLinearPCM;
        UInt32 formatFlags;

        std::vector<std::pair<UInt32, UInt32>> physicalFormats;
        formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsFloat) &
            ~kLinearPCMFormatFlagIsSignedInteger;
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(32, formatFlags)); // PCM 32 bits float
        formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsSignedInteger |
                       kAudioFormatFlagIsPacked) &
            ~kLinearPCMFormatFlagIsFloat;
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(32, formatFlags)); // PCM 32 bits packed signed integer
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(24, formatFlags)); // PCM 24 bits packed signed integer
        formatFlags &= ~(kAudioFormatFlagIsPacked | kAudioFormatFlagIsAlignedHigh);
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(24.2, formatFlags)); // PCM 24 bits over 4bytes, aligned low
        formatFlags |= kAudioFormatFlagIsAlignedHigh;
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(24.4, formatFlags)); // PCM 24 bits over 4bytes, aligned high
        formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsSignedInteger |
                       kAudioFormatFlagIsPacked) &
            ~kLinearPCMFormatFlagIsFloat;
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(16, formatFlags)); // PCM 16 bits signed integer
        physicalFormats.emplace_back(
            std::pair<Float32, UInt32>(8, formatFlags)); // PCM 8 bits signed integer

        bool setPhysicalFormat = false;
        for (auto &format : physicalFormats) {
            AudioStreamBasicDescription testDescription = description;
            testDescription.mBitsPerChannel = static_cast<UInt32>(format.first);
            testDescription.mFormatFlags = format.second;
            if ((24 == (UInt32)format.first) && ~(format.second & kAudioFormatFlagIsPacked)) {
                testDescription.mBytesPerFrame = 4 * testDescription.mChannelsPerFrame;
            }
            else {
                testDescription.mBytesPerFrame =
                    testDescription.mBitsPerChannel / 8 * testDescription.mChannelsPerFrame;
            }
            testDescription.mBytesPerPacket =
                testDescription.mBytesPerFrame * testDescription.mFramesPerPacket;
            result = AudioObjectSetPropertyData(this->_id,
                                                &property,
                                                0,
                                                nullptr,
                                                dataSize,
                                                &testDescription);
            if (result == noErr) {
                setPhysicalFormat = true;
                break;
            }
        }

        if (!setPhysicalFormat) {
            return RtCode::SYSTEM_ERROR;
        }
    }

    UInt32 latency;
    dataSize = sizeof(UInt32);
    property.mSelector = kAudioDevicePropertyLatency;
    if (AudioObjectHasProperty(this->_id, &property)) {
        result = AudioObjectGetPropertyData(this->_id, &property, 0, nullptr, &dataSize, &latency);
        if (result == kAudioHardwareNoError) {
            (void)latency; // if latency different from what is actually set change it
        }
        else {
            return RtCode::SYSTEM_ERROR;
        }
    }
    result = AudioDeviceCreateIOProcID(this->_id,
                                       NativeCallbackHandler,
                                       reinterpret_cast<void *>(this),
                                       &this->_callbackID);
    if (result != noErr) {
        return RtCode::SYSTEM_ERROR;
    }
    result = AudioDeviceStart(this->_id, this->_callbackID);
    if (result != noErr) {
        return RtCode::SYSTEM_ERROR;
    }
    this->_isOpen = true;
    return RtCode::SUCCESS;
}

netero::audio::Device::RtCode DeviceImpl::close()
{
    if (!this->_isOpen) {
        return RtCode::NOT_OPEN;
    }
    AudioDeviceStop(this->_id, this->_callbackID);
    AudioDeviceDestroyIOProcID(this->_id, this->_callbackID);
    this->_callbackID = nullptr;
    this->_bufferIdx = 0;
    return RtCode::SUCCESS;
}
