pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Qcm.Material as MD

MD.Page {
    id: root
    showBackground: false
    padding: MD.MProp.size.isCompact ? 0 : 12

    property var allItems: { // todo implement save system
        const a = [];
        for (let i = 0; i < 40; ++i)
            a.push({
                id: "id" + i,
                name: "Item " + i,
                size: (i * 37) % 100,
                modified: (i * 53) % 90,
                hidden: i % 7 === 0
            });
        return a;
    }
    property string searchText: ""
    property bool loading: false

    ListModel {
        id: itemsModel
    }

    // sorting
    readonly property var sortOptions: [
        {name: qsTr("Name"), key: "name"},
        {name: qsTr("Size"), key: "size"},
        {name: qsTr("Last modified"), key: "modified"}
    ]
    property int sortIndex: 0
    property bool sortAsc: true

    function rebuild() {
        const q = searchText.trim().toLowerCase();
        const key = sortOptions[sortIndex].key;
        const dir = sortAsc ? 1 : -1;
        const list = allItems.filter(it => q.length === 0 || it.name.toLowerCase().indexOf(q) >= 0);
        list.sort((a, b) => (a[key] < b[key] ? -1 : a[key] > b[key] ? 1 : 0) * dir);
        itemsModel.clear();
        for (const it of list)
            itemsModel.append(it);
    }

    function pickSort(idx) {
        if (idx === sortIndex)
            sortAsc = !sortAsc;
        else
            sortIndex = idx;
        rebuild();
    }

    onSearchTextChanged: rebuild()
    Component.onCompleted: rebuild()

    // selection
    property var selection: ({}) // { id: true }
    readonly property int selectedCount: Object.keys(selection).length
    readonly property bool selectionActive: selectedCount > 0

    function toggleSelected(id) {
        const next = Object.assign({}, selection);
        if (next[id]) delete next[id]; else next[id] = true;
        selection = next;
        selectedItem = null;
    }

    function clearSelection() {
        selection = ({});
    }

    // details
    property var selectedItem: null

    function handleClick(index, modifiers) {
        const it = itemsModel.get(index);
        if (selectionActive || (modifiers & Qt.ControlModifier)) {
            toggleSelected(it.id);
            return;
        }
        grid.currentIndex = index;
        selectedItem = {
            id: it.id,
            name: it.name,
            size: it.size,
            modified: it.modified,
            hidden: it.hidden
        };
        detailPresenter.present();
    }

    onSelectionActiveChanged: {
        if (selectionActive) {
            selectedItem = null;
            grid.currentIndex = -1;
        }
    }

    // actions for toolbar
    MD.Action {
        id: tweakAction
        text: qsTr("Tweak")
        icon.name: MD.Token.icon.tune
        onTriggered: console.log("tweak")
    }
    MD.Action {
        id: filterAction
        text: qsTr("Filters")
        icon.name: MD.Token.icon.filter_list
        onTriggered: console.log("filters")
    }
    MD.Action {
        id: refreshAction
        text: qsTr("Refresh")
        icon.name: MD.Token.icon.refresh
        enabled: !root.loading
        onTriggered: {
            root.loading = true;
            refreshTimer.restart();
        }
    }
    Timer {
        id: refreshTimer
        interval: 1500
        onTriggered: { root.loading = false; root.rebuild(); }
    }

    // Detail panels (one for bottom sheet and side view)
    property Item detailPanel: Item
    {
        id: panel
        parent: null
        implicitWidth: 280
        implicitHeight: 400

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            RowLayout {
                Layout.fillWidth: true
                MD.Text {
                    Layout.fillWidth: true
                    text: root.selectedItem
                    ?.
                        name ?? ""
                    typescale: MD.Token.typescale.title_large
                    color: MD.Token.color.on_surface
                }
                MD.IconButton {
                    icon.name: MD.Token.icon.close
                    onClicked: detailPresenter.dismiss()
                }
            }
            MD.Text {
                text: qsTr("Size: %1").arg(root.selectedItem
                ?.
                    size ?? 0
                )
                typescale: MD.Token.typescale.body_medium
                color: MD.Token.color.on_surface_variant
            }
            MD.Text {
                text: qsTr("Modified: %1").arg(root.selectedItem
                ?.
                    modified ?? 0
                )
                typescale: MD.Token.typescale.body_medium
                color: MD.Token.color.on_surface_variant
            }
            Item {
                Layout.fillHeight: true
            }
            MD.Button {
                Layout.fillWidth: true
                text: qsTr("Apply")
                mdState.type: MD.Enum.BtFilled
                onClicked: console.log("apply", root.selectedItem
                ?.
                id
                )
            }
        }
    }

    MD.AdaptivePresenter {
        id: detailPresenter
        content: root.detailPanel
        enabled: root.visible
        destination: root.selectionActive || !root.selectedItem
            ? null
            : detailSplit.width >= MD.Token.window_class.expanded.min_width ? sideSite : sheetSite
        onDismissed: {
            root.selectedItem = null;
            grid.currentIndex = -1;
        }
    }

    MD.BottomSheet {
        id: detailSheet
        parent: root
        nestedScrollEnabled: true
        maxSheetWidth: 400
        wideSideMargin: 24
        preferredContentHeight: root.height

        MD.PresentationSite {
            id: sheetSite
            presenter: detailPresenter
            popup: detailSheet
            width: detailSheet.contentViewportWidth
            height: detailSheet.contentViewportHeight
        }
    }

    // Layout
    contentItem: MD.SplitView
    {
        id: detailSplit
        orientation: Qt.Horizontal
        handle: null
        spacing: 24

        // Toolbar and grid
        MD.Pane {
            MD.SplitViewBase.fillWidth: true
            radius: root.MD.MProp.page.backgroundRadius
            padding: 0
            showBackground: true

            contentItem: ColumnLayout {
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.topMargin: 4
                    spacing: 8

                    MD.EmbedChip {
                        id: sortChip
                        text: root.sortOptions[root.sortIndex].name
                        trailingIconName: root.sortAsc ? MD.Token.icon.arrow_downward : MD.Token.icon.arrow_upward
                        mdState.borderWidth: 1
                        onClicked: sortMenu.open()

                        MD.Menu {
                            id: sortMenu
                            parent: sortChip
                            y: parent.height
                            model: root.sortOptions
                            contentDelegate: MD.MenuItem
                            {
                                required property var modelData
                                required property int index
                                text: modelData.name
                                icon.name: index === root.sortIndex ? (root.sortAsc ? MD.Token.icon.arrow_downward : MD.Token.icon.arrow_upward) : ' '
                                onClicked: {
                                    root.pickSort(index);
                                    sortMenu.close();
                                }
                            }
                        }
                    }

                    SearchChip {
                        id: searchField
                        Layout.preferredWidth: 160
                        placeholderText: qsTr("Search")
                        onTextEdited: root.searchText = text
                    }

                    MD.ActionToolBar {
                        Layout.fillWidth: true
                        actions: [tweakAction, filterAction, refreshAction]
                    }
                }

                MD.LinearIndicator {
                    Layout.fillWidth: true
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.topMargin: 4
                    visible: grid.count > 0 && root.loading
                    running: visible
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    MD.VerticalGridView {
                        id: grid
                        anchors.fill: parent
                        clip: true
                        focus: true
                        keyNavigationEnabled: true
                        currentIndex: -1
                        cacheBuffer: 300
                        topMargin: 2
                        bottomMargin: 8
                        leftMargin: 8
                        rightMargin: 8
                        visible: count > 0

                        readonly property real itemSize: 162 //200
                        readonly property real availableWidth: Math.max(0, width - leftMargin - rightMargin)
                        readonly property int cols: Math.max(1, Math.floor(availableWidth / itemSize))
                        cellWidth: availableWidth / cols
                        cellHeight: cellWidth

                        model: itemsModel

                        delegate: PreviewCard {
                            selected: root.selection[model.id] === true
                            hidden: model.hidden
                            title: model.name
                            source: "file:///home/miocasa/Pictures/photo_2026-09-29_20-08-27.jpg" //todo after save system was added
                            itemWidth: grid.cellWidth
                            itemHeight: grid.cellHeight
                            onClicked: modifiers => root.handleClick(index, modifiers)
                            onSelectionRequested: modifiers => root.toggleSelected(model.id)
                        }

                        Keys.onEscapePressed: event => {
                            if (root.selectionActive) {
                                root.clearSelection();
                                event.accepted = true;
                            }
                        }

                        highlightFollowsCurrentItem: true
                        highlight: Component {
                            Item {
                                visible: grid.currentItem !== null
                                z: 2
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    color: "transparent"
                                    border.color: MD.Token.color.primary
                                    border.width: 3
                                    radius: MD.Token.shape.corner.extra_small + 4
                                }
                            }
                        }
                    }

                    // counter button cancel selection
                    MD.Button {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.leftMargin: 16
                        anchors.topMargin: 12
                        z: 10
                        visible: root.selectionActive
                        checked: true
                        text: String(root.selectedCount)
                        icon.name: MD.Token.icon.close
                        mdState.type: MD.Enum.BtElevated
                        onClicked: root.clearSelection()
                    }

                    // empty value
                    MD.Loader {
                        anchors.centerIn: parent
                        active: grid.count === 0
                        sourceComponent: ColumnLayout {
                            spacing: 16
                            MD.BusyIndicator {
                                Layout.alignment: Qt.AlignHCenter
                                running: root.loading
                            }
                            MD.Text {
                                Layout.alignment: Qt.AlignHCenter
                                visible: !root.loading
                                text: qsTr("Nothing found")
                                typescale: MD.Token.typescale.body_large
                                color: MD.Token.color.on_surface_variant
                            }
                        }
                    }
                }
            }
        }

        // right panel (wide screen mode)
        MD.Pane {
            MD.SplitViewBase.minimumWidth: 280
            MD.SplitViewBase.preferredWidth: 280
            MD.SplitViewBase.maximumWidth: 280
            visible: sideSite.current
            radius: root.MD.MProp.page.backgroundRadius
            padding: 0
            showBackground: true

            contentItem: MD.PresentationSite
            {
                id: sideSite
                presenter: detailPresenter
            }
        }
    }
}