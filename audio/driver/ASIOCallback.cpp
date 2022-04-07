#include "ASIOCallback.hpp"

#include "audio/driver/ASIODriver.hpp"
#include "base/Base.hpp"
#include "controller/ASIODriverController.hpp"
#include "controller/AudioEngineController.hpp"

namespace Musec::Audio::Driver
{
std::int8_t bufferIndex = 0;
void onASIOBufferSwitch(long doubleBufferIndex, ASIOBool directProcess)
{
    ASIOTime timeInfo;
    Musec::Base::memoryReset(&timeInfo);
    auto& driver = AppASIODriver();
    // 以下代码可能在 ASIO 驱动被卸载后执行，因此需要额外的检验。
    // 加锁表面上更好，但此函数可能在中断时刻调用，因此上锁可能会影响效率。
    if(driver)
    {
        if(driver->getSamplePosition(&(timeInfo.timeInfo.samplePosition),
                                     &(timeInfo.timeInfo.systemTime))
           == ASE_OK)
        {
            timeInfo.timeInfo.flags = AsioTimeInfoFlags::kSystemTimeValid
                                    | AsioTimeInfoFlags::kSamplePositionValid;
        }
        onASIOBufferSwitchTimeInfo(&timeInfo, doubleBufferIndex, directProcess);
    }
}

ASIOTime* onASIOBufferSwitchTimeInfo(ASIOTime* params,
                                     long doubleBufferIndex,
                                     ASIOBool directProcess)
{
    auto currentBufferIndex = bufferIndex;
    if(bufferIndex == 1)
    {
        bufferIndex = 0;
    }
    else
    {
        bufferIndex = 1;
    }
    std::array<int, 64> inputs = {0};
    std::array<int, 64> outputs = {0};
    int inputCount = 0;
    int outputCount = 0;
    auto bufferSize = getASIODriverStreamInfo(AppASIODriver()).preferredBufferSize;
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
    for(int i = 0; i < outputCount; ++i)
    {
        auto buffer = bufferInfoList[outputs[i]].buffers;
        // buffer[0] 和 buffer[1] 是 ASIO 的双缓冲区地址
        // 有些驱动程序 (e.g. ASIO4ALL）不使用双缓冲，两个地址相同
        // 多数驱动程序（FlexASIO）使用双缓冲，两个地址不同
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
                for(long j = 0; j < bufferSize; ++j)
                {
//                  reinterpret_cast<std::int32_t*>(buffer[currentBufferIndex])[j] = 0x10000000 * (std::rand() % 2? 1: -1);
                }
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
    // TODO
    long ret = 0;
    switch(selector)
    {
        case kAsioSelectorSupported:
            if(value == kAsioResetRequest
            || value == kAsioEngineVersion
            || value == kAsioResyncRequest
            || value == kAsioLatenciesChanged
            || value == kAsioSupportsTimeInfo
            || value == kAsioSupportsTimeCode
            || value == kAsioSupportsInputMonitor)
                ret = 1L;
            break;
        case kAsioResetRequest:
            ret = 1L;
            break;
        case kAsioResyncRequest:
            ret = 1L;
            break;
        case kAsioLatenciesChanged:
            ret = 1L;
            break;
        case kAsioEngineVersion:
            ret = 2L;
            break;
        case kAsioSupportsTimeInfo:
            ret = 1;
            break;
        case kAsioSupportsTimeCode:
            ret = 0;
            break;
        default:
            ret = 0;
            break;
    }
    return ret;
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
