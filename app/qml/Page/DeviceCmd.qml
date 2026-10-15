import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qcm.Material as MD


ColumnLayout {
    MD.Label{ text: "Send to device"}
    MD.Button {
        text: "Send scene (background + widgets)"
        Layout.fillWidth: true
        enabled: app.ready
        onClicked: app.sendScene()
    }
    MD.Button {
        text: "Send background only"
        Layout.fillWidth: true
        enabled: app.canSendBackground
        onClicked: app.sendBackground()
    }
    MD.CheckBox {
        text: "Include background in scene"
        enabled: !app.busy && app.hasImage
        checked: app.includeBackground
        onToggled: app.includeBackground = checked
    }
    MD.CheckBox {
        text: "Refresh display after sending"
        enabled: !app.busy
        checked: app.refreshAfter
        onToggled: app.refreshAfter = checked
    }
    MD.LinearIndicator {
        Layout.fillWidth: true
        indeterminate: false
        value: app.progressValue / Math.max(1, app.progressMax)
        completionBehavior: MD.LinearIndicator.Keep
        wavy: true
    }

    // Device direct commands

    MD.Label{ text: "Device commands"}
    ColumnLayout {
        enabled: app.ready
        RowLayout {
            Layout.fillWidth: true
            MD.Button {
                text: "Refresh"
                Layout.fillWidth: true
                onClicked: app.refreshDisplay(false)
            }
            MD.Button {
                text: "Partial refresh"
                Layout.fillWidth: true
                onClicked: app.refreshDisplay(true)
            }
        }
        MD.Button {
            text: "Clear widgets"
            Layout.fillWidth: true
            onClicked: app.clearWidgets()
        }
        MD.Button {
            text: "Clear background"
            Layout.fillWidth: true
            onClicked: app.clearBackground()
        }
        RowLayout {
            Layout.fillWidth: true
            MD.Label { text: "Screen slot" }
            SpinBox { id: slotSpin; from: 0; to: 9; editable: true; Layout.fillWidth: true }
            MD.Button { text: "Save"; onClicked: app.saveScreen(slotSpin.value) }
            MD.Button { text: "Load"; onClicked: app.loadScreen(slotSpin.value) }
        }
        MD.Button {
            text: "Load default screen"
            Layout.fillWidth: true
            onClicked: app.loadDefaultScreen()
        }
    }
}