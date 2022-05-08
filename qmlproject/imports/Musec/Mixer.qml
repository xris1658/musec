import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15

import Musec.Controls 1.0 as MCtrl
import Musec.Models 1.0 as MModel

Rectangle {
    id: root
    color: Constants.backgroundColor2
    property MModel.TrackListModel tracks
    property bool effectVisible: true
    property bool gainAndMeterVisible: true
    clip: true

    MCtrl.Menu {
        id: instrumentSlotOptions
        property int trackIndex
        title: qsTr("乐器操作")
        delegate: MCtrl.MenuItem {}
        width: 100
        height: contentHeight
        MCtrl.Action {
            text: qsTr("删除(&D)")
            onTriggered: {
                instrumentSlotOptions.parent = null;
                tracks.removeInstrument(instrumentSlotOptions.trackIndex);
            }
        }
    }
    MCtrl.Menu {
        id: audioEffectSlotOptions
        property int trackIndex
        property int effectIndex
        title: qsTr("音频效果器操作")
        delegate: MCtrl.MenuItem {}
        width: 100
        height: contentHeight
        MCtrl.Action {
            text: qsTr("删除(&D)")
            onTriggered: {
                audioEffectSlotOptions.parent = null;
                tracks.removeEffect(audioEffectSlotOptions.trackIndex, audioEffectSlotOptions.effectIndex);
            }
        }
    }
    signal instrumentSlotRightClicked(trackIndex: int, x: int, y: int)
    onInstrumentSlotRightClicked: {
        instrumentSlotOptions.parent = (trackChannelList.itemAtIndex(trackIndex).mixerChannelOfThis.instrumentSlot);
        instrumentSlotOptions.trackIndex = trackIndex;
        instrumentSlotOptions.x = x;
        instrumentSlotOptions.y = y;
        instrumentSlotOptions.open();
    }

    signal audioEffectSlotRightClicked(trackIndex: int, audioEffectIndex: int, x: int, y: int)
    onAudioEffectSlotRightClicked: {
        audioEffectSlotOptions.parent = trackChannelList.itemAtIndex(trackIndex).mixerChannelOfThis.getAudioEffectSlot(audioEffectIndex);
        audioEffectSlotOptions.trackIndex = trackIndex;
        audioEffectSlotOptions.effectIndex = audioEffectIndex;
        audioEffectSlotOptions.x = x;
        audioEffectSlotOptions.y = y;
        audioEffectSlotOptions.open();
    }

    Rectangle {
        id: mixerContentIndicator
        width: 25
        height: parent.height
        color: Constants.backgroundColor
        MCtrl.Button {
            id: effectVisibleToggleButton
            width: parent.width
            height: 20
            visible: root.gainAndMeterVisible
            anchors.top: parent.top
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
            width: parent.width
            height: 20
            visible: root.effectVisible
            anchors.bottom: parent.bottom
            anchors.bottomMargin: scroll.height + masterChannel.channelInfoHeight
            border.width: 0
            color: root.gainAndMeterVisible? Constants.mouseOverElementColor: Constants.backgroundColor
            Image {
                source: "../../images/show-fader.svg"
                width: 16
                height: 16
                anchors.centerIn: parent
            }

            MCtrl.ToolTip {
                visible: parent.hovered
                text: qsTr("显示音量推子和电平表")
            }
            onClicked: {
                root.gainAndMeterVisible = !root.gainAndMeterVisible;
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
            clip: false
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
                id: trackChannelListDelegate
                clip: false
                z: trackChannelList.count - index - 1
                property MixerChannel mixerChannelOfThis: mixerChannel
                MixerChannel {
                    id: mixerChannel
                    z: 2
                    clip: false
                    effectListModel: plugin_list
                    channelName: trackname
                    channelColor: trackcolor
                    channelType: type
                    width: 120
                    height: root.height - scroll.height
                    channelNumber: index + 1
                    effectVisible: root.effectVisible
                    gainAndMeterVisible: root.gainAndMeterVisible
                    channelMuted: mute
                    channelSolo: solo
                    channelInverted: invertPhase
                    channelArmRecording: armRecording
                    instrumentEnabled: instrument? instrument.enabled: false
                    instrumentName: instrument? instrument.name: ""
                    instrumentSidechainExist: instrument? instrument.sidechainExist: false
                    instrumentSidechainEnabled: instrument? instrument.sidechainEnabled: false
                    instrumentEditorVisible: instrument? (instrument && instrument.windowVisible): false
                    onSetMute: (newMute) => {
                                   mute = newMute;
                               }
                    onSetSolo: (newSolo) => {
                                   solo = newSolo;
                               }
                    onSetInvertPhase: (newInvertPhase) => {
                                          invertPhase = newInvertPhase;
                                      }
                    onSetArmRecording: (newArmRecording) => {
                                           armRecording = newArmRecording;
                                       }
                    onLoadInstrument: {
                        tracks.loadInstrument(index, pluginFormat, pluginPath, pluginSubId);
                    }
                    onInsertEffect: {
                        tracks.insertEffect(index, pluginFormat, pluginPath, pluginSubId, effectIndex);
                    }
                    onReplaceEffect: {
                        tracks.replaceEffect(index, pluginFormat, pluginPath, pluginSubId, effectIndex);
                    }
                    onInstrumentSlotVisibleToggled: {
                        if(instrument) {
                            instrument.windowVisible = instrumentWindowVisible;
                        }
                    }
                    onAudioEffectSlotVisibleToggled: {
                        plugin_list.setWindowVisible(effectIndex, audioEffectWindowVisible);
                    }
                    onInstrumentSlotRightClicked: {
                        root.instrumentSlotRightClicked(index, x, y);
                    }
                    onAudioSlotRightClicked: (audioEffectIndex, menuX, menuY) => {
                        root.audioEffectSlotRightClicked(index, audioEffectIndex, menuX, menuY);
                    }
                }
                Rectangle {
                    z: 1
                    width: 1
                    height: root.height
                    color: Constants.borderColor
                }
            }
        }
    }
}
