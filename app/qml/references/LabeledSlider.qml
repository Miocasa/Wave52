import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root

    property alias from: slider.from
    property alias to: slider.to
    property alias value: slider.value
    signal valueModified(int v)

    Layout.fillWidth: true

    Slider {
        id: slider
        Layout.fillWidth: true
        stepSize: 1
        snapMode: Slider.SnapAlways
        onMoved: root.valueModified(Math.round(value))
    }
    Label {
        text: Math.round(slider.value)
        Layout.minimumWidth: 34
        horizontalAlignment: Text.AlignRight
    }
}
