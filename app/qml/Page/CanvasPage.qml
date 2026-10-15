import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls

import Qcm.Material as MD
import Wave52

MD.Page {
    id: root

    //  DropArea {
    //     anchors.fill: parent
    //     onDropped: (drop) => {
    //         if (drop.hasUrls && drop.urls.length > 0)
    //             app.loadImage(drop.urls[0])
    //     }
    // }

    showBackground: false
    padding: BreakPoint.isCompact ? 0 : 12

    contentItem: MD.SplitView {
        MD.Pane {
            id: content_box_
            radius: root.MD.MProp.page.backgroundRadius
            padding: 12
            showBackground: true

            contentItem: ScrollView {
                id: scroll
                contentWidth: availableWidth
                clip: true

                ColumnLayout {
                    width: Math.min(scroll.availableWidth, 400)
                    height: Math.max(implicitHeight, scroll.availableHeight)
                    spacing: 16

                    Preview {
                        source: "image://preview/screen?" + app.screenRevision
                        landscape: true
                        maxWidth: BreakPoint.isCompact ? Number.POSITIVE_INFINITY : 400
                    }

                    ControlPane {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                        Layout.maximumWidth: 480

                        onOpenRequested: openDialog.open()
                        onSaveBinRequested: saveBinDialog.open()
                        onSavePngRequested: savePngDialog.open()
                    }

                    ExpandableList {
                        id: widgetList
                        Layout.fillWidth: true

                        model: app.widgets
                        textRole: "text"
                        currentIndex: -1
                        reorderable: false        // TODO model.move to reorder

                        rightContent: [
                            MD.SplitButton {
                                id: addWidgetBtn
                                text: "Widget"
                                icon.name: MD.Token.icon.add

                                onClicked: {
                                    const idx = app.widgets.add(widgetList.currentIndex)
                                    widgetList.currentIndex = idx
                                    widgetList.expanded = true
                                }

                                menu: MD.Menu {
                                    MD.MenuItem {
                                        text: "Textbox"
                                        icon.name: MD.Token.icon.text_fields
                                        onTriggered: {
                                            const idx = app.widgets.add(widgetList.currentIndex)
                                            widgetList.currentIndex = idx
                                            widgetList.expanded = true
                                        }
                                    }
                                    MD.MenuItem {
                                        text: "VarBitmap"
                                        icon.name: MD.Token.icon.image
                                        enabled: false
                                    }
                                    MD.MenuItem {
                                        text: "Bitmap"
                                        icon.name: MD.Token.icon.photo
                                        enabled: false
                                    }
                                }
                            },

                            MD.IconButton {
                                icon.name: MD.Token.icon.delete
                                enabled: widgetList.currentIndex >= 0
                                onClicked: {
                                    const r = widgetList.currentIndex
                                    app.widgets.remove(r)
                                    widgetList.currentIndex = app.widgets.count === 0
                                        ? -1
                                        : Math.min(r, app.widgets.count - 1)
                                }
                            }
                        ]
                    }
                    BGPropsEditor{}
                    DeviceCmd{}

                    WidgetEditor {
                        Layout.fillWidth: true
                        Layout.maximumWidth: 520
                        // visible: widgetList.currentIndex >= 0
                        row: widgetList.currentIndex
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }
    }

    MD.FileDialog {
        id: openDialog
        title: "Open image"
        nameFilters: ["Images (*.png *.jpg *.jpeg *.bmp *.webp *.gif *.tif *.tiff)", "All files (*)"]
        onAccepted: app.loadImage(selectedFile)
    }
    MD.FileDialog {
        id: saveBinDialog
        title: "Save bitmap"
        fileMode: FileDialog.SaveFile
        defaultSuffix: "bin"
        nameFilters: ["Binary (*.bin)"]
        onAccepted: app.saveBin(selectedFile)
    }
    MD.FileDialog {
        id: savePngDialog
        title: "Save preview"
        fileMode: FileDialog.SaveFile
        defaultSuffix: "png"
        nameFilters: ["PNG (*.png)"]
        onAccepted: app.savePng(selectedFile)
    }
}