#ifndef MUSEC_CONTROLLER_PLUGINSETTINGSCONTROLLER
#define MUSEC_CONTROLLER_PLUGINSETTINGSCONTROLLER

#include <QString>

namespace Musec
{
namespace Controller
{
namespace PluginSettingsController
{
// 获取插件目录
QStringList getPluginDirectoryList();

// 添加插件目录
void addPluginDirectory(const QString&);

// 移除插件目录
void removePluginDirectory(const QString&);

// 扫描所有目录的插件
void scanPlugins();

// 更改扫描插件快捷方式的设置项
void setScanShortcut(bool value);
}
}
}

#endif // MUSEC_CONTROLLER_PLUGINSETTINGSCONTROLLER
