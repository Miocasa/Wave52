import QtQuick 2.0
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

import Qcm.Material as MD
import Wave52

MD.Page {
    id: root

    showBackground: false
    padding: BreakPoint.isCompact ? 0 : 12

    contentItem: MD.SplitView
    {
        MD.Pane {
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            radius: root.MD.MProp.page.backgroundRadius
            padding: 12
            showBackground: true

            contentItem: Item {
                // anchors.fill: parent
                ColumnLayout {
                    id: col
                    spacing: 8

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 0

                    width: Math.min(parent.width, 360)

                    MD.Text {
                        text: "w: " + col.width + "  parent: " + col.parent.width
                    }

                    MD.Button {
                        text: "Scan"
                        Layout.fillWidth: true
                        enabled: !app.busy && !app.connected
                        onClicked: app.scan()
                    }
                    MD.ComboBox {
                        id: deviceCombo
                        Layout.fillWidth: true
                        model: app.deviceNames
                        enabled: !app.busy && !app.connected
                    }
                    Connections {
                        target: app

                        function onDevicesChanged() {
                            Qt.callLater(() => deviceCombo.currentIndex =
                                    app.preferredDevice >= 0 ? app.preferredDevice : 0)
                        }
                    }
                    MD.Button {
                        text: app.connected ? "Disconnect" : "Connect"
                        Layout.fillWidth: true
                        enabled: !app.busy && (app.connected || deviceCombo.count > 0)
                        onClicked: app.toggleConnection(deviceCombo.currentIndex)
                    }
                    MD.Label {
                        text: app.connected ? "Connected" : "Not connected"
                    }

                    MD.Divider {
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150

                        MaterialTextArea {
                            readOnly: true
                            wrapMode: TextArea.Wrap
                            text: app.logText
                            onTextChanged: cursorPosition = length
                        }
                    }
                }
            }
        }
    }
}