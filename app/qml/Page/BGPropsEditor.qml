import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Qcm.Material as MD
import Wave52

ColumnLayout {
    GridLayout {
    Layout.fillWidth: true
    columns: 2
    columnSpacing: 8
    rowSpacing: 4

    MD.Label { text: "Mode" }
    MD.ComboBox {
        Layout.fillWidth: true
        model: ["Cover", "Contain", "Stretch"]
        currentIndex: app.fit
        onActivated: app.fit = currentIndex
    }

    MD.Label { text: "Size" }
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

    MD.Label { text: "Rotation" }
    ComboBox {
        Layout.fillWidth: true
        model: ["0°", "90°", "180°", "270°"]
        currentIndex: app.rotation / 90
        onActivated: app.rotation = currentIndex * 90
    }

    MD.Label { text: "Brightness" }
    LabeledSlider {
        from: -100; to: 100
        value: app.brightness
        onValueModified: (v) => app.brightness = v
    }

    MD.Label { text: "Contrast" }
    LabeledSlider {
        from: -100; to: 100
        value: app.contrast
        onValueModified: (v) => app.contrast = v
    }

    MD.Label { text: "Margin color" }
    LabeledSlider {
        from: 0; to: 255
        enabled: app.fit === 1
        value: app.background
        onValueModified: (v) => app.background = v
    }

    MD.CheckBox {
        Layout.columnSpan: 2
        text: "Dithering (Floyd–Steinberg)"
        checked: app.dither
        onToggled: app.dither = checked
    }
    MD.CheckBox {
        Layout.columnSpan: 2
        text: "Invert colors"
        checked: app.invert
        onToggled: app.invert = checked
    }
    }

    MD.Button {
        text: "Reset settings"
        Layout.fillWidth: true
        onClicked: app.resetAdjustments()
    }

}