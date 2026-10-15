import QtQuick 2.0
import QtQuick.Window
import QtQuick.Layouts

import Qcm.Material as MD
import Wave52

MD.Page {
    id: root
    property bool loading: true
    showBackground: false
    padding: BreakPoint.isCompact ? 0 : 12

    contentItem: MD.SplitView {
        MD.Pane {
            radius: root.MD.MProp.page.backgroundRadius
            padding: 12
            showBackground: true
            contentItem: ColumnLayout{
                Loader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    active: root.loading
                    sourceComponent: MD.BusyIndicator {
                        anchors.centerIn: parent
                        running: true
                    }
                }
                Loader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    active: !root.loading
                    sourceComponent: MD.Text{
                        anchors.centerIn: parent
                        text: "text"
                    }
                }


            }
        }
    }
}
