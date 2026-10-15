import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qcm.Material as MD

ColumnLayout {
    id: root
    spacing: 8

    property int row: -1
    property int bytes: 0
    property bool loading: false
    property var cur: ({
        text: "", x: 0, y: 0, w: 0, h: 0,
        fontSize: 1, fontId: 0, valign: 0, halign: 0,
        color: 0, bgColor: 0xFFFF
    })

    readonly property var colors: [
        {text: "Black", value: 0x0000},
        {text: "White", value: 0xFFFF},
        {text: "Dark grey", value: 0x7BEF},
        {text: "Light grey", value: 0xC618}
    ]

    function colorIndex(v) {
        for (let i = 0; i < colors.length; ++i)
            if (colors[i].value === v)
                return i
        return 0
    }

    function reload() {
        loading = true
        if (row >= 0 && row < app.widgets.count) {
            cur = app.widgets.get(row)
            textEdit.text = cur.text
            bytes = app.utf8Size(cur.text)
        }
        loading = false
    }

    function set(name, value) {
        if (row >= 0)
            app.widgets.setField(row, name, value)
    }

    onRowChanged: reload()

    Connections {
        target: app.widgets

        function onCountChanged() {
            Qt.callLater(root.reload)
        }

        function onSpecsChanged() {
            // update on changes
            if (!root.loading)
                Qt.callLater(root.reload)
        }
    }

    MD.Label {
        text: "Edit widget #" + (root.row + 1)
        typescale: MD.Token.typescale.title_small
    }

    MD.TextField {
        id: textEdit
        Layout.fillWidth: true
        placeholderText: "Text"
        onTextChanged: {
            if (root.loading)
                return
            root.bytes = app.utf8Size(text)
            root.set("text", text)
        }
    }

    MD.Label {
        text: root.bytes + " / " + app.maxTextBytes + " bytes"
        color: root.bytes > app.maxTextBytes
            ? MD.Token.color.error
            : MD.Token.color.on_surface_variant
        typescale: MD.Token.typescale.body_small
    }

    GridLayout {
        Layout.fillWidth: true
        columns: 2
        columnSpacing: 12
        rowSpacing: 8

        MD.Label {
            text: "Position X, Y"
        }
        RowLayout {
            SpinBox {
                Layout.fillWidth: true
                editable: true
                from: -2000;
                to: 2000
                value: root.cur.x
                onValueModified: root.set("x", value)
            }
            SpinBox {
                Layout.fillWidth: true
                editable: true
                from: -2000;
                to: 2000
                value: root.cur.y
                onValueModified: root.set("y", value)
            }
        }

        MD.Label {
            text: "Size W, H"
        }
        RowLayout {
            SpinBox {
                Layout.fillWidth: true
                editable: true
                from: 0;
                to: 2000
                value: root.cur.w
                onValueModified: root.set("w", value)
            }
            SpinBox {
                Layout.fillWidth: true
                editable: true
                from: 0;
                to: 2000
                value: root.cur.h
                onValueModified: root.set("h", value)
            }
        }

        MD.Label {
            text: "Font, size"
        }
        RowLayout {
            ComboBox {
                Layout.fillWidth: true
                model: app.fontNames
                currentIndex: root.cur.fontId
                onActivated: root.set("fontId", currentIndex)
            }
            SpinBox {
                editable: true
                from: 1;
                to: 10
                value: root.cur.fontSize
                onValueModified: root.set("fontSize", value)
            }
        }

        MD.Label {
            text: "V align, H align"
        }
        RowLayout {
            ComboBox {
                Layout.fillWidth: true
                model: ["Top", "Center", "Bottom"]
                currentIndex: root.cur.valign
                onActivated: root.set("valign", currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: ["Left", "Center", "Right"]
                currentIndex: root.cur.halign
                onActivated: root.set("halign", currentIndex)
            }
        }

        MD.Label {
            text: "Color, background"
        }
        RowLayout {
            ComboBox {
                Layout.fillWidth: true
                model: root.colors
                textRole: "text"
                currentIndex: root.colorIndex(root.cur.color)
                onActivated: root.set("color", root.colors[currentIndex].value)
            }
            ComboBox {
                Layout.fillWidth: true
                model: root.colors
                textRole: "text"
                currentIndex: root.colorIndex(root.cur.bgColor)
                onActivated: root.set("bgColor", root.colors[currentIndex].value)
            }
        }
    }
}