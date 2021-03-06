#include "VST3Plugin.hpp"

#include "audio/host/MusecVST3Host.hpp"
#include "controller/AudioEngineController.hpp"

#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/gui/iplugview.h>
#include <pluginterfaces/vst/vstspeaker.h>

#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/common/memorystream.cpp>

#include <stdexcept>

namespace Musec::Audio::Plugin
{
// VST3Plugin ctor & dtor
// ------------------------------------------------------------------------------------------
template<typename SampleType>
VST3Plugin<SampleType>::VST3Plugin():
    VST3Plugin::Base(), VST3Plugin::IPluginInterface()
{
}

template<typename SampleType>
VST3Plugin<SampleType>::VST3Plugin(const QString& path, int classIndex):
    VST3Plugin::Base(path), VST3Plugin::IPluginInterface()
{
    auto pluginInitProc = Musec::Native::getExport<Musec::Base::VST3PluginInitProc>(*this, "InitDll");
    auto pluginFactoryProc = Musec::Native::getExport<Musec::Base::VST3PluginFactoryProc>(*this, "GetPluginFactory");
    if (!pluginFactoryProc)
    {
        auto error = GetLastError();
        switch (error)
        {
        case ERROR_PROC_NOT_FOUND:
            break;
        default:
            break;
        }
        throw std::invalid_argument("");
    }
    if (pluginInitProc)
    {
        // Windows 平台可省略入口和出口函数的调用，但 macOS 和 Linux 不行
        // macOS 和 Linux 平台的入口和出口函数的名字与 Windows 的不同
        // macOS: bundleEntry 和 BundleExit
        // Linux: ModuleEntry 和 ModuleExit
        // https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/VST+Module+Architecture/Loading.html
        if (!pluginInitProc())
        {
            // 初始化出现问题
            throw std::runtime_error("");
        }
    }
    factory_ = pluginFactoryProc();
    audioProcessorStatus_ = VST3AudioProcessorStatus::Factory;
    editControllerStatus_ = VST3EditControllerStatus::Factory;
    auto classCount = factory_->countClasses();
    if (classIndex >= classCount)
    {
        throw std::runtime_error("");
    }
    factory_->getClassInfo(classIndex, &classInfo_);
    auto createComponentResult = factory_->createInstance(classInfo_.cid,
        Steinberg::Vst::IComponent::iid, reinterpret_cast<void**>(&component_));
    if(createComponentResult != Steinberg::kResultOk)
    {
        throw std::runtime_error("Error creating audio processor instance!");
    }
    audioProcessorStatus_ = VST3AudioProcessorStatus::Created;
}

template<typename SampleType>
VST3Plugin<SampleType>::~VST3Plugin()
{
    if(editControllerStatus_ == VST3EditControllerStatus::Connected)
    {
        uninitializeEditor();
    }
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::Processing)
    {
        stopProcessing();
    }
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::Activated)
    {
        deactivate();
    }
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::SetupDone)
    {
        uninitialize();
    }
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::Created)
    {
        component_->release();
        audioProcessorStatus_ = VST3AudioProcessorStatus::Factory;
    }
    if (factory_)
    {
        factory_->release();
        audioProcessorStatus_ = VST3AudioProcessorStatus::NoAudioProcessor;
        editControllerStatus_ = VST3EditControllerStatus::NoEditController;
        auto pluginExitProc = Musec::Native::getExport<Musec::Base::VST3PluginExitProc>(*this, "ExitDll");
        if (pluginExitProc)
        {
            pluginExitProc();
        }
    }
}
// ------------------------------------------------------------------------------------------

// VST3Plugin raw pointer type
// ------------------------------------------------------------------------------------------
template<typename SampleType>
Steinberg::Vst::IAudioProcessor* VST3Plugin<SampleType>::effect() const
{
    return audioProcessor_;
}

template<typename SampleType>
Steinberg::Vst::IComponent* VST3Plugin<SampleType>::component() const
{
    return component_;
}

