pragma ComponentBehavior: Bound
import QtQuick
import QtQml as Qml
import QtQuick.Layouts
import Qcm.Material as MD

Item {
    id: root

    property alias text: m_input.text
    property string placeholderText
    property int debounceMs: 300 // 1000 for server response
    signal textEdited

    function clear() {
        m_input.clear();
        m_debounce.stop();
        root.textEdited();
    }

    implicitHeight: 32
    implicitWidth: 200

    Qml.Timer {
        id: m_debounce
        interval: root.debounceMs
        repeat: false
        onTriggered: root.textEdited()
    }

    Rectangle {
        id: m_bg
        anchors.fill: parent
        radius: 8
        color: "transparent"
        border.width: 1
        border.color: MD.MProp.color.outline_variant

        MD.Ripple {
            anchors.fill: parent
            radius: parent.radius
            pressed: m_mouse.pressed
            stateOpacity: m_mouse.containsMouse ? MD.Token.state.hover.state_layer_opacity : 0
            color: MD.MProp.color.on_surface_variant
        }

        MD.FocusIndicator {
            corners: MD.Util.corners(parent.radius)
            active: m_input.activeFocus
        }

        // Clicking anywhere on the chip focuses the input field
        MouseArea {
            id: m_mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.IBeamCursor
            onClicked: m_input.forceActiveFocus()
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: m_clear.visible ? 4 : 8
            spacing: 6

            MD.Icon {
                name: MD.Token.icon.search
                size: 16
                color: MD.MProp.color.on_surface_variant
                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredHeight: m_input.implicitHeight

                MD.Label {
                    id: m_placeholder
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    typescale: MD.Token.typescale.label_large
                    color: MD.MProp.color.on_surface_variant
                    text: root.placeholderText
                    visible: m_input.text.length === 0 && !m_input.activeFocus
                    elide: Text.ElideRight
                    font.capitalization: Font.MixedCase // revert MD.Page recursive Capitalize param
                }

                TextInput {
                    id: m_input
                    anchors.fill: parent
                    verticalAlignment: TextInput.AlignVCenter
                    color: MD.MProp.color.on_surface
                    selectionColor: MD.MProp.color.primary
                    selectedTextColor: MD.MProp.color.on_primary
                    selectByMouse: true
                    clip: true
                    font: m_placeholder.font
                    onTextEdited: m_debounce.restart()
                    Keys.onEscapePressed: event => {
                        focus = false;
                        event.accepted = true;
                    }
                }
            }

            MD.IconButton {
                id: m_clear
                visible: m_input.text.length > 0
                icon.name: MD.Token.icon.close
                icon.width: 14
                icon.height: 14
                padding: 2
                topInset: 0
                bottomInset: 0
                leftInset: 0
                rightInset: 0
                background.implicitWidth: 22
                background.implicitHeight: 22
                Layout.alignment: Qt.AlignVCenter
                onClicked: root.clear()
            }
        }
    }
}