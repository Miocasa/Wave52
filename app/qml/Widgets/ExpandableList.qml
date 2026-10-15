import QtQuick
import QtQuick.Layouts

import Qcm.Material as MD

Item {
    id: root

    property var model
    property string textRole: "text"
    property int currentIndex: 0
    property bool expanded: false
    property bool reorderable: true
    property int rowHeight: 56
    property int headerHeight: 48
    property int cornerRadius: 24
    property int revision: 0

    property color containerColor: MD.Token.color.surface_container_high
    property color contentColor: MD.Token.color.on_surface

    property alias rightContent: rightSlot.data
    readonly property bool hasRight: rightSlot.children.length > 0

    readonly property int count: !model ? 0 : (model.count !== undefined ? model.count : (model.length !== undefined ? model.length : 0))

    signal activated(int index)

    signal moved(int from, int to)

    property int dragIndex: -1
    property int overIndex: -1
    property real dragY: 0

    property real reveal: expanded ? 1 : 0
    Behavior on reveal {
        NumberAnimation {
            duration: 300
            easing.type: Easing.BezierSpline
            // MD3 emphasized: cubic-bezier(0.2, 0, 0, 1)
            easing.bezierCurve: [0.2, 0.0, 0.0, 1.0, 1.0, 1.0]
        }
    }

    implicitWidth: 280
    implicitHeight: headerRow.implicitHeight + bodyClip.height

    function textAt(i) {
        if (!model || i < 0 || i >= count)
            return "";
        const it = model.get ? model.get(i) : model[i];
        if (it === undefined || it === null)
            return "";
        return typeof it === "object" ? ((1 + i) + ": " + it[textRole] ?? "") : String(it);
    }

    function finishDrag() {
        if (dragIndex < 0)
            return;
        const from = dragIndex;
        const to = overIndex;
        if (to >= 0 && from !== to) {
            if (model && typeof model.move === "function")
                model.move(from, to, 1);
            if (currentIndex === from)
                currentIndex = to;
            else if (from < currentIndex && currentIndex <= to)
                currentIndex--;
            else if (to <= currentIndex && currentIndex < from)
                currentIndex++;
            revision++;
            moved(from, to);
        }
        dragIndex = -1;
        overIndex = -1;
    }

    // header left chip and right content
    RowLayout {
        id: headerRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 12

        Rectangle {
            id: chip
            Layout.fillWidth: !root.hasRight
            Layout.minimumWidth: 80
            Layout.preferredWidth: chipRow.implicitWidth + 34
            Layout.preferredHeight: root.headerHeight

            color: root.containerColor
            topLeftRadius: height / 2
            topRightRadius: height / 2
            bottomLeftRadius: height / 2 * (1 - root.reveal)
            bottomRightRadius: height / 2 * (1 - root.reveal)

            activeFocusOnTab: true
            Keys.onSpacePressed: root.expanded = !root.expanded
            Keys.onReturnPressed: root.expanded = !root.expanded
            border.width: activeFocus ? 2 : 0
            border.color: MD.Token.color.primary

            RowLayout {
                id: chipRow
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 14
                spacing: 8

                MD.Label {
                    Layout.fillWidth: true
                    typescale: MD.Token.typescale.label_large
                    color: root.contentColor
                    wrapMode: Text.NoWrap
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    text: {
                        root.revision;
                        root.count;
                        return root.textAt(root.currentIndex);
                    }
                }

                MD.Label {
                    text: "▾"
                    color: root.contentColor
                    rotation: root.reveal * 180
                }
            }

            // state layer
            Rectangle {
                anchors.fill: parent
                topLeftRadius: chip.topLeftRadius
                topRightRadius: chip.topRightRadius
                bottomLeftRadius: chip.bottomLeftRadius
                bottomRightRadius: chip.bottomRightRadius
                color: Qt.alpha(MD.Token.color.on_surface, chipArea.pressed ? 0.10 : (chipArea.containsMouse ? 0.08 : 0))
            }

            MouseArea {
                id: chipArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    chip.forceActiveFocus();
                    root.expanded = !root.expanded;
                }
            }
        }

        Item { // space
            Layout.fillWidth: true
            visible: root.hasRight
        }

        RowLayout {
            id: rightSlot
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            visible: root.hasRight
            spacing: 8
        }
    }

    Item {
        id: bodyClip
        y: headerRow.implicitHeight
        width: parent.width
        height: (list.height + 8) * root.reveal
        clip: true
        visible: root.reveal > 0.001

        Rectangle {
            id: body
            width: parent.width
            height: list.height + 8
            color: root.containerColor

            topLeftRadius: 0
            topRightRadius: chip.width >= root.width - 1 ? 0 : root.cornerRadius
            bottomLeftRadius: root.cornerRadius
            bottomRightRadius: root.cornerRadius
        }

        Item {
            id: list
            y: 4
            width: parent.width
            height: root.count * root.rowHeight

            Repeater {
                model: root.model

                delegate: Item {
                    id: del
                    required property int index
                    required property var model

                    readonly property bool dragging: root.dragIndex === index
                    readonly property int slot: {
                        if (root.dragIndex < 0 || dragging)
                            return index;
                        if (root.dragIndex < index && index <= root.overIndex)
                            return index - 1;
                        if (root.overIndex <= index && index < root.dragIndex)
                            return index + 1;
                        return index;
                    }

                    width: list.width
                    height: root.rowHeight
                    y: dragging ? root.dragY : slot * root.rowHeight
                    z: dragging ? 2 : 1

                    Behavior on y {
                        enabled: !del.dragging
                        NumberAnimation {
                            duration: 150
                            easing.type: Easing.OutCubic
                        }
                    }

                    // background
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 4
                        radius: 12
                        color: del.dragging ? MD.Token.color.surface_container_highest
                            : (del.index === root.currentIndex
                                ? Qt.alpha(MD.Token.color.primary, 0.14)
                                : Qt.alpha(MD.Token.color.on_surface, rowArea.pressed ? 0.10 : (rowArea.containsMouse ? 0.08 : 0)))
                    }

                    // Divider
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        height: 1
                        color: Qt.alpha(MD.Token.color.on_surface, 0.12)
                        visible: del.index < root.count - 1 && !del.dragging
                    }

                    // Select
                    MouseArea {
                        id: rowArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            root.currentIndex = del.index;
                            root.activated(del.index);
                        }
                    }

                    // drag icon
                    Item {
                        id: handle
                        visible: root.reorderable
                        width: 48
                        height: parent.height

                        Grid {
                            anchors.centerIn: parent
                            columns: 2
                            spacing: 3
                            Repeater {
                                model: 6
                                Rectangle {
                                    width: 4
                                    height: 4
                                    radius: 2
                                    color: MD.Token.color.primary
                                }
                            }
                        }

                        MouseArea {
                            id: dragArea
                            anchors.fill: parent
                            preventStealing: true
                            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                            property real grab: 0

                            onPressed: m => {
                                grab = mapToItem(list, m.x, m.y).y - del.y;
                                root.dragY = del.y;
                                root.overIndex = del.index;
                                root.dragIndex = del.index;
                            }
                            onPositionChanged: m => {
                                if (!pressed || root.dragIndex !== del.index)
                                    return;
                                const py = mapToItem(list, m.x, m.y).y;
                                const maxY = (root.count - 1) * root.rowHeight;
                                root.dragY = Math.max(0, Math.min(maxY, py - grab));
                                root.overIndex = Math.max(0, Math.min(root.count - 1, Math.round(root.dragY / root.rowHeight)));
                            }
                            onReleased: root.finishDrag()
                            onCanceled: root.finishDrag()
                        }
                    }

                    // Text
                    MD.Label {
                        anchors.left: root.reorderable ? handle.right : parent.left
                        anchors.leftMargin: root.reorderable ? 4 : 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        typescale: MD.Token.typescale.body_large
                        elide: Text.ElideRight
                        text: {
                            const v = del.model[root.textRole];
                            return (1 + del.index) + ": " + (v !== undefined ? v : String(del.model.modelData ?? ""));
                        }
                    }
                }
            }
        }
    }
}
