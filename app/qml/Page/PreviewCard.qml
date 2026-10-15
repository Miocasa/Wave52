pragma
ComponentBehavior: Bound
import QtQuick
import Qcm.Material as MD

Item {
    id: root

    required property var model
    required property int index
    property bool selected: false
    property bool hidden: false
    property url source: ""
    property string title: ""
    property real itemWidth: width
    property real itemHeight: height

    width: GridView.view ? GridView.view.cellWidth : 0
    height: GridView.view ? GridView.view.cellHeight : 0

    focusPolicy: Qt.StrongFocus

    signal clicked(int modifiers)

    signal selectionRequested(int modifiers)

    readonly property int baseRadius_: MD.Token.shape.corner.extra_small
    readonly property int selectedRadius_: MD.Token.shape.corner.large
    readonly property int radius_: root.selected ? root.selectedRadius_ : root.baseRadius_
    readonly property real selectedInset_: root.selectedRadius_ / 2
    readonly property real cardWidth: Math.min(root.itemWidth, root.width)
    readonly property real cardHeight: Math.min(root.itemHeight, root.height)

    // background
    Rectangle {
        anchors.fill: parent
        visible: root.selected
        color: MD.Token.color.primary_container
    }

    // card
    Item {
        id: card
        width: root.cardWidth
        height: root.cardHeight
        anchors.centerIn: parent

        Item {
            id: cell
            anchors.fill: parent
            anchors.margins: 6 + (root.selected ? root.selectedInset_ : 0)

            Behavior on anchors.margins {
                NumberAnimation {
                    duration: 150; easing.type: Easing.OutCubic
                }
            }

            MD.Image {
                id: thumb
                anchors.fill: parent
                source: root.source
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                radius: root.radius_
            }

            // Gradient under header for visibility
            Rectangle {
                anchors.left: thumb.left
                anchors.right: thumb.right
                anchors.bottom: thumb.bottom
                height: Math.max(0, thumb.height - title.y)
                visible: height > 0
                radius: root.radius_
                gradient: Gradient {
                    GradientStop {
                        position: 0.0; color: "transparent"
                    }
                    GradientStop {
                        position: 1.0; color: Qt.rgba(0, 0, 0, 0.6)
                    }
                }
            }

            MD.Text {
                id: title
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
                text: root.title.length > 0 ? root.title : qsTr("Untitled")
                typescale: MD.Token.typescale.title_small
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                maximumLineCount: 2
                leftPadding: 8
                rightPadding: 8
            }

            MouseArea {
                property bool selectionRequestedByHold: false

                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                cursorShape: Qt.PointingHandCursor
                onPressed: selectionRequestedByHold = false
                onCanceled: selectionRequestedByHold = false
                onPressAndHold: mouse => {
                    if (mouse.button !== Qt.LeftButton)
                        return;
                    selectionRequestedByHold = true;
                    root.selectionRequested(mouse.modifiers);
                }
                onClicked: mouse => {
                    if (selectionRequestedByHold) {
                        selectionRequestedByHold = false;
                        return;
                    }
                    if (mouse.button === Qt.RightButton) {
                        root.selectionRequested(mouse.modifiers);
                        return;
                    }
                    root.clicked(mouse.modifiers);
                }
            }
        }
    }

    // selection check mark
    Rectangle {
        anchors.top: card.top
        anchors.left: card.left
        anchors.margins: 8
        width: 32
        height: 32
        radius: width / 2
        visible: root.selected
        color: MD.Token.color.primary
        border.color: MD.Token.color.primary_container
        border.width: 3

        MD.Icon {
            anchors.centerIn: parent
            name: MD.Token.icon.check
            size: 20
            color: MD.Token.color.on_primary
        }
    }

    // «Hidden» icon
    Rectangle {
        anchors.top: card.top
        anchors.right: card.right
        anchors.margins: 8
        width: 32
        height: 32
        radius: width / 2
        visible: root.hidden
        color: MD.Token.color.secondary_container
        border.color: MD.Token.color.secondary
        border.width: 3

        MD.Icon {
            anchors.centerIn: parent
            name: MD.Token.icon.visibility_off
            size: 20
            color: MD.Token.color.on_secondary_container
        }
    }
}