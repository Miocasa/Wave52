import QtQuick
import QtQuick.Controls.Basic as T
import Qcm.Material as MD

T.TextArea {
    id: root
    property MD.typescale typescale: MD.Token.typescale.body_large
    property bool outlined: true            // outlined, filled
    property bool error: false
    property string labelText: ""
    property string supportingText: ""
    property bool showBackground: true
    property real radius: 4

    property color containerColor: MD.Token.color.surface_container_highest
    property color activeColor: error ? MD.Token.color.error : MD.Token.color.primary
    property color outlineColor: error ? MD.Token.color.error : MD.Token.color.outline
    property color labelColor: root.activeFocus ? root.activeColor
                                                : (error ? MD.Token.color.error : MD.Token.color.on_surface_variant)
    property color supportingColor: error ? MD.Token.color.error : MD.Token.color.on_surface_variant

    readonly property bool labelFloating: root.activeFocus || root.length > 0 || root.preeditText.length > 0
    readonly property bool hasLabel: labelText.length > 0
    readonly property bool hasSupporting: supportingText.length > 0

    // Text
    font.pixelSize: root.typescale.size
    font.weight: root.typescale.weight
    font.letterSpacing: root.typescale.tracking
    color: enabled ? MD.Token.color.on_surface : Qt.alpha(MD.Token.color.on_surface, 0.38)
    selectionColor: Qt.alpha(root.activeColor, 0.4)
    selectedTextColor: MD.Token.color.on_surface
    placeholderTextColor: MD.Token.color.on_surface_variant
    wrapMode: TextEdit.Wrap
    selectByMouse: true
    verticalAlignment: TextEdit.AlignTop
    cursorDelegate: Rectangle {
        width: 2
        color: root.activeColor
        visible: root.activeFocus && !root.readOnly
    }

    // Offsets
    leftPadding: 16
    rightPadding: 16
    topPadding: hasLabel ? 28 : 16
    bottomPadding: 16 + (hasSupporting ? 20 : 0)

    background: Item {
        implicitWidth: 200
        implicitHeight: 56

        // Filled
        Rectangle {
            visible: root.showBackground && !root.outlined
            anchors.fill: parent
            color: root.containerColor
            topLeftRadius: root.radius
            topRightRadius: root.radius
            bottomLeftRadius: 0
            bottomRightRadius: 0
            opacity: root.enabled ? 1 : 0.38

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: root.activeFocus ? 2 : 1
                color: root.activeFocus ? root.activeColor : root.outlineColor
            }
        }

        // Outlined
        Rectangle {
            visible: root.showBackground && root.outlined
            anchors.fill: parent
            anchors.bottomMargin: root.hasSupporting ? 20 : 0
            color: "transparent"
            radius: root.radius
            border.width: root.activeFocus ? 2 : 1
            border.color: root.activeFocus ? root.activeColor : root.outlineColor
            opacity: root.enabled ? 1 : 0.38
        }

        // Label
        Text {
            id: label
            visible: root.hasLabel
            text: root.labelText
            color: root.labelColor
            antialiasing: true
            elide: Text.ElideRight
            width: parent.width - 32
            x: 16
            font.pixelSize: root.labelFloating ? MD.Token.typescale.body_small.size
                                               : root.typescale.size
            font.weight: root.typescale.weight
            y: root.labelFloating ? (root.outlined ? -font.pixelSize / 2 : 8) : 16

            Rectangle {
                z: -1
                visible: root.outlined && root.labelFloating
                anchors.fill: parent
                anchors.leftMargin: -4
                anchors.rightMargin: parent.width - parent.contentWidth - 4
                color: root.palette.window
            }

            Behavior on y { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }
            Behavior on font.pixelSize { NumberAnimation { duration: 150 } }
        }

        // Supporting text
        Text {
            visible: root.hasSupporting
            text: root.supportingText
            color: root.supportingColor
            antialiasing: true
            elide: Text.ElideRight
            x: 16
            width: parent.width - 32
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            font.pixelSize: MD.Token.typescale.body_small.size
            font.weight: MD.Token.typescale.body_small.weight
        }
    }
}
