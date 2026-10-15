import QtQuick 2.0
import QtQuick.Window
import QtQuick.Layouts
// import QtQuick.Controls

import Qcm.Material as MD
import Wave52

MD.Pane {
    id: root
    showBackground: true
    spacing: 8

    signal openRequested()
    signal saveBinRequested()
    signal savePngRequested()

    contentItem: RowLayout{
        MD.Button {
            text: "Open..."
            onClicked: root.openRequested()
        }
        Loader{
            active: isDebugBuild
            sourceComponent: MD.Button {
                text: "Save bin"
                onClicked: root.saveBinRequested()
            }
        }

        Loader{
            active: isDebugBuild
            sourceComponent: MD.Button {
                text: "Save png"
                onClicked: root.savePngRequested()
            }
        }
        Item{
            Layout.fillWidth: true
        }
    }


}