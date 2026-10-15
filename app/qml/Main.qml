import QtQuick 2.0
import QtQuick.Window
import QtQuick.Layouts

import Qcm.Material as MD
import Wave52

MD.ApplicationWindow {
    id: root
    width: 900
    height: 600
    visible: true

    // colors setup
    MD.MProp.textColor: MD.MProp.color.on_surface
    MD.MProp.backgroundColor: MD.MProp.color.surface_container
    property int windowClass: MD.Token.window_class.select_type(width)
    MD.MProp.size.windowClass: windowClass

    onWidthChanged: {
        windowClass = MD.Token.window_class.select_type(width);
    }

    // -------------------------------------------------------
    property int pageIndex: 0

    readonly property var pageModel: [
        {
            name: 'Home',
            source: 'qrc:/qt/qml/Wave52/qml/Page/HomePage.qml',
            icon: MD.Token.icon.home,
            cacheable: true
        },
        {
            name: 'Connect',
            source: 'qrc:/qt/qml/Wave52/qml/Page/ConnectPage.qml',
            icon: MD.Token.icon.bluetooth, // bluetooth_connected // sensors
            cacheable: true
        },
        {
            name: 'Canvas',
            source: 'qrc:/qt/qml/Wave52/qml/Page/CanvasPage.qml',
            icon: MD.Token.icon.art_track,
            cacheable: true
        },
        {
            name: 'Settings',
            source: 'qrc:/qt/qml/Wave52/qml/Page/SettingsPage.qml',
            icon: MD.Token.icon.settings,
            cacheable: true
        },
    ]

    onPageIndexChanged: {
        m_content.switchTo(pageModel[pageIndex].source, {}, pageModel[pageIndex].cacheable);
    }

    Component.onCompleted: {
        pageIndexChanged();
        MD.Token.color.useSysColorSM = true;
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Loader { // NavigationRail
                id: m_drawer_loader
                Layout.fillHeight: true
                active: !BreakPoint.isCompact
                visible: active

                sourceComponent: MD.NavigationRail {
                    id: m_rail
                    model: root.pageModel
                    currentIndex: root.pageIndex

                    fabAction: MD.Action {
                        icon.name: MD.Token.icon.add
                        text: 'Create'
                    }

                    onClicked: function (model) {
                        root.pageIndex = model.index;
                    }
                }
            }
            MD.PageContainer { // body content
                id: m_content
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                initialItem: Item {}

                MD.MProp.page: m_page_ctx_c

                MD.PageContext {
                    id: m_page_ctx_c
                    showHeader: false
                    backgroundRadius: BreakPoint.isCompact ? 0 : MD.Token.shape.corner.large
                    showBackground:  !BreakPoint.isCompact
                }
            }
        }
        Loader { // NavigationBar
            id: m_bar_loader
            Layout.fillWidth: true
            active: BreakPoint.isCompact
            visible: active

            sourceComponent: BottomNavBar {
                model: root.pageModel
                currentIndex: root.pageIndex
                onClicked: function (index) { root.pageIndex = index }
            }
        }
    }
}