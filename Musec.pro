QT += quick
QT += widgets
# 1. 直接使用了 COM API，但没有使用 Active Qt
# 2. Qt 6 中的 Active Qt 好像出了点小意外
#QT += axcontainer

CONFIG += c++17
CONFIG += resources_big
CONFIG += qtquickcompiler

CONFIG(debug, debug | release) {
    CONFIG += declarative_debug
    CONFIG += qml_debug
}

QMAKE_CXXFLAGS += /Zc:wchar_t

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += D:/apps/vcpkg/installed/x64-windows/include \
               D:/apps/vst3sdk \
               D:/apps/vst3sdk/pluginterfaces \
               D:/apps/vst3sdk/public.sdk/source \
               C:/asiosdk_2.3.3_2019-06-14 \
               D:/apps/avcpp/src \
               D:/apps/ffmpeg-master-windows-desktop-vs2019-default/ffmpeg-master-windows-desktop-vs2019-default/include

CONFIG(debug, debug | release) {
    QMAKE_LIBDIR += \
        D:/apps/vcpkg/installed/x64-windows/debug/lib \
        D:/apps/vst3sdk/build/debug/lib
}
CONFIG(release, debug | release) {
     QMAKE_LIBDIR += \
        D:/apps/vst3sdk/build/release/lib \
        D:/apps/vcpkg/installed/x64-windows/lib
}

DEFINES += _CRT_SECURE_NO_WARNINGS

HEADERS += \
    audio/arrangement/ClipInTrack.hpp \
    audio/arrangement/TrackSequence.hpp \
    audio/base/AudioBufferView.hpp \
    audio/base/Automation.hpp \
    audio/base/Chrono.hpp \
    audio/base/TempoAutomation.hpp \
    audio/base/TimeSignature.hpp \
    audio/base/TimeSignatureSequence.hpp \
    audio/device/IDevice.hpp \
    audio/device/IDevice.hpp \
    audio/driver/ASIOCallback.hpp \
    audio/driver/ASIODriver.hpp \
    audio/driver/ASIODriverForward.hpp \
    audio/driver/ASIODriverStreamInfo.hpp \
    audio/engine/Graph.hpp \
    audio/engine/MIDIClock.hpp \
    audio/engine/Project.hpp \
    audio/host/MusecVST3Host.hpp \
    audio/host/VST3ComponentHandler.hpp \
    audio/media/AudioSequence.hpp \
    audio/media/MIDISequence.hpp \
    audio/plugin/IPlugin.hpp \
    audio/plugin/VST2Plugin.hpp \
    audio/plugin/VST3Plugin.hpp \
    audio/track/AudioTrack.hpp \
    audio/track/InstrumentTrack.hpp \
    audio/track/ITrack.hpp \
    audio/track/MIDITrack.hpp \
    audio/track/PluginSequence.hpp \
    audio/track/TrackInformation.hpp \
    audio/util/Util.hpp \
    base/AssetDirectoryBase.hpp \
    base/Base.hpp \
    base/Color.hpp \
    base/Constants.hpp \
    base/FileBase.hpp \
    base/FixedSizeMemoryPool.hpp \
    base/FolderBase.hpp \
    base/PluginBase.hpp \
    base/QmlBase.hpp \
    controller/AppController.hpp \
    controller/ASIODriverController.hpp \
    controller/AssetController.hpp \
    controller/AssetDirectoryController.hpp \
    controller/AudioEngineController.hpp \
    controller/ConfigController.hpp \
    controller/GeneralSettingsController.hpp \
    controller/MIDIClockController.hpp \
    controller/PluginController.hpp \
    controller/PluginSettingsController.hpp \
    dao/AssetDirectoryDAO.hpp \
    dao/ConfigDAO.hpp \
    dao/DatabaseDAO.hpp \
    dao/LoggingDAO.hpp \
    dao/PluginDAO.hpp \
    dao/PluginDirectoryDAO.hpp \
    dkwtp/ThreadPool.hpp \
    entities/CompleteTrack.hpp \
    entities/EntitiesInitializer.hpp \
    event/EventBase.hpp \
    event/EventHandler.hpp \
    event/MainWindow.hpp \
    event/ObjectCreateListener.hpp \
    event/SplashScreen.hpp \
    event/SplashScreenForward.hpp \
    event/SplashScreenWorkerThread.hpp \
    math/Integration.hpp \
    math/QuadraticFunction.hpp \
    model/ASIODriverListModel.hpp \
    model/AssetDirectoryListModel.hpp \
    model/AudioTrackSequenceModel.hpp \
    model/AutomationModel.hpp \
    model/FileListModel.hpp \
    model/FolderListModel.hpp \
    model/ModelBase.hpp \
    model/ModelInitializer.hpp \
    model/PluginListModel.hpp \
    model/TrackListModel.hpp \
    native/Native.hpp \
    native/WindowsLibraryRAII.hpp \
    ui/FontUtility.hpp \
    ui/MessageDialog.hpp \
    ui/Render.hpp \
    ui/UI.hpp \
    util/Endian.hpp \
    util/Literal.hpp

