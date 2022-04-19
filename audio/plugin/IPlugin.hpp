#ifndef MUSEC_AUDIO_PLUGIN_IPLUGIN
#define MUSEC_AUDIO_PLUGIN_IPLUGIN

#include "audio/base/AudioBufferView.hpp"
#include "audio/device/IDevice.hpp"

#include <array>
#include <cstdint>

namespace Musec
{
namespace Audio
{
namespace Plugin
{
template<typename SampleType>
class IPlugin: public Musec::Audio::Device::IDevice<SampleType>
{
    using Base = Musec::Audio::Device::IDevice<SampleType>;
public:
    IPlugin(): IPlugin<SampleType>::Base()
    {}
    IPlugin(const IPlugin<SampleType>&) = delete;
    IPlugin(IPlugin<SampleType>&&) = default;
    virtual ~IPlugin() noexcept override = default;
public:
    virtual bool initialize(double sampleRate, std::int32_t maxSampleCount) = 0;
    virtual bool uninitialize() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool startProcessing() = 0;
    virtual bool stopProcessing() = 0;
    virtual bool setBypass(bool bypass)
    {
        return bypass? stopProcessing(): startProcessing();
    }
};
}
}
}

#endif //MUSEC_AUDIO_PLUGIN_IPLUGIN
