#include "ASIOCallback.hpp"

#include "audio/base/Constants.hpp"
#include "audio/driver/ASIODriver.hpp"
#include "base/Base.hpp"
#include "controller/ASIODriverController.hpp"
#include "controller/AudioEngineController.hpp"
#include "native/Native.hpp"
#include "util/Endian.hpp"
#include "util/TimerClamp.hpp"

namespace Musec::Audio::Driver
{
bool driverSupportsOutputReady;

void(*functionThatUsesDuration)(std::int64_t) = nullptr;

using BufferSwitchCallbackType = decltype(ASIOCallbacks::bufferSwitch);

namespace Impl
{
void onASIOBufferSwitchWithoutTimerClamp(long doubleBufferIndex, ASIOBool directProcess)
{
    if(*AppASIODriver())
    {
        std::array<Base::AudioBufferView<float>, 2> masterTrackAudioBufferView = {};
        auto bufferSize = getBufferSize().preferredBufferSize;
        masterTrackAudioBufferView[0] = Base::AudioBufferView<float>(
                reinterpret_cast<float*>(Controller::AudioEngineController::AppProject().masterTrackAudioBuffer().data()),
                bufferSize
        );
        masterTrackAudioBufferView[1] = Base::AudioBufferView<float>(
                reinterpret_cast<float*>(Controller::AudioEngineController::AppProject().masterTrackAudioBuffer().data()) + bufferSize,
                bufferSize
        );
        std::array<int, maxInputChannelCount> inputs = {0};
        std::array<int, maxOutputChannelCount> outputs = {0};
        int inputCount = 0;
        int outputCount = 0;
        auto& bufferInfoList = getASIOBufferInfoList();
        auto& channelInfoList = getASIOChannelInfoList();
        for(int i = 0; i < channelInfoList.size(); ++i)
        {
            if(channelInfoList[i].isActive)
            {
                if(channelInfoList[i].isInput)
                {
                    inputs[inputCount++] = i;
                }
                else
                {
                    outputs[outputCount++] = i;
                }
            }
        }
        for(int i = 0; i < inputCount; ++i)
        {
            auto buffer = bufferInfoList[inputs[i]].buffers;
            // MSB -> BE; LSB -> LE;
            auto type = channelInfoList[i].type;
            switch(type)
            {
            case ASIOSTInt16MSB:
                break;
            case ASIOSTInt24MSB:
                break;
            case ASIOSTInt32MSB:
                break;
            case ASIOSTFloat32MSB:
                break;
            case ASIOSTFloat64MSB:
                break;
            case ASIOSTInt32MSB16:
                break;
            case ASIOSTInt32MSB18:
                break;
            case ASIOSTInt32MSB20:
                break;
            case ASIOSTInt32MSB24:
                break;
            case ASIOSTInt16LSB:
                break;
            case ASIOSTInt24LSB:
                break;
            case ASIOSTInt32LSB:
                break;
            case ASIOSTFloat32LSB:
                break;
            case ASIOSTFloat64LSB:
                break;
            case ASIOSTInt32LSB16:
                break;
            case ASIOSTInt32LSB18:
                break;
            case ASIOSTInt32LSB20:
                break;
            case ASIOSTInt32LSB24:
                break;
            case ASIOSTDSDInt8LSB1:
                break;
            case ASIOSTDSDInt8MSB1:
                break;
            case ASIOSTDSDInt8NER8:
                break;
            case ASIOSTLastEntry:
                break;
            default:
                break;
            }
        }
        Controller::AudioEngineController::AppProject().process();
        for(int i = 0; i < 2; ++i)
        {
            using int24_t = int8_t[3];
            auto buffer = bufferInfoList[outputs[i]].buffers[doubleBufferIndex];
            // buffer[0] ??? buffer[1] ??? ASIO ?????????????????????
            // ?????????????????? (e.g. ASIO4ALL??????????????????????????????????????????
            // ?????????????????????FlexASIO???????????????????????????????????????
            switch(channelInfoList[outputs[i]].type)
            {
            // MSB
            case ASIOSTInt16MSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int16_t* int16Buffer = reinterpret_cast<int16_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int16Buffer[j] = static_cast<int16_t>(masterTrackAudioBufferView[i][j] * Base::Int16Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int16Buffer + j, sizeof(int16_t));
                    }
                }
                break;
            }
            case ASIOSTInt24MSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int24_t* int24Buffer = reinterpret_cast<int24_t*>(buffer);
                int32_t temp;
                for(int j = 0; j < bufferSize; ++j)
                {
                    temp = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int24Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        memcpy(int24Buffer + j, &temp, sizeof(int24_t));
                    }
                    else
                    {
                        memcpy(int24Buffer + j, reinterpret_cast<int8_t*>(&temp) + 1, sizeof(int24_t));
                    }
                }
                break;
            }
            case ASIOSTInt32MSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int32Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTFloat32MSB:
            {
                if constexpr(Native::endian() == Util::Endian::BigEndian)
                {
                    memcpy(buffer, masterTrackAudioBufferView[i].getSamples(), bufferSize * sizeof(float));
                }
                else
                {
                    Util::reverseEndiannessCopy(masterTrackAudioBufferView[i].getSamples(), bufferSize * sizeof(float), buffer);
                }
                break;
            }
            case ASIOSTFloat64MSB:
            {
                double* doubleBuffer = reinterpret_cast<double*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    doubleBuffer[j] = static_cast<double>(masterTrackAudioBufferView[i][j]);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(doubleBuffer + j, sizeof(double));
                    }
                }
                break;
            }
            case ASIOSTInt32MSB16:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int16Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32MSB18:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int18Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32MSB20:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int20Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32MSB24:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int24Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            // LSB
            case ASIOSTInt16LSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int16_t* int16Buffer = reinterpret_cast<int16_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int16Buffer[j] = static_cast<int16_t>(masterTrackAudioBufferView[i][j] * Base::Int16Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int16Buffer + j, sizeof(int16_t));
                    }
                }
                break;
            }
            case ASIOSTInt24LSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int24_t* int24Buffer = reinterpret_cast<int24_t*>(buffer);
                int32_t temp;
                for(int j = 0; j < bufferSize; ++j)
                {
                    temp = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int24Max);
                    if constexpr(Native::endian() == Util::Endian::LittleEndian)
                    {
                        memcpy(int24Buffer + j, &temp, sizeof(int24_t));
                    }
                    else
                    {
                        memcpy(int24Buffer + j, reinterpret_cast<int8_t*>(&temp) + 1, sizeof(int24_t));
                    }
                }
                break;
            }
            case ASIOSTInt32LSB:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int32Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTFloat32LSB:
            {
                if constexpr(Native::endian() == Util::Endian::LittleEndian)
                {
                    memcpy(buffer, masterTrackAudioBufferView[i].getSamples(), bufferSize * sizeof(float));
                }
                else
                {
                    Util::reverseEndiannessCopy(masterTrackAudioBufferView[i].getSamples(), bufferSize * sizeof(float), buffer);
                }
                break;
            }
            case ASIOSTFloat64LSB:
            {
                double* doubleBuffer = reinterpret_cast<double*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    doubleBuffer[j] = static_cast<double>(masterTrackAudioBufferView[i][j]);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(doubleBuffer + j, sizeof(double));
                    }
                }
                break;
            }
            case ASIOSTInt32LSB16:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int16Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32LSB18:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int18Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32LSB20:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int20Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            case ASIOSTInt32LSB24:
            {
                Base::clip(masterTrackAudioBufferView[i]);
                int32_t* int32Buffer = reinterpret_cast<int32_t*>(buffer);
                for(int j = 0; j < bufferSize; ++j)
                {
                    int32Buffer[j] = static_cast<int32_t>(masterTrackAudioBufferView[i][j] * Base::Int24Max);
                    if constexpr(Native::endian() == Util::Endian::BigEndian)
                    {
                        Util::reverseEndianness(int32Buffer + j, sizeof(int32_t));
                    }
                }
                break;
            }
            // DSD
            case ASIOSTDSDInt8LSB1:
                break;
            case ASIOSTDSDInt8MSB1:
                break;
            case ASIOSTDSDInt8NER8:
                break;
            case ASIOSTLastEntry:
                break;
            default:
                break;
            }
        }
        if(AppASIODriver() && driverSupportsOutputReady)
        {
            AppASIODriver()->outputReady();
        }
    }
}
}

