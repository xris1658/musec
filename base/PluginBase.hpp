#ifndef MUSEC_BASE_PLUGINBASE
#define MUSEC_BASE_PLUGINBASE

#include <QList>
#include <QString>

#include <pluginterfaces/base/ipluginbase.h>

namespace Musec
{
namespace Base
{
// 插件基本信息：UID，名称，格式，类型
using PluginBasicInfo = std::tuple<int, QString, int, int>;

// 存入数据库的插件信息：路径，UID，名称，格式，类型
using PluginWriteInfo = std::tuple<QString, int, QString, int, int>;

// 从数据库读取的插件信息：ID，路径，UID，名称，格式，类型
using PluginReadInfo = std::tuple<int, QString, int, QString, int, int>;

// VST3 插件的初始化函数（InitDll）
using VST3PluginInitProc = bool(*)();

// VST3 插件的工厂函数（GetPluginFactory）
using VST3PluginFactoryProc = Steinberg::IPluginFactory*(*)();

// VST3 插件的出口函数（ExitDll）
using VST3PluginExitProc = bool(*)();

// 插件基本信息的字段
enum PluginBasicInfoField
{
    BasicFieldUid = 0,
    BasicFieldName,
    BasicFieldFormat,
    BasicFieldType
};

enum PluginWriteInfoField
{
    WriteFieldPath = 0,
    WriteFieldUid,
    WriteFieldName,
    WriteFieldFormat,
    WriteFieldType
};

enum PluginReadInfoField
{
    ReadFieldId = 0,
    ReadFieldPath,
    ReadFieldUid,
    ReadFieldName,
    ReadFieldFormat,
    ReadFieldType
};

// 插件的格式（非插件，VST2，VST3，CLAP）
enum PluginFormat
{
    FormatNotAPlugin = 0,
    FormatVST2,
    FormatVST3,
    FormatCLAP,
    PluginFormatCount
};

// 插件的类型（未知，MIDI 效果器，乐器，音频效果器）
enum PluginType
{
    TypeUnknown = 0,
    TypeMidiFX,
    TypeInstrument,
    TypeAudioFX
};

// 获取插件的格式
PluginFormat pluginFormat(const QString& path);

// 扫描一个库文件
QList<PluginBasicInfo> scanSingleLibraryFile(const QString& path);

// 默认的插件目录列表
QStringList& defaultPluginDirectoryList();
}
}

#endif // MUSEC_BASE_PLUGINBASE