template<typename SampleType>
Steinberg::Vst::IEditController* VST3Plugin<SampleType>::editController() const
{
    return editController_;
}
// ------------------------------------------------------------------------------------------

// VST3Plugin IDevice interfaces
// ------------------------------------------------------------------------------------------
template<typename SampleType>
std::uint8_t VST3Plugin<SampleType>::inputCount() const
{
    std::uint8_t ret = 0;
    for (auto& arrangement: inputSpeakerArrangements_)
    {
        ret += Steinberg::Vst::SpeakerArr::getChannelCount(arrangement);
    }
    return ret;
}

template<typename SampleType>
std::uint8_t VST3Plugin<SampleType>::outputCount() const
{
    std::uint8_t ret = 0;
    for (auto& arrangement: outputSpeakerArrangements_)
    {
        ret += Steinberg::Vst::SpeakerArr::getChannelCount(arrangement);
    }
    return ret;
}

template<typename SampleType>
void VST3Plugin<SampleType>::process(const Musec::Audio::Base::AudioBufferViews<SampleType>& inputs,
    const Musec::Audio::Base::AudioBufferViews<SampleType>& outputs)
{
    processData_.numSamples = Musec::Controller::AudioEngineController::getCurrentBlockSize();
    auto inputBusSize = inputs_.size();
    auto outputBusSize = outputs_.size();
    for(auto i = 0; i < inputBusSize; ++i)
    {
        if(i == audioInputBusIndex)
        {
            for(auto j = 0; j < inputRaws_[i].size(); ++j)
            {
                if(j < inputs.size())
                {
                    inputRaws_[i][j] = inputs[j].getSamples();
                }
                else
                {
                    inputRaws_[i][j] = Musec::Controller::AudioEngineController::dummyBufferView<SampleType>().getSamples();
                }
            }
        }
        else
        {
            for(auto j = 0; j < inputRaws_[i].size(); ++j)
            {
                inputRaws_[i][j] = Musec::Controller::AudioEngineController::dummyBufferView<SampleType>().getSamples();
            }
        }
        inputs_[i].numChannels = inputRaws_[i].size();
        inputs_[i].channelBuffers32 = reinterpret_cast<float**>(inputRaws_[i].data());
    }
    for(auto i = 0; i < outputBusSize; ++i)
    {
        if(i == audioOutputBusIndex)
        {
            for(auto j = 0; j < outputRaws_[i].size(); ++j)
            {
                if(j < outputs.size())
                {
                    outputRaws_[i][j] = outputs[j].getSamples();
                }
                else
                {
                    outputRaws_[i][j] = Musec::Controller::AudioEngineController::dummyBufferView<SampleType>().getSamples();
                }
            }
        }
        else
        {
            for(auto j = 0; j < outputRaws_[i].size(); ++j)
            {
                outputRaws_[i][j] = Musec::Controller::AudioEngineController::dummyBufferView<SampleType>().getSamples();
            }
        }
        outputs_[i].numChannels = outputRaws_[i].size();
        outputs_[i].channelBuffers32 = reinterpret_cast<float**>(outputRaws_[i].data());
    }
    processData_.numInputs = inputs_.size();
    processData_.numOutputs = outputs_.size();
    processData_.inputs = inputs_.data();
    processData_.outputs = outputs_.data();
    auto& processContext = Musec::Audio::Host::MusecVST3Host::AppProcessContext();
    processData_.processContext = &processContext;
    audioProcessor_->process(processData_);
    if(editController_)
    {
        auto paramCount = outputParameterChanges_.getParameterCount();
        for(decltype(paramCount) i = 0; i < paramCount; ++i)
        {
            auto* paramValueQueue = outputParameterChanges_.getParameterData(i);
            auto paramId = paramValueQueue->getParameterId();
            Steinberg::int32 sampleOffset = 0;
            Steinberg::Vst::ParamValue paramValue;
            paramValueQueue->getPoint(i, sampleOffset, paramValue);
            editController_->setParamNormalized(paramId, paramValue);
        }
    }
}

