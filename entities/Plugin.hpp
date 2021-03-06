#ifndef MUSEC_ENTITIES_PLUGIN
#define MUSEC_ENTITIES_PLUGIN

#include "audio/plugin/IPlugin.hpp"
#include "base/PluginBase.hpp"

#include <QString>
#include <qqml.h>

#include <memory>

namespace Musec
{
namespace Entities
{
class Plugin: public QObject
{
    Q_OBJECT
//  | ******* | type  | name for UI    | member in class        | getter                | setter                  | signal                       |
//  | ------- | ----- | -------------- | ---------------------- | --------------------- | ----------------------- | ---------------------------- |
    Q_PROPERTY(QString name             MEMBER name_             READ getName            WRITE setName             NOTIFY nameChanged            )
    Q_PROPERTY(bool    enabled          MEMBER enabled_          READ isEnabled          WRITE setEnabled          NOTIFY enabledChanged         )
    Q_PROPERTY(bool    sidechainExist   MEMBER sidechainExist_   READ isSidechainExist   WRITE setSidechainExist   NOTIFY sidechainExistChanged  )
    Q_PROPERTY(bool    sidechainEnabled MEMBER sidechainEnabled_ READ isSidechainEnabled WRITE setSidechainEnabled NOTIFY sidechainEnabledChanged)
    Q_PROPERTY(bool    windowVisible                             READ isWindowVisible    WRITE setWindowVisible    NOTIFY windowVisibleChanged   )
    QML_ELEMENT
public:
    enum PluginType
    {
        Unknown = Musec::Base::PluginType::TypeUnknown,
        MIDIEffect = Musec::Base::PluginType::TypeMidiFX,
        Instrument = Musec::Base::PluginType::TypeInstrument,
        AudioEffect = Musec::Base::PluginType::TypeAudioFX
    };
    Q_ENUM(PluginType)
public:
    Plugin(QObject* parent = nullptr);
    Plugin(std::shared_ptr<Musec::Audio::Plugin::IPlugin<float>> plugin,
           const QString& name, bool enabled,
           bool sidechainExist, bool sidechainEnabled);
    Plugin(Plugin&& rhs) noexcept;
    Plugin& operator=(Plugin&& rhs) noexcept;
    ~Plugin();
public:
    static Musec::Entities::Plugin fromPlugin(std::shared_ptr<Musec::Audio::Plugin::IPlugin<float>> plugin = nullptr);
    bool valid() const;
    const QString& getName() const;
    void setName(const QString& name);
    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool isSidechainExist() const;
    void setSidechainExist(bool sidechainExist);
    bool isSidechainEnabled() const;
    void setSidechainEnabled(bool sidechainEnabled);
    bool isWindowVisible() const;
    void setWindowVisible(bool windowVisible);

signals:
    void nameChanged();
    void enabledChanged();
    void sidechainExistChanged();
    void sidechainEnabledChanged();
    void windowVisibleChanged();
public:
    void swap(Plugin& rhs) noexcept;
private:
    void initSignal();
private:
    std::shared_ptr<Musec::Audio::Plugin::IPlugin<float>> plugin_;
    QString name_;
    bool enabled_;
    bool sidechainExist_;
    bool sidechainEnabled_;
};
}
}

namespace std
{
template<>
void swap(Musec::Entities::Plugin& lhs, Musec::Entities::Plugin& rhs) noexcept;
}

#endif //MUSEC_ENTITIES_PLUGIN
