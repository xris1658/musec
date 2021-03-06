import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

import Musec 1.0

Window {
    id: splashScreen
    flags: Qt.SplashScreen
    title: Strings.applicationName
    property EventBridge eventBridge: EventBridge
    property Strings strings: Strings

    property alias bootText: text_.text
    signal setBootText(newBootText: string)

    onSetBootText: {
        bootText = newBootText;
    }

    width: 810
    height: 540
    visible: true

    Component.onCompleted: {
        Objects.splashScreen = splashScreen;
    }
    signal initDialog()
    signal closeDialog()

    onCloseDialog: {
        close();
    }

    Image {
        source: "images/Splashscreen.png"
        anchors.fill: parent
        Text {
            id: text_
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 45
            anchors.bottomMargin: 45
            text: qsTr("Starting...")
            color: "#C0C0C0"
            font.family: Constants.font
            font.pixelSize: 15
        }
    }
}