// ------------------------------------------------------------------------------------------

// VST3Plugin IPlugin interfaces
// ------------------------------------------------------------------------------------------
template<typename SampleType>
bool VST3Plugin<SampleType>::initialize(double sampleRate, std::int32_t maxSampleCount)
{
    auto initializeComponentResult = component_->initialize(&Musec::Audio::Host::MusecVST3Host::instance());
    if(initializeComponentResult != Steinberg::kResultOk)
    {
        return false;
    }
    auto queryAudioProcessorResult = component_->queryInterface(
        Steinberg::Vst::IAudioProcessor::iid,
        reinterpret_cast<void**>(&audioProcessor_));
    if(queryAudioProcessorResult != Steinberg::kResultOk)
    {
        return false;
    }
    audioProcessorStatus_ = VST3AudioProcessorStatus::Initialized;
    if constexpr(std::is_same_v<SampleType, float>)
    {
        if(audioProcessor_->canProcessSampleSize(Steinberg::Vst::SymbolicSampleSizes::kSample32)
            == Steinberg::kResultFalse)
        {
            return false;
        }
    }
    if constexpr(std::is_same_v<SampleType, double>)
    {
        if(audioProcessor_->canProcessSampleSize(Steinberg::Vst::SymbolicSampleSizes::kSample64)
            == Steinberg::kResultFalse)
        {
            return false;
        }
    }
    if(initializeEditor())
    {
        paramCount_ = editController_->getParameterCount();
        paramBlock_ = Musec::Base::FixedSizeMemoryBlock((sizeof(Steinberg::Vst::ParamValue) + sizeof(Steinberg::Vst::ParameterInfo)) * paramCount_);
        auto* paramValue = reinterpret_cast<Steinberg::Vst::ParamValue*>(paramBlock_.data());
        auto* paramInfo = reinterpret_cast<Steinberg::Vst::ParameterInfo*>(paramValue + paramCount_);
        for(decltype(paramCount_) i = 0; i < paramCount_; ++i)
        {
            editController_->getParameterInfo(i, paramInfo[i]);
            paramValue[i] = editController_->getParamNormalized(paramInfo[i].id);
        }
    }
    audioProcessor_->queryInterface(Steinberg::Vst::IProcessContextRequirements::iid,
        reinterpret_cast<void**>(&processContextRequirements_));
    audioProcessor_->queryInterface(Steinberg::Vst::IAudioPresentationLatency::iid,
        reinterpret_cast<void**>(&audioPresentationLatency_));
    // ProcessSetup -------------------------------------------------------------------------
    Steinberg::Vst::ProcessSetup setup;
    setup.processMode = Steinberg::Vst::ProcessModes::kRealtime;
    if constexpr(std::is_same_v<SampleType, float>)
    {
        setup.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32;
    }
    else
    {
        setup.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample64;
    }
    setup.maxSamplesPerBlock = maxSampleCount;
    setup.sampleRate = sampleRate;
    // ProcessSetup -------------------------------------------------------------------------
    // ProcessData --------------------------------------------------------------------------
    processData_.inputParameterChanges = &inputParameterChanges_;
    processData_.outputParameterChanges = &outputParameterChanges_;
    processData_.processMode = setup.processMode;
    processData_.symbolicSampleSize = setup.symbolicSampleSize;
    auto inputBusCount = component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kInput);
    auto outputBusCount = component_->getBusCount(Steinberg::Vst::MediaTypes::kAudio,
        Steinberg::Vst::BusDirections::kOutput);
    inputRaws_ = decltype(inputRaws_)(inputBusCount);
    outputRaws_ = decltype(outputRaws_)(outputBusCount);
    inputSpeakerArrangements_.resize(inputBusCount, Steinberg::Vst::SpeakerArr::kStereo);
    outputSpeakerArrangements_.resize(outputBusCount, Steinberg::Vst::SpeakerArr::kStereo);
    inputs_ = decltype(inputs_)(inputBusCount);
    outputs_ = decltype(outputs_)(outputBusCount);
    // setupProcessing ------------------------------------------------------------------------------------------------- setupProcessing
    auto setupProcessingResult = audioProcessor_->setupProcessing(setup);
    if (setupProcessingResult != Steinberg::kResultOk
        && setupProcessingResult != Steinberg::kNotImplemented)
    {
        throw std::runtime_error("");
    }
    if(processContextRequirements_)
    {
        processContextRequirement_ = processContextRequirements_->getProcessContextRequirements();
    }
    auto setBusArrangementsResult = audioProcessor_->setBusArrangements(
        inputSpeakerArrangements_.data(), processData_.numInputs,
        outputSpeakerArrangements_.data(), processData_.numOutputs);
    decltype(setBusArrangementsResult) getBusArrangementResult = Steinberg::kResultOk;
    if(setBusArrangementsResult != Steinberg::kNotImplemented)
    {
        if (setBusArrangementsResult != Steinberg::kResultOk)
        {
            // 再次调用 getBusArrangement，用于宿主程序与插件之间协商总线布局
            // 参考：https://developer.steinberg.help/pages/viewpage.action?pageId=49906849
            // My plug-in is capable of processing all possible channel configurations
            // https://developer.steinberg.help/display/VST/Bus+Arrangement+Setting+Sequences
            for (decltype(outputBusCount) i = 0; i < outputBusCount; ++i)
            {
                getBusArrangementResult = audioProcessor_->getBusArrangement(
                    Steinberg::Vst::BusDirections::kOutput, i, outputSpeakerArrangements_[i]);
                if(getBusArrangementResult == Steinberg::kNotImplemented)
                {
                    break;
                }
            }
            if(getBusArrangementResult != Steinberg::kNotImplemented)
            {
                for(decltype(inputBusCount) i = 0; i < inputBusCount; ++i)
                {
                    audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kInput, i,
                        inputSpeakerArrangements_[i]);
                }
            }
            setBusArrangementsResult = audioProcessor_->setBusArrangements(
                inputSpeakerArrangements_.data(), inputSpeakerArrangements_.size(),
                outputSpeakerArrangements_.data(), outputSpeakerArrangements_.size());
            if(setBusArrangementsResult != Steinberg::kResultOk)
            {
                return false;
            }
        }
    }
    if(getBusArrangementResult == Steinberg::kResultOk)
    {
        for (decltype(inputBusCount) i = 0; i < inputBusCount; ++i)
        {
            audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kInput, i,
                inputSpeakerArrangements_[i]);
            inputs_[i].numChannels = Steinberg::Vst::SpeakerArr::getChannelCount(inputSpeakerArrangements_[i]);
        }
        for (decltype(outputBusCount) i = 0; i < outputBusCount; ++i)
        {
            audioProcessor_->getBusArrangement(Steinberg::Vst::BusDirections::kOutput, i,
                outputSpeakerArrangements_[i]);
            outputs_[i].numChannels = Steinberg::Vst::SpeakerArr::getChannelCount(outputSpeakerArrangements_[i]);
        }
    }
    else
    {
        throw std::runtime_error("");
    }
    // 本人电脑中的 VST3 插件大多不支持获取 RoutingInfo
    // Steinberg::Vst::RoutingInfo inputRoutingInfo = {0, 0, 0};
    // Steinberg::Vst::RoutingInfo outputRoutingInfo = {0, 0, 0};
    // auto getRoutingInfoResult = component_->getRoutingInfo(inputRoutingInfo, outputRoutingInfo);
    Steinberg::Vst::BusInfo busInfo;
    for(int i = 0; i < inputBusCount; ++i)
    {
        auto getBusInfoResult = component_->getBusInfo(Steinberg::Vst::MediaTypes::kAudio,
            Steinberg::Vst::BusDirections::kInput, i, busInfo
        );
        if(getBusInfoResult == Steinberg::kResultOk
        && busInfo.busType == Steinberg::Vst::BusTypes::kMain)
        {
            audioInputBusIndex = i;
        }
        inputs_[i].numChannels = busInfo.channelCount;
        inputRaws_[i] = SampleTypePointers(busInfo.channelCount, nullptr);
        component_->activateBus(Steinberg::Vst::MediaTypes::kAudio,
            Steinberg::Vst::BusDirections::kInput, i, true);
    }
    for(int i = 0; i < outputBusCount; ++i)
    {
        auto getBusInfoResult = component_->getBusInfo(Steinberg::Vst::MediaTypes::kAudio,
                Steinberg::Vst::BusDirections::kOutput, i, busInfo);
        if(getBusInfoResult == Steinberg::kResultOk
        && busInfo.busType == Steinberg::Vst::BusTypes::kMain)
        {
            audioOutputBusIndex = i;
            component_->activateBus(Steinberg::Vst::MediaTypes::kAudio,
                Steinberg::Vst::BusDirections::kOutput, i, true);
        }
        else if(busInfo.flags & Steinberg::Vst::BusInfo::BusFlags::kDefaultActive)
        {
            component_->activateBus(Steinberg::Vst::MediaTypes::kAudio,
                Steinberg::Vst::BusDirections::kOutput, i, true);
        }
        outputs_[i].numChannels = busInfo.channelCount;
        outputRaws_[i] = SampleTypePointers(busInfo.channelCount, nullptr);
    }
    auto inputEventBusCount = component_->getBusCount(Steinberg::Vst::MediaTypes::kEvent,
        Steinberg::Vst::BusDirections::kInput);
    auto outputEventBusCount = component_->getBusCount(Steinberg::Vst::MediaTypes::kEvent,
        Steinberg::Vst::BusDirections::kOutput);
    for(decltype(inputEventBusCount) i = 0; i < inputEventBusCount; ++i)
    {
        auto getBusInfoResult = component_
            ->getBusInfo(Steinberg::Vst::MediaTypes::kEvent, Steinberg::Vst::BusDirections::kInput, i, busInfo);
        if(getBusInfoResult == Steinberg::kResultOk
            && (busInfo.busType == Steinberg::Vst::BusTypes::kMain
                || busInfo.flags & Steinberg::Vst::BusInfo::BusFlags::kDefaultActive))
        {
            auto activateBusResult = component_->activateBus(
                Steinberg::Vst::MediaTypes::kEvent, Steinberg::Vst::BusDirections::kInput, i, true);
            if(activateBusResult != Steinberg::kResultOk)
            {
                return false;
            }
        }
    }
    for(decltype(outputEventBusCount) i = 0; i < outputEventBusCount; ++i)
    {
        auto getBusInfoResult = component_->getBusInfo(Steinberg::Vst::MediaTypes::kEvent,
            Steinberg::Vst::BusDirections::kOutput, i, busInfo);
        if(getBusInfoResult == Steinberg::kResultOk
            && (busInfo.busType == Steinberg::Vst::BusTypes::kMain
                || busInfo.flags & Steinberg::Vst::BusInfo::BusFlags::kDefaultActive))
        {
            auto activateBusResult = component_->activateBus(
                Steinberg::Vst::MediaTypes::kEvent, Steinberg::Vst::BusDirections::kOutput, i, true);
            if(activateBusResult != Steinberg::kResultOk)
            {
                return false;
            }
        }
    }
    audioProcessorStatus_ = VST3AudioProcessorStatus::SetupDone;
    return true;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::uninitialize()
{
    if(editControllerStatus_ == VST3EditControllerStatus::Initialized)
    {
        uninitializeEditor();
    }
    audioProcessorStatus_ = VST3AudioProcessorStatus::Initialized;
    audioProcessor_->release();
    auto terminateComponentResult = component_->terminate();
    if(terminateComponentResult != Steinberg::kResultOk)
    {
        throw std::runtime_error("");
    }
    audioProcessorStatus_ = VST3AudioProcessorStatus::Created;
    auto ret = (terminateComponentResult == Steinberg::kResultOk);
    return ret;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::initializeEditor()
{
    Steinberg::TUID controllerId;
    if (component_->getControllerClassId(controllerId) == Steinberg::kResultOk)
    {
        auto createEditControllerInstanceResult = factory_->createInstance(
            controllerId, Steinberg::Vst::IEditController::iid,
            reinterpret_cast<void**>(&editController_));
        if(createEditControllerInstanceResult != Steinberg::kResultOk)
        {
            return false;
        }
        else
        {
            editControllerStatus_ = VST3EditControllerStatus::Created;
        }
    }
    else
    {
        auto queryEditorFromComponentResult = component_->queryInterface(Steinberg::Vst::IEditController::iid,
            reinterpret_cast<void**>(&editController_));
        if (queryEditorFromComponentResult == Steinberg::kResultOk)
        {
            editControllerStatus_ = VST3EditControllerStatus::Created;
            effectAndEditorUnified_ = EffectAndEditorUnified::Unified;
        }
    }
    if (editController_)
    {
        editController_->queryInterface(Steinberg::Vst::IEditController2::iid,
            reinterpret_cast<void**>(&editController2_));
        editController_->queryInterface(Steinberg::Vst::IMidiMapping::iid,
            reinterpret_cast<void**>(&midiMapping_));
        editController_->queryInterface(Steinberg::Vst::IEditControllerHostEditing::iid,
            reinterpret_cast<void**>(&editControllerHostEditing_));
        editController_->queryInterface(Steinberg::Vst::INoteExpressionController::iid,
            reinterpret_cast<void**>(&noteExpressionController_));
        editController_->queryInterface(Steinberg::Vst::IKeyswitchController::iid,
            reinterpret_cast<void**>(&keyswitchController_));
        editController_->queryInterface(Steinberg::Vst::IXmlRepresentationController::iid,
            reinterpret_cast<void**>(&xmlRepresentationController_));
        if(effectAndEditorUnified_ != EffectAndEditorUnified::Unified)
        {
            auto initEditControllerResult = editController_->initialize(&Musec::Audio::Host::MusecVST3Host::instance());
            if(initEditControllerResult != Steinberg::kResultOk)
            {
                throw std::runtime_error("");
            }
        }
        editControllerStatus_ = VST3EditControllerStatus::Initialized;
        editController_->setComponentHandler(this);
        component_->queryInterface(Steinberg::Vst::IConnectionPoint::iid,
                                   reinterpret_cast<void**>(&componentPoint_));
        editController_->queryInterface(Steinberg::Vst::IConnectionPoint::iid,
                                        reinterpret_cast<void**>(&editControllerPoint_));
        if(componentPoint_ && editControllerPoint_)
        {
            componentPoint_->addRef();
            editControllerPoint_->addRef();
            // connect ------------------------------------------------------------------------------------------------- connect
            componentPoint_->connect(editControllerPoint_);
            editControllerPoint_->connect(componentPoint_);
            audioProcessorStatus_ = VST3AudioProcessorStatus::Connected;
            editControllerStatus_ = VST3EditControllerStatus::Connected;
        }
        Steinberg::MemoryStream memoryStream;
        if(component_->getState(&memoryStream) == Steinberg::kResultOk)
        {
            editController_->setComponentState(&memoryStream);
        }
        view_ = editController_->createView(Steinberg::Vst::ViewType::kEditor);
        if (view_)
        {
            view_->addRef();
            view_->setFrame(this);
        }
    }
    return true;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::uninitializeEditor()
{
    detachWithWindow();
    if(componentPoint_ && editControllerPoint_)
    {
        componentPoint_->disconnect(editControllerPoint_);
        editControllerPoint_->disconnect(componentPoint_);

        componentPoint_->release();
        editControllerPoint_->release();
        componentPoint_ = nullptr;
        editControllerPoint_ = nullptr;
        editControllerStatus_ = VST3EditControllerStatus::Initialized;
    }
    Steinberg::tresult terminateEditControllerResult = Steinberg::kResultOk;
    if(editController_ && (effectAndEditorUnified_ != EffectAndEditorUnified::Unified))
    {
        terminateEditControllerResult = editController_->terminate();
        if(terminateEditControllerResult != Steinberg::kResultOk)
        {
            throw std::runtime_error("");
        }
    }
    editControllerStatus_ = VST3EditControllerStatus::Created;
    return true;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::activate()
{
    auto ret = (component_->setActive(true));
    if(ret == Steinberg::kResultOk)
    {
        audioProcessorStatus_ = VST3AudioProcessorStatus::Activated;
    }
    return ret == Steinberg::kResultOk;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::deactivate()
{
    auto ret = component_->setActive(false);
    if(ret == Steinberg::kResultOk)
    {
        audioProcessorStatus_ = VST3AudioProcessorStatus::SetupDone;
    }
    return ret == Steinberg::kResultOk;
}

// 或许上 RAII 更合适？
template<typename SampleType>
bool VST3Plugin<SampleType>::startProcessing()
{
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::Activated)
    {
        auto ret = audioProcessor_->setProcessing(true);
        if(ret == Steinberg::kResultOk
        // VST3 SDK 中，IAudioProcessor 的实现 AudioEffect
        // 在调用此函数时只返回 kNotImplemented。
        // 一些插件厂商直接套用了 AudioEffect 原来的实现，并且没有改动此处的代码。
        // Steinberg 的文档中没有提醒开发者“需要改动这一处函数”。
        || ret == Steinberg::kNotImplemented)
        {
            audioProcessorStatus_ = VST3AudioProcessorStatus::Processing;
            return true;
        }
    }
    return false;
}

// 或许上 RAII 更合适？
template<typename SampleType>
bool VST3Plugin<SampleType>::stopProcessing()
{
    if(audioProcessorStatus_ == VST3AudioProcessorStatus::Processing)
    {
        auto ret = audioProcessor_->setProcessing(false);
        if(ret == Steinberg::kResultOk
        // 参阅 startProcessing 中的说明
        || ret == Steinberg::kNotImplemented)
        {
            audioProcessorStatus_ = VST3AudioProcessorStatus::Activated;
            return true;
        }
    }
    return false;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::queryInterface(const Steinberg::int8* iid, void** obj)
{
    if(iid == Steinberg::Vst::IComponentHandler::iid || iid == Steinberg::IPlugFrame::iid || iid == Steinberg::FUnknown::iid)
    {
        *obj = this;
        return Steinberg::kResultOk;
    }
    return Steinberg::kNoInterface;
}

template<typename SampleType>
Steinberg::uint32 VST3Plugin<SampleType>::addRef()
{
    return 1;
}

template<typename SampleType>
Steinberg::uint32 VST3Plugin<SampleType>::release()
{
    return 1;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::beginEdit(Steinberg::Vst::ParamID id)
{
    return Steinberg::kNotImplemented;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::performEdit(Steinberg::Vst::ParamID id,
    Steinberg::Vst::ParamValue valueNormalized)
{
    return Steinberg::kNotImplemented;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::endEdit(Steinberg::Vst::ParamID id)
{
    return Steinberg::kNotImplemented;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::restartComponent(Steinberg::int32 flags)
{
    return Steinberg::kNotImplemented;
}

template<typename SampleType>
Steinberg::tresult VST3Plugin<SampleType>::resizeView(Steinberg::IPlugView* view, Steinberg::ViewRect* newSize)
{
    Steinberg::ViewRect oldSize; view->getSize(&oldSize);
    window_->setWidth(newSize->getWidth());
    window_->setHeight(newSize->getHeight());
    view->onSize(newSize);
    Steinberg::ViewRect newSize2; view->getSize(&newSize2);
    return Steinberg::kResultOk;
}

template<typename SampleType>
const SpeakerArrangements& VST3Plugin<SampleType>::inputSpeakerArrangements()
{
    return inputSpeakerArrangements_;
}

template<typename SampleType>
const SpeakerArrangements& VST3Plugin<SampleType>::outputSpeakerArrangements()
{
    return outputSpeakerArrangements_;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::attachToWindow(QWindow* window)
{
    if(view_)
    {
        window_ = window;
        Steinberg::ViewRect viewRect;
        view_->getSize(&viewRect);
        window_->setPosition(viewRect.left, viewRect.top);
        window_->setWidth(viewRect.getWidth());
        window_->setHeight(viewRect.getHeight());
        window_->setTitle(classInfo_.name);
        view_->attached(reinterpret_cast<HWND>(window_->winId()), Steinberg::kPlatformTypeHWND);
        Musec::Controller::AudioEngineController::AppProject().addPluginWindowMapping(audioProcessor_, window_);
        QObject::connect(window_, &QWindow::widthChanged,
            [this](int) { onWindowSizeChanged(); });
        QObject::connect(window_, &QWindow::heightChanged,
            [this](int) { onWindowSizeChanged(); });
        return true;
    }
    return false;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::detachWithWindow()
{
    if(!window_)
    {
        return true;
    }
    if(view_)
    {
        view_->removed();
        Musec::Controller::AudioEngineController::AppProject().removePluginWindowMapping(audioProcessor_);
        window_ = nullptr;
        return true;
    }
    return false;
}

template<typename SampleType>
const Steinberg::PClassInfo& VST3Plugin<SampleType>::getClassInfo() const
{
    return classInfo_;
}

template<typename SampleType>
Steinberg::IPluginFactory* VST3Plugin<SampleType>::factory() const
{
    return factory_;
}

template<typename SampleType>
Steinberg::IPlugView* VST3Plugin<SampleType>::getView() const
{
    return view_;
}

template<typename SampleType>
void VST3Plugin<SampleType>::onWindowSizeChanged()
{
    if(view_)
    {
        auto x = window_->x();
        auto y = window_->y();
        auto width = window_->width();
        auto height = window_->height();
        auto newRect = Steinberg::ViewRect(x, y, x + width, y + height);
        auto oldRect = Steinberg::ViewRect(); view_->getSize(&oldRect);
        if(view_->checkSizeConstraint(&newRect) == Steinberg::kResultOk)
        {
            window_->setX(newRect.left);
            window_->setY(newRect.top);
            window_->setWidth(newRect.getWidth());
            window_->setHeight(newRect.getHeight());
            view_->onSize(&newRect);
        }
    }
}

template<typename SampleType>
bool VST3Plugin<SampleType>::getBypass() const
{
    return this->audioProcessorStatus_ <= VST3AudioProcessorStatus::Processing;
}

template<typename SampleType> QString VST3Plugin<SampleType>::getName() const
{
    if(!factory_)
    {
        return QString();
    }
    return QString(getClassInfo().name);
}

template<typename SampleType> QWindow* VST3Plugin<SampleType>::window()
{
    return window_;
}

template<typename SampleType> bool VST3Plugin<SampleType>::hasUI()
{
    return view_;
}

template<typename SampleType>
Musec::Base::PluginFormat VST3Plugin<SampleType>::pluginFormat()
{
    return Musec::Base::PluginFormat::FormatVST3;
}

template<typename SampleType>
bool VST3Plugin<SampleType>::activated()
{
    return audioProcessorStatus_ >= VST3AudioProcessorStatus::Activated;
}

template class VST3Plugin<float>;
template class VST3Plugin<double>;
}
