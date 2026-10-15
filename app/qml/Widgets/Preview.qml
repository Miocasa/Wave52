import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qcm.Material as MD

MD.Pane {
    id: box

    property alias source: img.source
    property real maxWidth: 400

    // true 4:3, false 3:4
    property bool landscape: true
    readonly property real aspect: landscape ? 4 / 3 : 3 / 4

    showBackground: true
    MD.MProp.backgroundColor: MD.MProp.color.surface_container_highest
    radius: 12
    padding: 12

    background: Rectangle {
        radius: box.radius
        color: box.MD.MProp.backgroundColor
        border.width: 1
        border.bottom: 2
        border.color: box.MD.MProp.color.outline_variant
    }

    Layout.fillWidth: true
    Layout.maximumWidth: maxWidth
    Layout.alignment: Qt.AlignHCenter
    Layout.preferredWidth: maxWidth

    Layout.preferredHeight: {
        const w = Math.min(width > 0 ? width : maxWidth, maxWidth)
        const contentW = Math.max(0, w - leftPadding - rightPadding)
        return contentW / aspect + topPadding + bottomPadding
    }

    implicitWidth: maxWidth
    implicitHeight: (maxWidth - leftPadding - rightPadding) / aspect
        + topPadding + bottomPadding

    contentItem: Item {
        clip: true

        Rectangle {
            anchors.fill: parent
            visible: img.status !== Image.Ready
            color: "transparent"

            MD.Label {
                anchors.centerIn: parent
                text: img.status === Image.Loading ? "Loading…" : "No image"
                opacity: 0.5
            }
        }

        Image {
            id: img
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            cache: false
            asynchronous: true
            mipmap: true
            smooth: paintedWidth < sourceSize.width
        }
        MD.IconButton{
            anchors.right: img.right
            anchors.bottom: img.bottom
            anchors.margins: 4

            icon.name: MD.Token.icon.expand_content
            mdState.type: MD.Enum.IBtFilled
        }
    }
}