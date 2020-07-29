/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceImpl.hpp"

netero::audio::Device::~Device() = default;

static std::string cf_string_to_std_string(CFStringRef &cf_str)
{
    int   length = CFStringGetLength(cf_str);
    char *c_str = new (std::nothrow) char[(length * 3 + 1)];
    if (!c_str) {
        throw std::bad_alloc();
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
    : _id(id), _isValid(true), _scope(scope)
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
            // handle output here
        }
        else if (device->_scope == kAudioObjectPropertyScopeInput) {
            // handle input here
        }
        else {
            return 1;
        }
    }
    return kAudioHardwareNoError;
}

netero::audio::Device::RtCode DeviceImpl::open()
{
    if (this->_scope == kAudioObjectPropertyScopeOutput && !this->_processingCallback) {
        return RtCode::NO_REGISTERED_CALLBACK;
    }
    if (this->_scope == kAudioObjectPropertyScopeInput && !this->_acquisitionCallback) {
        return RtCode::NO_REGISTERED_CALLBACK;
    }

    return RtCode::SUCCESS;
}

netero::audio::Device::RtCode DeviceImpl::close()
{
    return RtCode::SUCCESS;
}