ASIOTime* onASIOBufferSwitchTimeInfo(ASIOTime* params,
                                     long doubleBufferIndex,
                                     ASIOBool directProcess)
{
    onASIOBufferSwitch(doubleBufferIndex, directProcess);
    return nullptr;
}

void onASIOSampleRateDidChange(ASIOSampleRate sRate)
{
    // TODO
}

long onASIOMessage(long selector,
                   long value,
                   void* message,
                   double* opt)
{
    long ret = 0;
    switch(selector)
    {
        case kAsioEngineVersion:
            ret = 2L;
            break;
    case kAsioSupportsTimeInfo:
        break;
    default:
        ret = 1L;
        break;
    }
    return ret;
}

void onASIOBufferSwitch(long doubleBufferIndex, ASIOBool directProcess)
{
    auto timeInNanoSecond = Musec::Util::timerClampVoid<long, ASIOBool>(Impl::onASIOBufferSwitchWithoutTimerClamp,
        std::forward<long>(doubleBufferIndex), std::forward<ASIOBool>(directProcess));
    if(functionThatUsesDuration)
    {
        functionThatUsesDuration(timeInNanoSecond);
    }
}

ASIOCallbacks& getCallbacks()
{
    static ASIOCallbacks ret
    {
        &onASIOBufferSwitch,
        &onASIOSampleRateDidChange,
        &onASIOMessage,
        &onASIOBufferSwitchTimeInfo
    };
    return ret;
}
}
