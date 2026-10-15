import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 6

    // compact: панель лежит внутри общей прокручиваемой страницы (портрет),
    // поэтому высоты задаются явно, а не растягиваются
    property bool compact: false

    component PreviewBox: GroupBox {
        property alias source: img.source
        Layout.fillWidth: true
        Layout.fillHeight: !root.compact
        Layout.preferredWidth: 1
        Layout.preferredHeight: root.compact ? Math.max(220, root.width * 0.75) : 1

        Image {
            id: img
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            cache: false
            asynchronous: false
            mipmap: true
            smooth: paintedWidth < sourceSize.width
        }
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.fillHeight: !root.compact
        columns: (!root.compact && root.width >= 700) ? 2 : 1
        columnSpacing: 8
        rowSpacing: 8

        PreviewBox {
            title: "Original"
            source: app.hasImage ? "image://preview/original?" + app.originalRevision : ""
        }
        PreviewBox {
            title: "Screen preview (4 gray levels, rendered like the device)"
            source: "image://preview/screen?" + app.screenRevision
        }
    }

    Label {
        Layout.fillWidth: true
        text: app.infoText
        wrapMode: Text.Wrap
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.preferredHeight: 150

        TextArea {
            readOnly: true
            wrapMode: TextArea.Wrap
            font.family: "monospace"
            text: app.logText
            onTextChanged: cursorPosition = length
        }
    }
}