SOURCES += \
    audio/base/TimeSignature.cpp \
    audio/driver/ASIOCallback.cpp \
    audio/driver/ASIODriver.cpp \
    audio/engine/MIDIClock.cpp \
    audio/engine/Project.cpp \
    audio/host/MusecVST3Host.cpp \
    audio/host/VST3ComponentHandler.cpp \
    audio/media/AudioSequence.cpp \
    audio/plugin/ShellPluginId.cpp \
    audio/plugin/VST2Plugin.cpp \
    audio/plugin/VST3Plugin.cpp \
    audio/track/AudioTrack.cpp \
    audio/track/InstrumentTrack.cpp \
    audio/track/MIDITrack.cpp \
    audio/util/Util.cpp \
    base/Color.cpp \
    base/FixedSizeMemoryPool.cpp \
    base/PluginBase.cpp \
    controller/AppController.cpp \
    controller/ASIODriverController.cpp \
    controller/AssetController.cpp \
    controller/AssetDirectoryController.cpp \
    controller/AudioEngineController.cpp \
    controller/ConfigController.cpp \
    controller/GeneralSettingsController.cpp \
    controller/MIDIClockController.cpp \
    controller/PluginController.cpp \
    controller/PluginSettingsController.cpp \
    dao/AssetDirectoryDAO.cpp \
    dao/ConfigDAO.cpp \
    dao/DatabaseDAO.cpp \
    dao/LoggingDAO.cpp \
    dao/PluginDAO.cpp \
    dao/PluginDirectoryDAO.cpp \
    dkwtp/ThreadPool.cpp \
    entities/CompleteTrack.cpp \
    entities/EntitiesInitializer.cpp \
    event/EventBase.cpp \
    event/EventHandler.cpp \
    event/MainWindow.cpp \
    event/ObjectCreateListener.cpp \
    event/SplashScreen.cpp \
    event/SplashScreenWorkerThread.cpp \
    main.cpp \
    math/Integration.cpp \
    math/QuadraticFunction.cpp \
    model/ASIODriverListModel.cpp \
    model/AssetDirectoryListModel.cpp \
    model/AudioTrackSequenceModel.cpp \
    model/AutomationModel.cpp \
    model/FileListModel.cpp \
    model/FolderListModel.cpp \
    model/ModelInitializer.cpp \
    model/PluginListModel.cpp \
    model/TrackListModel.cpp \
    native/Native.cpp \
    native/WindowsLibraryRAII.cpp \
    ui/FontUtility.cpp \
    ui/MessageDialog.cpp \
    ui/Render.cpp \
    ui/UI.cpp \
    util/Endian.cpp \
    util/Literal.cpp \
    # avcpp 源码
    D:/apps/avcpp/src/audioresampler.cpp \
    D:/apps/avcpp/src/averror.cpp \
    D:/apps/avcpp/src/avtime.cpp \
    D:/apps/avcpp/src/avutils.cpp \
    D:/apps/avcpp/src/codec.cpp \
    D:/apps/avcpp/src/codeccontext.cpp \
    D:/apps/avcpp/src/dictionary.cpp \
    D:/apps/avcpp/src/format.cpp \
    D:/apps/avcpp/src/formatcontext.cpp \
    D:/apps/avcpp/src/frame.cpp \
    D:/apps/avcpp/src/packet.cpp \
    D:/apps/avcpp/src/pixelformat.cpp \
    D:/apps/avcpp/src/rational.cpp \
    D:/apps/avcpp/src/rect.cpp \
    D:/apps/avcpp/src/sampleformat.cpp \
    D:/apps/avcpp/src/stream.cpp \
    D:/apps/avcpp/src/timestamp.cpp \
    D:/apps/avcpp/src/videorescaler.cpp \
    D:/apps/avcpp/src/filters/buffersink.cpp \
    D:/apps/avcpp/src/filters/buffersrc.cpp \
    D:/apps/avcpp/src/filters/filter.cpp \
    D:/apps/avcpp/src/filters/filtercontext.cpp \
    D:/apps/avcpp/src/filters/filtergraph.cpp \
    D:/apps/avcpp/src/filters/filterpad.cpp

RESOURCES += \
    $$files(qmlproject/*) \
    $$files(qmlproject/imports/Musec) \
    Musec.qrc

LIBS += \
    sqlite3.lib \ # SQLite 3
    sndfile.lib \ # libsndfile
    AdvAPI32.lib \ # 注册表相关 API
    Shell32.lib \ # SHGetFolderPathW
    ole32.lib \
    # VST3 SDK
    sdk.lib \
    base.lib \
    pluginterfaces.lib \
    sdk_common.lib \
    sdk_hosting.lib \
    avcodec.lib \
    avdevice.lib \
    avfilter.lib \
    avformat.lib \
    avutil.lib \
    swresample.lib \
    swscale.lib

CONFIG(debug, debug | release) {
    LIBS += yaml-cppd.lib \
    fmtd.lib \
    spdlogd.lib
}
CONFIG(release, debug | release) {
    LIBS += yaml-cpp.lib \
    fmt.lib \
    spdlog.lib
}

RC_ICONS = qmlproject/images/Musec-image-2.ico

VERSION = 0.0.0.1
QMAKE_TARGET_COMPANY = "xris1658"
QMAKE_TARGET_DESCRIPTION = "Musec based on Qt 5.15.2 (MSVC 2019 64-bit)"
QMAKE_TARGET_COPYRIGHT = "Copyright xris1658 2021-2022. All rights reserved."
QMAKE_TARGET_PRODUCT = "Musec"
RC_LANG = 0x0804 # 简体中文（中国）

TRANSLATIONS += \
    Musec_zh_CN.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = qmlproject/imports

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH = qmlproject/imports

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    $$files(qmlproject/*) \
    $$files(qmlproject/imports/Musec)

#msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
#msvc:QMAKE_CXXFLAGS += -source-charset:utf-8

# 修改 Release 时使用的异常处理模式
# 参考链接：
# http://www.databaseforum.info/2/9/f69800e14add61dd.html
# https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-160
QMAKE_CXXFLAGS_RELEASE -= -EHsc
QMAKE_CXXFLAGS_RELEASE += -EHs
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -EHsc
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -EHs
