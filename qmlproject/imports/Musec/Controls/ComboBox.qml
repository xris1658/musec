import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Shapes 1.15
import QtQml.Models 2.15

import Musec 1.0
import Musec.Controls 1.0

QQC2.ComboBox {
    enum ShowToolTip {
        Never,
        Needed,
        Always
    }
    property int showToolTip: ComboBox.ShowToolTip.Needed
    property string toolTipText: currentText
    id: comboBox
    opacity: enabled? 1.0: 0.5
    property alias color: _background.color
    property alias border: _background.border
    font.family: Constants.font
    height: 20
    property int popupMinWidth
    property int popupWidth: Math.max(width, popupMinWidth)
    ToolTip {
        text: currentText
        visible: showToolTip === ComboBox.ShowToolTip.Always? hovered:
            showToolTip === ComboBox.ShowToolTip.Needed? (popupWidth < width) && hovered:
            false
    }
    delegate: QQC2.ItemDelegate {
        id: comboBoxDelegate
        width: comboBox.popupWidth
        height: 20
        background: Rectangle { // 实现高亮
            width: comboBoxDelegate.width
            height: comboBoxDelegate.height
            color: comboBox.highlightedIndex === index? Constants.menuHighlightBackgroundColor : Constants.menuBackgroundColor
            Item {
                id: currentSelectionIndicator
                anchors.right: parent.right
                width: parent.height
                height: width
                visible: comboBox.currentIndex === index
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width / 3
                    height: parent.width / 3
                    radius: width / 2
                    color: Constants.menuContentColor
                }
            }
        }
        contentItem: Rectangle {
            id: content
            width: comboBoxDelegate.width
            height: comboBoxDelegate.height
            Text {
                id: contentText
                function updatePopupMinWidth() {
                    if(contentText.contentWidth + comboBoxDelegate.height - anchors.leftMargin > comboBox.popupMinWidth) {
                        comboBox.popupMinWidth = contentText.contentWidth + comboBoxDelegate.height - anchors.leftMargin;
                    }
                }
                Component.onCompleted: {
                    updatePopupMinWidth();
                }
                onContentHeightChanged: {
                    updatePopupMinWidth();
                }
                anchors.left: parent.left
                anchors.leftMargin: -5
                anchors.verticalCenter: content.verticalCenter
                anchors.verticalCenterOffset: -2
                text: comboBox.textRole ? (Array.isArray(comboBox.model) ? modelData[comboBox.textRole] : model[comboBox.textRole]) : modelData
                color: comboBox.highlightedIndex === index? Constants.menuHighlightContentColor: Constants.menuContentColor
                font: comboBox.font
            }
        }
    }
    indicator: Item {
        anchors.right: parent.right
        width: parent.height
        height: width
        Text {
            anchors.centerIn: parent
            text: "\u25BC"
            font.pixelSize: parent.height * 0.4
            color: Constants.contentColor1
        }
    }
    contentItem: Text {
        id: contentItemAsText
        anchors.left: parent.left
        anchors.leftMargin: 5
        text: parent.displayText
        font: parent.font
        color: Constants.contentColor1
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    // 控件背景
    background: Rectangle {
        id: _background
        anchors.fill: parent
        color: Constants.backgroundColor
        border.color: Constants.borderColor
        border.width: comboBox.activeFocus? 2: 1
    }
    popup: Menu {
        width: comboBox.popupWidth
        y: parent.height
        implicitHeight: contentItem.implicitHeight + 2
        padding: 1
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: comboBox.delegateModel
            currentIndex: comboBox.currentIndex
            focus: true
            interactive: false
        }
    }
}
