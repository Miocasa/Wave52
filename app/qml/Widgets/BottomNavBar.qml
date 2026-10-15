import QtQuick
import QtQuick.Layouts
import Qcm.Material as MD

MD.Pane {
    id: root

    property var model: []
    property int currentIndex: 0
    signal clicked(int index)

    padding: 0
    backgroundColor: MD.MProp.color.surface_container
    elevation: MD.Token.elevation.level2

    contentItem: RowLayout {
        spacing: 0

        Repeater {
            model: root.model

            delegate: Item {
                id: cell
                required property var modelData
                required property int index

                Layout.fillWidth: true
                implicitHeight: 12 + bar_item.implicitHeight + 16

                MD.BarItem {
                    id: bar_item
                    anchors.fill: parent
                    anchors.topMargin: 12
                    anchors.bottomMargin: 16
                    icon.name: cell.modelData.icon
                    text: cell.modelData.name
                    // width: Math.min(parent.width, 80)
                    checked: root.currentIndex === cell.index
                    onClicked: root.clicked(cell.index)
                }
            }
        }
    }
}