import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GroupBox {
    default property alias content: col.data

    Layout.fillWidth: true

    ColumnLayout {
        id: col
        anchors.fill: parent
        spacing: 6
    }
}
