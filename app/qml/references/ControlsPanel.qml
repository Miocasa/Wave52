import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 8

    signal openRequested()
    signal saveBinRequested()
    signal savePngRequested()


    // ---------------- Device ----------------
    Section {
        title: "Device"

        Button {
            text: "Scan"
            Layout.fillWidth: true
            enabled: !app.busy && !app.connected
            onClicked: app.scan()
        }
        ComboBox {
            id: deviceCombo
            Layout.fillWidth: true
            model: app.deviceNames
            enabled: !app.busy && !app.connected
        }
        Connections {
            target: app
            function onDevicesChanged() {
                // model обновится тем же сигналом — выставляем индекс после него
                Qt.callLater(() => deviceCombo.currentIndex =
                        app.preferredDevice >= 0 ? app.preferredDevice : 0)
            }
        }
        Button {
            text: app.connected ? "Disconnect" : "Connect"
            Layout.fillWidth: true
            enabled: !app.busy && (app.connected || deviceCombo.count > 0)
            onClicked: app.toggleConnection(deviceCombo.currentIndex)
        }
        Label { text: app.connected ? "Connected" : "Not connected" }
    }

    // ---------------- Background image ----------------
    Section {
        title: "Background image"

        Button {
            text: "Open..."
            Layout.fillWidth: true
            onClicked: root.openRequested()
        }
        Label {
            text: app.fileName
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 8
            rowSpacing: 4

            Label { text: "Mode" }
            ComboBox {
                Layout.fillWidth: true
                model: ["Cover", "Contain", "Stretch"]
                currentIndex: app.fit
                onActivated: app.fit = currentIndex
            }

            Label { text: "Size" }
            RowLayout {
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: 8; to: 2000
                    value: app.imgWidth
                    onValueModified: app.imgWidth = value
                }
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: 8; to: 2000
                    value: app.imgHeight
                    onValueModified: app.imgHeight = value
                }
            }

            Label { text: "Rotation" }
            ComboBox {
                Layout.fillWidth: true
                model: ["0°", "90°", "180°", "270°"]
                currentIndex: app.rotation / 90
                onActivated: app.rotation = currentIndex * 90
            }

            Label { text: "Brightness" }
            LabeledSlider {
                from: -100; to: 100
                value: app.brightness
                onValueModified: (v) => app.brightness = v
            }

            Label { text: "Contrast" }
            LabeledSlider {
                from: -100; to: 100
                value: app.contrast
                onValueModified: (v) => app.contrast = v
            }

            Label { text: "Margin color" }
            LabeledSlider {
                from: 0; to: 255
                enabled: app.fit === 1 // только для Contain
                value: app.background
                onValueModified: (v) => app.background = v
            }

            CheckBox {
                Layout.columnSpan: 2
                text: "Dithering (Floyd–Steinberg)"
                checked: app.dither
                onToggled: app.dither = checked
            }
            CheckBox {
                Layout.columnSpan: 2
                text: "Invert colors"
                checked: app.invert
                onToggled: app.invert = checked
            }
        }

        Button {
            text: "Reset settings"
            Layout.fillWidth: true
            onClicked: app.resetAdjustments()
        }
        Button {
            text: "Save .bin"
            Layout.fillWidth: true
            enabled: app.hasImage
            onClicked: root.saveBinRequested()
        }
        Button {
            text: "Save screen preview PNG"
            Layout.fillWidth: true
            enabled: app.hasScreen
            onClicked: root.savePngRequested()
        }
    }

    // ---------------- Text widgets ----------------
    WidgetEditor {}

    // ---------------- Send ----------------
    Section {
        title: "Send to device"

        Button {
            text: "Send scene (background + widgets)"
            Layout.fillWidth: true
            enabled: app.ready
            onClicked: app.sendScene()
        }
        Button {
            text: "Send background only"
            Layout.fillWidth: true
            enabled: app.canSendBackground
            onClicked: app.sendBackground()
        }
        CheckBox {
            text: "Include background in scene"
            enabled: !app.busy && app.hasImage
            checked: app.includeBackground
            onToggled: app.includeBackground = checked
        }
        CheckBox {
            text: "Refresh display after sending"
            enabled: !app.busy
            checked: app.refreshAfter
            onToggled: app.refreshAfter = checked
        }
        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: Math.max(1, app.progressMax)
            value: app.progressValue
        }
    }

    // ---------------- Device commands ----------------
    Section {
        title: "Device commands"
        enabled: app.ready

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: "Refresh"
                Layout.fillWidth: true
                onClicked: app.refreshDisplay(false)
            }
            Button {
                text: "Partial refresh"
                Layout.fillWidth: true
                onClicked: app.refreshDisplay(true)
            }
        }
        Button {
            text: "Clear widgets"
            Layout.fillWidth: true
            onClicked: app.clearWidgets()
        }
        Button {
            text: "Clear background"
            Layout.fillWidth: true
            onClicked: app.clearBackground()
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Screen slot" }
            SpinBox { id: slotSpin; from: 0; to: 9; editable: true; Layout.fillWidth: true }
            Button { text: "Save"; onClicked: app.saveScreen(slotSpin.value) }
            Button { text: "Load"; onClicked: app.loadScreen(slotSpin.value) }
        }
        Button {
            text: "Load default screen"
            Layout.fillWidth: true
            onClicked: app.loadDefaultScreen()
        }
    }

}
