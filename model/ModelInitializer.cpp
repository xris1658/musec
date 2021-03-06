#include "ModelInitializer.hpp"

#include "base/QmlBase.hpp"
#include "model/ASIODriverListModel.hpp"
#include "model/AssetDirectoryListModel.hpp"
#include "model/AudioTrackSequenceModel.hpp"
#include "model/AutomationModel.hpp"
#include "model/FileListModel.hpp"
#include "model/FolderListModel.hpp"
#include "model/PluginListModel.hpp"
#include "model/TrackListModel.hpp"

namespace Musec::Model
{
ModelInitializer::ModelInitializer()
{
    QML_REGISTER_TYPE(ASIODriverListModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(AssetDirectoryListModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(AudioTrackSequenceModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(AutomationModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(FileListModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(FolderListModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(PluginListModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(PluginSequenceModel, Musec.Models, 1, 0);
    QML_REGISTER_TYPE(TrackListModel, Musec.Models, 1, 0);
}

void ModelInitializer::initialize()
{
    static ModelInitializer ret;
}
}
