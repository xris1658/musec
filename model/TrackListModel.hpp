
#ifndef MUSEC_MODEL_TRACKLISTMODEL
#define MUSEC_MODEL_TRACKLISTMODEL

#include "audio/engine/Project.hpp"
#include "model/ModelBase.hpp"
#include "model/PluginSequenceModel.hpp"

#include <QAbstractListModel>

namespace Musec
{
namespace Model
{
class TrackListModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(double masterTrackGain         READ getMasterTrackGain         WRITE setMasterTrackGain         NOTIFY masterTrackGainChanged        )
    Q_PROPERTY(double masterTrackPanning      READ getMasterTrackPanning      WRITE setMasterTrackPanning      NOTIFY masterTrackPanningChanged     )
    Q_PROPERTY(bool   masterTrackMute         READ getMasterTrackMute         WRITE setMasterTrackMute         NOTIFY masterTrackMuteChanged        )
    Q_PROPERTY(bool   masterTrackSolo         READ getMasterTrackSolo         WRITE setMasterTrackSolo         NOTIFY masterTrackSoloChanged        )
    Q_PROPERTY(bool   masterTrackInvertPhase  READ getMasterTrackInvertPhase  WRITE setMasterTrackInvertPhase  NOTIFY masterTrackInvertPhaseChanged )
    Q_PROPERTY(bool   masterTrackArmRecording READ getMasterTrackArmRecording WRITE setMasterTrackArmRecording NOTIFY masterTrackArmRecordingChanged)
public:
    enum RoleNames
    {
        NameRole = Qt::UserRole,
        TypeRole,
        ColorRole,
        HeightRole,
        MuteRole,
        SoloRole,
        InvertPhaseRole,
        ArmRecordingRole,
        InstrumentRole,
        PluginListRole,
        kRoleNamesCount
    };
public:
    explicit TrackListModel(QObject* parent = nullptr);
    virtual ~TrackListModel();
public:
    static constexpr int columnSize();
public:
    Q_INVOKABLE int trackCount() const noexcept;
public:
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&, int) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Musec::Model::PluginSequenceModel& masterTrackPluginSequenceModel();
public:
    Q_INVOKABLE void insertTrack(int index, const Musec::Entities::CompleteTrack& track);
    Q_INVOKABLE void appendTrack(const Musec::Entities::CompleteTrack& track);
    Q_INVOKABLE void removeTrack(int index);
    Q_INVOKABLE void loadInstrument(int trackIndex, int pluginFormat, const QString& path, int pluginSubId);
    Q_INVOKABLE void removeInstrument(int trackIndex);
    Q_INVOKABLE void insertEffect(int trackIndex, int pluginFormat, const QString& path, int pluginSubId, int pluginIndex);
    Q_INVOKABLE void replaceEffect(int trackIndex, int pluginFormat, const QString& path, int pluginSubId, int pluginIndex);
    Q_INVOKABLE void removeEffect(int trackIndex, int pluginIndex);
    Q_INVOKABLE void copyInstrument(int fromTrackIndex, int toTrackIndex);
    Q_INVOKABLE void copyEffect(int fromTrackIndex, int fromPluginIndex, int toTrackIndex, int toPluginIndex);
    Q_INVOKABLE void moveInstrument(int fromTrackIndex, int toTrackIndex);
    Q_INVOKABLE void moveEffect(int fromTrackIndex, int fromPluginIndex, int toTrackIndex, int toPluginIndex);
    Q_INVOKABLE void insertEffectMasterTrack(int pluginFormat, const QString& path, int pluginSubId, int pluginIndex);
    Q_INVOKABLE void replaceEffectMasterTrack(int pluginFormat, const QString& path, int pluginSubId, int pluginIndex);
    Q_INVOKABLE void removeEffectMasterTrack(int pluginIndex);
public:
    double getMasterTrackGain() const;
    void setMasterTrackGain(double gain);
    double getMasterTrackPanning() const;
    void setMasterTrackPanning(double panning);
    bool getMasterTrackMute() const;
    void setMasterTrackMute(bool mute);
    bool getMasterTrackSolo() const;
    void setMasterTrackSolo(bool solo);
    bool getMasterTrackInvertPhase() const;
    void setMasterTrackInvertPhase(bool invertPhase);
    bool getMasterTrackArmRecording() const;
    void setMasterTrackArmRecording(bool armRecording);
signals:
    void masterTrackGainChanged();
    void masterTrackPanningChanged();
    void masterTrackMuteChanged();
    void masterTrackSoloChanged();
    void masterTrackInvertPhaseChanged();
    void masterTrackArmRecordingChanged();
protected:
    virtual RoleNamesType roleNames() const override;
private:
    Musec::Audio::Engine::Project& project_;
    Musec::Audio::Engine::Project::MasterTrackRef projectMasterTrackRef_;
    std::vector<std::unique_ptr<Musec::Entities::Plugin>> instruments_;
    std::vector<std::unique_ptr<Musec::Model::PluginSequenceModel>> pluginSequences_;
    Musec::Model::PluginSequenceModel masterPluginSequences_;
    RoleNamesType roleNames_;
};
}
}

#endif //MUSEC_MODEL_TRACKLISTMODEL
