import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Section {
    id: root
    title: "Text widgets"

    readonly property int row: list.currentIndex
    property int bytes: 0
    property bool loading: false
    property var cur: ({
        text: "", x: 0, y: 0, w: 0, h: 0, fontSize: 1, fontId: 0,
        valign: 0, halign: 0, color: 0, bgColor: 0xFFFF
    })

    readonly property var colors: [
        { text: "Black",      value: 0x0000 },
        { text: "White",      value: 0xFFFF },
        { text: "Dark grey",  value: 0x7BEF },
        { text: "Light grey", value: 0xC618 }
    ]

    function colorIndex(v) {
        for (let i = 0; i < colors.length; ++i)
            if (colors[i].value === v) return i
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
        function onCountChanged() { Qt.callLater(root.reload) }
    }

    // ---- список ----
    Frame {
        Layout.fillWidth: true
        Layout.preferredHeight: 110
        padding: 1

        ListView {
            id: list
            anchors.fill: parent
            clip: true
            model: app.widgets
            currentIndex: -1
            ScrollBar.vertical: ScrollBar {}

            delegate: ItemDelegate {
                width: ListView.view.width
                text: model.display
                highlighted: ListView.isCurrentItem
                onClicked: list.currentIndex = index
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Button {
            text: "Add"
            Layout.fillWidth: true
            onClicked: list.currentIndex = app.widgets.add(list.currentIndex)
        }
        Button {
            text: "Remove"
            Layout.fillWidth: true
            enabled: root.row >= 0
            onClicked: {
                const r = list.currentIndex
                app.widgets.remove(r)
                list.currentIndex = Math.min(r, app.widgets.count - 1)
            }
        }
        Button {
            text: "Clear all"
            Layout.fillWidth: true
            enabled: app.widgets.count > 0
            onClicked: {
                app.widgets.clear()
                list.currentIndex = -1
            }
        }
    }

    // ---- форма выбранного виджета ----
    ColumnLayout {
        Layout.fillWidth: true
        enabled: root.row >= 0
        spacing: 6

        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            TextArea {
                id: textEdit
                placeholderText: "Text"
                wrapMode: TextArea.Wrap
                onTextChanged: {
                    if (root.loading) return
                    root.bytes = app.utf8Size(text)
                    root.set("text", text) // при превышении лимита модель значение не примет
                }
            }
        }
        Label {
            text: root.bytes + " / " + app.maxTextBytes + " bytes"
            color: root.bytes > app.maxTextBytes ? "#d33" : palette.windowText
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 8
            rowSpacing: 4

            Label { text: "Position X, Y" }
            RowLayout {
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: -2000; to: 2000
                    value: root.cur.x
                    onValueModified: root.set("x", value)
                }
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: -2000; to: 2000
                    value: root.cur.y
                    onValueModified: root.set("y", value)
                }
            }

            Label { text: "Size W, H" }
            RowLayout {
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: 0; to: 2000
                    value: root.cur.w
                    onValueModified: root.set("w", value)
                }
                SpinBox {
                    Layout.fillWidth: true; editable: true; from: 0; to: 2000
                    value: root.cur.h
                    onValueModified: root.set("h", value)
                }
            }

            Label { text: "Font, size" }
            RowLayout {
                ComboBox {
                    Layout.fillWidth: true
                    model: app.fontNames
                    currentIndex: root.cur.fontId
                    onActivated: root.set("fontId", currentIndex)
                }
                SpinBox {
                    editable: true; from: 1; to: 10
                    value: root.cur.fontSize
                    onValueModified: root.set("fontSize", value)
                }
            }

            Label { text: "V align, H align" }
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

            Label { text: "Color, background" }
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
}
