import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtQml.Models 2.15

import Musec 1.0
import Musec.Controls 1.0 as MCtrl
import Musec.Entities 1.0

Item {
    id: root
    width: 80
    height: 320
    clip: false
    Rectangle {
        anchors.fill: parent
        color: Constants.backgroundColor
    }
    property bool instrumentEnabled: false
    property string instrumentName
    property bool instrumentSidechainExist: false
    property bool instrumentSidechainEnabled: false
    property bool instrumentEditorVisible: false
    property alias effectListModel: channelEffectList.model
    readonly property int channelInfoHeight: 20
    readonly property int channelEffectListFooterMinimumHeight: 20
    property Item instrumentSlot
    signal instrumentSlotRightClicked(x: int, y: int)
    function getAudioEffectSlot(index: int) {
        return channelEffectList.itemAtIndex(index);
    }
    signal audioSlotRightClicked(index: int, x: int, y: int)
    // 乐器操作
    signal instrumentSlotDragEventEntered(drag: var)
    onInstrumentSlotDragEventEntered: {
        if(drag.getDataAsString("type") != 2) {
            drag.accepted = false;
        }
    }
    signal instrumentSlotDragEventDropped(drop: var)
    onInstrumentSlotDragEventDropped: {
        var format = parseInt(drop.getDataAsString("format"));
        var path = drop.getDataAsString("path");
        var pluginSubId = parseInt(drop.getDataAsString("uid"));
        loadInstrument(path, pluginSubId, format);
    }
    signal instrumentSlotVisibleToggled(instrumentWindowVisible: bool)
    signal audioEffectSlotVisibleToggled(audioEffectWindowVisible: bool, effectIndex: int)
    // 效果器操作
    signal audioEffectSlotDragEventEntered(drag: var, audioEffectIndex: int)
    onAudioEffectSlotDragEventEntered: {
        if(drag.getDataAsString("type") != 3) {
            drag.accepted = false;
        }
    }
    signal audioEffectSlotDragEventDropped(drop: var, audioEffectIndex: int)
    onAudioEffectSlotDragEventDropped: {
        var format = parseInt(drop.getDataAsString("format"));
        var path = drop.getDataAsString("path");
        var pluginSubId = parseInt(drop.getDataAsString("uid"));
        replaceEffect(path, pluginSubId, format, audioEffectIndex);
    }
    signal betweenAudioEffectSlotDragEventEntered(drag: var, audioEffectIndex: int)
    onBetweenAudioEffectSlotDragEventEntered: {
        if(drag.getDataAsString("type") != 3) {
            drag.accepted = false;
        }
    }

    signal betweenAudioEffectSlotDragEventDropped(drop: var, audioEffectIndex: int)
    onBetweenAudioEffectSlotDragEventDropped: {
        var format = parseInt(drop.getDataAsString("format"));
        var path = drop.getDataAsString("path");
        var pluginSubId = parseInt(drop.getDataAsString("uid"));
        insertEffect(path, pluginSubId, format, audioEffectIndex);
    }
    signal blankAreaDragEventEntered(drag: var)
    onBlankAreaDragEventEntered: {
        if(drag.getDataAsString("type") != 3 && root.channelType != CompleteTrack.InstrumentTrack) {
            drag.accepted = false;
        }
    }
    signal blankAreaDragEventDropped(drop: var)
    onBlankAreaDragEventDropped: {
        var format = parseInt(drop.getDataAsString("format"));
        var path = drop.getDataAsString("path");
        var pluginSubId = parseInt(drop.getDataAsString("uid"));
        if(drop.getDataAsString("type") == 2) {
            loadInstrument(path, pluginSubId, format);
        }
        else {
            insertEffect(path, pluginSubId, format, channelEffectList.count);
        }
    }
    signal loadInstrument(pluginPath: string, pluginSubId: int, pluginFormat: int)
    signal insertEffect(pluginPath: string, pluginSubId: int, pluginFormat: int, effectIndex: int)
    signal replaceEffect(pluginPath: string, pluginSubId: int, pluginFormat: int, effectIndex: int)
    property int channelType
    property bool channelMuted: false
    property bool channelSolo: false
    property bool channelInverted: false
    property bool channelArmRecording: false
    property real panning: 0.0
    property real stereo: 1.0
    property real gain: 1.00
    property real peak: 0.00
    property string channelName
    property color channelColor
    property int channelNumber: 0
    property alias effectVisible: channelEffects.visible
    property alias gainAndMeterVisible: channelGainAndPeak.visible
    signal setMute(newMute: bool)
    signal setSolo(newSolo: bool)
    signal setInvertPhase(newInvertPhase: bool)
    signal setArmRecording(newArmRecording: bool)
    Column {
        width: parent.width
        height: parent.height
        Item {
            id: channelEffects
            width: parent.width
            height: (root.height - channelControl.height - channelStereo.height - channelInfo.height) / (gainAndMeterVisible? 2: 1)
            clip: false
            ListView {
                id: channelEffectList
                anchors.fill: parent
                anchors.margins: 2
                visible: parent.visible
                spacing: 2
                interactive: false
                clip: false
                property int headerHeight: 20
                header: Item {
                    id: effectListHeader
                    width: parent.width
                    height: channelEffectList.headerHeight + channelEffectList.spacing
                    MixerSlot {
                        id: instrumentButton
                        visible: root.channelType == CompleteTrack.InstrumentTrack
                        anchors.left: parent.left
                        anchors.fill: parent
                        anchors.bottomMargin: channelEffectList.spacing
                        name: root.instrumentName
                        slotEnabled: root.instrumentEnabled
                        sidechainExist: root.instrumentSidechainExist
                        sidechainEnabled: root.instrumentSidechainEnabled
                        editorVisible: root.instrumentEditorVisible
                        onEntered: {
                            root.instrumentSlotDragEventEntered(drag);
                        }
                        onDropped: {
                            root.instrumentSlotDragEventDropped(drop);
                        }
                        onClicked: {
                            root.instrumentSlotVisibleToggled(!editorVisible);
                        }
                        onEnabledButtonClicked: {
                            //
                        }
                        onRightClicked: {
                            root.instrumentSlot = instrumentButton;
                            root.instrumentSlotRightClicked(x, y);
                        }
                    }
                }
                delegate: Item {
                    width: parent.width
                    height: channelEffectList.headerHeight
                    MixerSlot {
                        anchors.fill: parent
                        name: plugin_name
                        slotEnabled: valid && activated
                        sidechainExist: sidechain_exist
                        sidechainEnabled: sidechain_enabled
                        editorVisible: window_visible
                        onEntered: {
                            root.audioEffectSlotDragEventEntered(drag, index);
                        }
                        onDropped: {
                            root.audioEffectSlotDragEventDropped(drop, index);
                        }
                        onClicked: {
                            root.audioEffectSlotVisibleToggled(!editorVisible, index);
                        }
                        onEnabledButtonClicked: {
                            //
                        }
                        onRightClicked: {
                            root.audioSlotRightClicked(index, x, y);
                        }
                    }
                    DropArea {
                        id: betweenMixerSlot
                        width: parent.width
                        height: 10
                        anchors.top: parent.top
                        anchors.topMargin: (-height) / 2
                        Rectangle {
                            anchors.fill: parent
                            color: betweenMixerSlot.containsDrag? Constants.mouseOverElementColor: "transparent"
                        }
                        onEntered: {
                            root.betweenAudioEffectSlotDragEventEntered(drag, index);
                        }
                        onDropped: {
                            root.betweenAudioEffectSlotDragEventDropped(drop, index);
                        }
                    }
                }
                footer: Item {
                    width: parent.width
                    height: channelEffects.height <= channelEffectList.headerHeight + 20 * channelEffectList.count + root.channelEffectListFooterMinimumHeight?
                        root.channelEffectListFooterMinimumHeight:
                        channelEffects.height - channelEffectList.headerHeight - 20 * channelEffectList.count
                    Rectangle {
                        anchors.fill: parent
                        anchors.topMargin: channelEffectList.spacing
                        color: channelEffectDropArea.containsDrag? Constants.mouseOverElementColor: "transparent"
                    }
                    DropArea {
                        id: channelEffectDropArea
                        anchors.fill: parent
                        onEntered: {
                            root.blankAreaDragEventEntered(drag);
                        }
                        onDropped: {
                            root.blankAreaDragEventDropped(drop);
                        }
                    }
                }
            }
        }
        Item {
            id: channelControl
            width: parent.width
            height: 20
            clip: true
            Row {
                Item {
                    width: root.width / 4
                    height: channelControl.height
                    MCtrl.Button {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        border.width: 2
                        border.color: Constants.muteIndicatorColor
                        color: channelMuted? Qt.darker(border.color, 1.5) : Constants.backgroundColor
                        Text {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: -1.2
                            text: "M"
                            font.family: "Noto Sans Mono"
                            font.styleName: "Condensed SemiBold"
                            font.pixelSize: 15
                            color: Constants.contentColor1
                        }
                        onClicked: {
                            setMute(!channelMuted);
                        }
                    }
                }
                Item {
                    width: root.width / 4
                    height: channelControl.height
                    MCtrl.Button {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        border.width: 2
                        border.color: Constants.soloIndicatorColor
                        color:  channelSolo? Qt.darker(border.color, 1.5) : Constants.backgroundColor
                        Text {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: -1.2
                            text: "S"
                            font.family: "Noto Sans Mono"
                            font.styleName: "Condensed SemiBold"
                            font.pixelSize: 15
                            color: Constants.contentColor1
                        }
                        onClicked: {
                            setSolo(!channelSolo);
                        }
                    }
                }
                Item {
                    width: root.width / 4
                    height: channelControl.height
                    MCtrl.Button {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        border.width: 2
                        border.color: Constants.invertIndicatorColor
                        color:  channelInverted? border.color : Constants.backgroundColor
                        Text {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: -1.2
                            text: "\u2205"
                            font.family: "Noto Sans Mono"
                            font.styleName: "Condensed SemiBold"
                            font.pixelSize: 15
                            color: Constants.contentColor1
                        }
                        onClicked: {
                            setInvertPhase(!channelInverted);
                        }
                    }
                }
                Item {
                    width: root.width / 4
                    height: channelControl.height
                    MCtrl.Button {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        border.width: 2
                        border.color: Constants.recordIndicatorColor
                        color: channelArmRecording? border.color : Constants.backgroundColor
                        Text {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: -1.2
                            text: "R"
                            font.family: "Noto Sans Mono"
                            font.styleName: "Condensed SemiBold"
                            font.pixelSize: 15
                            color: Constants.contentColor1
                        }
                        onClicked: {
                           setArmRecording(!channelArmRecording);
                        }
                    }
                }
            }
        }
        Item {
            id: channelStereo
            width: parent.width
            height: 20
            clip: true
            Row {
                Item {
                    width: root.width / 2
                    height: 20
                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        color: "transparent"
                        border.width: 1
                        border.color: Constants.borderColor
                        z: 2
                        clip: true
                    }
                    Rectangle {
                        color: "#1D2088"
                        width: panning>0? parent.width * panning / 2 : parent.width * panning / -2
                        height: parent.height - 2
                        x: panning>0? parent.width / 2: parent.width / 2 - width
                        y: 1
                        z: 0
                    }
                    Text {
                        anchors.centerIn: parent
                        color: Constants.contentColor1
                        font.family: Constants.font
                        text: panning == 0? "C" : panning < 0? panning * -100 + "L" : panning * 100 + "R"
                        z: 1
                    }
                }
                Item {
                    width: root.width / 2
                    height: 20
                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width - 2
                        height: parent.height - 2
                        color: "transparent"
                        border.width: 1
                        border.color: Constants.borderColor
                        z: 2
                        clip: true
                    }
                    Text {
                        anchors.centerIn: parent
                        color: Constants.contentColor1
                        font.family: Constants.font
                        text: stereo == 0? "Mono" : stereo * 100 + "%"
                        z: 1
                    }
                }
            }
        }
        Item {
            id: channelGainAndPeak
            width: parent.width
            height: root.height - (effectVisible? channelEffects.height: 0) - channelControl.height - channelStereo.height - channelInfo.height
            clip: true
            Row {
                visible: parent.visible
                Column {
                    Item {
                        width: root.width / 2
                        height: channelGainAndPeak.height - 35
                        MCtrl.Slider {
                            topPadding: 5
                            id: mixerChannelSlider
                            implicitThickness: 4
                            handleImplicitThickness: 12
                            handleImplicitLength: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                            orientation: Qt.Vertical
                            width: 20
                            height: parent.height
                            from: -96
                            to: 6
                            value: 20 * Math.log10(gain)
                            onValueChanged: {
                                gain = Math.pow(10.0, value * 0.05);
                            }
                        }
                    }
                    Item {
                        width: root.width / 2
                        height: 35
                        Popup {
                            width: parent.width * 0.75
                            height: 20
                            anchors.centerIn: parent
                            padding: 0
                            id: gainTextInputPopup
                            visible: false
                            clip: false
                            background: Rectangle {
                                color: Constants.backgroundColor
                                border.width: 1
                                border.color: Constants.borderColor
                            }
                            TextField {
                                id: gainTextInput
                                focus: true
                                anchors.fill: parent
                                text: mixerChannelGainText.text
                                color: Constants.contentColor1
                                selectionColor: Constants.currentElementColor
                                selectedTextColor: Constants.backgroundColor
                                horizontalAlignment: TextInput.AlignHCenter
                                placeholderText: "0.00"
                                placeholderTextColor: Constants.currentElementColor
                                padding: 0
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                background: Rectangle {
                                    color: Constants.backgroundColor
                                    border.width: 1
                                    border.color: Constants.borderColor
                                }
                                font.family: Constants.font
                                onAccepted: {
                                    gain = text.length == 0? 1.0: Math.pow(10.0, text * 0.05);
                                    gainTextInputPopup.visible = false;
                                }
                            }
                        }
                        Text {
                            id: mixerChannelGainText
                            anchors.centerIn: parent
                            text: gain == 0? "-inf": (20 * Math.log10(gain)).toPrecision(3)
                            font.family: Constants.font
                            color: Constants.contentColor1
                        }
                        MouseArea {
                            id: gainMouseArea
                            anchors.fill: parent
                            onClicked: {
                                gainTextInputPopup.visible = true;
                                gainTextInput.text = mixerChannelGainText.text;
                                gainTextInput.selectAll();
                                gainTextInput.forceActiveFocus();
                            }
                        }
                    }
                }
                Column {
                    Item {
                        id: levelPlaceholder
                        width: root.width / 2
                        height: channelGainAndPeak.height - 35
                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 5
                            topPadding: 5
                            Level {
                                width: 10
                                height: levelPlaceholder.height - 5
                                level: peak
                            }
                            Level {
                                width: 10
                                height: levelPlaceholder.height- 5
                                level: peak
                            }
                        }
                    }
                    Item {
                        width: root.width / 2
                        height: 35
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width * 0.75
                            height: 20
                            color: peak > 1 ? Constants.peakAboveZeroColor : Constants.backgroundColor
                            border.width: 1
                            border.color: Constants.borderColor
                            Text {
                                anchors.centerIn: parent
                                text: peak == 0? "-inf": (20 * Math.log10(peak)).toPrecision(3)
                                font.family: Constants.font
                                color: peak > 1? "#000000" : Constants.contentColor1
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: channelInfo
            width: parent.width
            height: channelInfoHeight
            color: channelColor
            Row {
                Item {
                    id: mixerChannelIndex
                    width: channelNumber? 30: 0
                    height: channelInfoHeight
                    clip: true
                    Text {
                        id: textInfoIndex
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: 1
                        text: channelNumber
                        font.family: "Noto Sans Mono"
                        font.styleName: "Condensed SemiBold"
                        color: Constants.contentColorOnBackground(channelColor)
                    }
                }
                Item {
                    width: root.width - mixerChannelIndex.width
                    height: channelInfoHeight
                    Text {
                        anchors.centerIn: parent
                        text: channelName
                        font.family: Constants.font
                        color: textInfoIndex.color
                    }
                }
            }
        }
    }
}

