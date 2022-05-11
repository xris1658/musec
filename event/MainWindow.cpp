#include "MainWindow.hpp"

#include "audio/driver/ASIODriver.hpp"
#include "controller/ASIODriverController.hpp"
#include "controller/AppController.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/MIDIClockController.hpp"
#include "event/EventBase.hpp"
#include "ui/UI.hpp"

#include <QString>
#include <QQuickWindow>

namespace Musec::Event
{
MainWindow::MainWindow(const Musec::Event::SplashScreen& splash): QObject(nullptr)
{
    QObject::connect(&splash, &Musec::Event::SplashScreen::openMainWindow,
                     this,    &MainWindow::openMainWindow);
    QObject::connect(this,    &MainWindow::openMainWindowComplete,
                     &splash, &Musec::Event::SplashScreen::closeDialog);
}

void MainWindow::openMainWindow()
{
    using namespace Musec::UI;
    using namespace Musec::Event;
    Musec::Controller::AudioEngineController::initializeFacility();
    engine->load(QUrl("qrc:///qmlproject/Musec.qml"));
    mainWindow = qobject_cast<QQuickWindow*>(engine->rootObjects()[1]);
    mainWindow->setIcon(QIcon(":/qmlproject/images/Musec-image-2.ico"));
    optionsWindow = mainWindow->findChild<QQuickWindow*>("optionsWindow");
    auto eventBridge = qvariant_cast<QObject*>(splashWindow->property("eventBridge"));
    Musec::Event::eventBridge = eventBridge;
    Musec::Event::eventHandler = &Musec::Event::EventHandler::instance(eventBridge);
    QObject::connect(eventHandler, &Musec::Event::EventHandler::updatePluginList,
                     this, &Musec::Event::MainWindow::updatePluginList);
    QObject::connect(this, &Musec::Event::MainWindow::openMainWindowComplete,
                     eventHandler, &Musec::Event::EventHandler::onMainWindowOpened);
    // Musec.qml, Musec.onActiveFocusItemChanged()
    auto visibility = mainWindow->visibility();
    mainWindow->showFullScreen();
    mainWindow->setVisibility(visibility);
    updateAssetDirectoryList();
    updatePluginList();
    mainWindow->setProperty("trackList", QVariant::fromValue(&Musec::Controller::AudioEngineController::AppTrackListModel()));
    mainWindow->setProperty("masterTrackPluginSequence", QVariant::fromValue(&Musec::Controller::AudioEngineController::AppTrackListModel().masterTrackPluginSequenceModel()));
    Musec::Controller::MIDIClockController::AppMIDIClock();
    openMainWindowComplete();
}

void MainWindow::updateAssetDirectoryList()
{
    using namespace Musec::UI;
    std::remove_reference_t<decltype(Musec::Controller::AppAssetDirectoryList())> list;
    mainWindow->setProperty("assetDirectoryList", QVariant::fromValue<QObject*>(&list));
    mainWindow->setProperty(
        "assetDirectoryList",
        QVariant::fromValue<QObject*>(&Musec::Controller::AppAssetDirectoryList()));
}

void MainWindow::updateASIODriverList()
{
    using namespace Musec::UI;
    std::remove_reference_t<decltype(Musec::Controller::AppASIODriverList())> list;
    mainWindow->setProperty("driverList", QVariant::fromValue<QObject*>(&list));
    auto& driverList = Musec::Controller::AppASIODriverList();
    mainWindow->setProperty("driverList",
        QVariant::fromValue<QObject*>(&driverList));
    QString driverCLSID = Controller::ASIODriverController::getASIODriver();
    auto driverListCount = driverList.itemCount();
    auto driverListBase = driverList.getList();
    int driverCurrentIndex = 0;
    for(int i = 0; i < driverListCount; ++i)
    {
        using namespace Musec::Audio::Driver;
        if(std::get<ASIODriverField::CLSIDField>(driverListBase[i]) == driverCLSID)
        {
            driverCurrentIndex = i;
            break;
        }
    }
    mainWindow->setProperty("currentDriver",
        QVariant::fromValue<int>(driverCurrentIndex)
    );
}

void MainWindow::updatePluginList()
{
    using namespace Musec::UI;
    mainWindow->setProperty(
        "pluginList",
        QVariant::fromValue<QObject*>(&Musec::Controller::AppPluginList())
    );
    mainWindow->setProperty(
        "midiEffectList",
        QVariant::fromValue<QObject*>(&Musec::Controller::AppMidiEffectList())
    );
    mainWindow->setProperty(
        "instrumentList",
        QVariant::fromValue<QObject*>(&Musec::Controller::AppInstrumentList())
    );
    mainWindow->setProperty(
        "audioEffectList",
        QVariant::fromValue<QObject*>(&Musec::Controller::AppAudioEffectList())
    );
}
}
