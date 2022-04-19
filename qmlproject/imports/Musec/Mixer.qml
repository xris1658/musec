import QtQuick 2.15
import QtQuick.Controls 2.15

import Musec.Controls 1.0 as MCtrl
import Musec.Models 1.0 as MModel

Rectangle {
    id: root
    color: Constants.backgroundColor2
    property MModel.TrackListModel tracks
    property bool effectVisible: true
    property bool gainAndMeterVisible: true
    clip: true
    Rectangle {
        id: mixerContentIndicator
        width: 25
        height: parent.height
        color: Constants.backgroundColor
        Column {
            MCtrl.Button {
                id: effectVisibleToggleButton
                width: mixerContentIndicator.width
                height: 20
                border.width: 0
                color: root.effectVisible? Constants.mouseOverElementColor: Constants.backgroundColor
                Text {
                    text: qsTr("fx")
                    font.family: "Noto Sans Mono"
                    font.styleName: "Condensed"
                    font.italic: true
                    anchors.centerIn: parent
                    color: Constants.contentColor1
                }
                MCtrl.ToolTip {
                    visible: parent.hovered
                    text: qsTr("显示效果")
                }
                onClicked: {
                    root.effectVisible  = !root.effectVisible;
                }
            }
            MCtrl.Button {
                id: gainAndMeterVisibleToggleButton
                width: mixerContentIndicator.width
                height: 20
                border.width: 0
                color: root.gainAndMeterVisible? Constants.mouseOverElementColor: Constants.backgroundColor
                Text {
                    text: qsTr("dB")
                    font.family: "Noto Sans Mono"
                    font.styleName: "Condensed"
                    font.italic: true
                    anchors.centerIn: parent
                    color: Constants.contentColor1
                }
                MCtrl.ToolTip {
                    visible: parent.hovered
                    text: qsTr("显示电平表和增益推子")
                }
                onClicked: {
                    root.gainAndMeterVisible = !root.gainAndMeterVisible;
                }
            }
        }
        Rectangle {
            anchors.right: parent.right
            anchors.top: parent.top
            width: 1
            height: parent.height
            color: Constants.borderColor
            z: 2
        }
    }
    Rectangle {
        id: mixerContentArea
        anchors.left: mixerContentIndicator.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        clip: true
        color: Constants.backgroundColor2
        MixerChannel {
            id: masterChannel
            channelName: qsTr("Master")
            channelColor: Constants.backgroundColor2
            channelNumber: 0
            width: 120
            height: root.height - scroll.height
            effectVisible: root.effectVisible
            gainAndMeterVisible: root.gainAndMeterVisible
        }
        Rectangle {
            id: masterChannelRightBorder
            anchors.left: masterChannel.right
            anchors.top: parent.top
            width: 1
            height: parent.height
            color: Constants.borderColor
            z: 2
        }
        ListView {
            id: trackChannelList
            orientation: Qt.Horizontal
            anchors.left: masterChannelRightBorder.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            clip: true
            model: tracks
            interactive: false
            ScrollBar.horizontal: ScrollBar {
                id: scroll
                parent: mixerContentArea
                width: parent.width - masterChannel.width - masterChannelRightBorder.width
                anchors.right: parent.right
                height: 15
                anchors.bottom: parent.bottom
                background: Rectangle {
                    color: Constants.backgroundColor
                }
                contentItem: Rectangle {
                    radius: 2
                    color: Constants.mouseOverElementColor
                    opacity: trackChannelList.width < trackChannelList.contentWidth? 1: 0
                }
            }
            footer: Item {
                width: 50
                height: root.height - scroll.height
            }
            delegate: Row {
                MixerChannel {
                    channelName: trackname
                    channelColor: trackcolor
                    width: 120
                    height: root.height - scroll.height
                    channelNumber: index + 1
                    effectVisible: root.effectVisible
                    gainAndMeterVisible: root.gainAndMeterVisible
                }
                Rectangle {
                    width: 1
                    height: root.height
                    color: Constants.borderColor
                }
            }
        }
    }
}
