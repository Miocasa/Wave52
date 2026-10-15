import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// Material 3 demo: all main Qt Quick Controls + live style settings.
// The style must be Material: QQuickStyle::setStyle("Material") in main.cpp
// or QT_QUICK_CONTROLS_STYLE=Material.
ApplicationWindow {
    id: win
    width: 720
    height: 720
    visible: true
    title: "Wave52 — Material 3 demo"

    readonly property bool wide: width >= 900

    // ---- option tables ----------------------------------------------------
    readonly property var colorNames: ["Red", "Pink", "Purple", "DeepPurple", "Indigo", "Blue", "LightBlue",
        "Cyan", "Teal", "Green", "LightGreen", "Lime", "Yellow", "Amber", "Orange", "DeepOrange",
        "Brown", "Grey", "BlueGrey"]
    readonly property var colorValues: [Material.Red, Material.Pink, Material.Purple, Material.DeepPurple,
        Material.Indigo, Material.Blue, Material.LightBlue, Material.Cyan, Material.Teal, Material.Green,
        Material.LightGreen, Material.Lime, Material.Yellow, Material.Amber, Material.Orange,
        Material.DeepOrange, Material.Brown, Material.Grey, Material.BlueGrey]

    readonly property var themeNames: ["Light", "Dark", "System"]
    readonly property var themeValues: [Material.Light, Material.Dark, Material.System]

    readonly property var roundNames: ["NotRounded", "ExtraSmall", "Small", "Medium", "Large", "ExtraLarge", "Full"]
    readonly property var roundValues: [Material.NotRounded, Material.ExtraSmallScale, Material.SmallScale,
        Material.MediumScale, Material.LargeScale, Material.ExtraLargeScale, Material.FullScale]

    // ---- live style settings (inherited by all children) ------------------
    Material.theme: themeValues[themeBox.currentIndex]
    Material.accent: colorValues[accentBox.currentIndex]
    Material.primary: colorValues[primaryBox.currentIndex]
    Material.roundedScale: roundValues[roundBox.currentIndex]

    // not inherited: applied per control below
    readonly property int containerStyle: filledCheck.checked ? Material.Filled : Material.Outlined
    readonly property real elev: elevSlider.value
    readonly property bool upperCase: capsCheck.checked

    // ---- header / drawer / menu -------------------------------------------
    header: ToolBar {
        Material.elevation: win.elev
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: "\u2630"
                font.pixelSize: 20
                onClicked: drawer.open()
            }
            Label {
                text: win.title
                elide: Label.ElideRight
                Layout.fillWidth: true
                font.pixelSize: 18
            }
            ToolButton {
                text: "\u22EE"
                font.pixelSize: 20
                onClicked: mainMenu.open()

                Menu {
                    id: mainMenu
                    y: parent.height
                    MenuItem { text: "Open dialog"; onTriggered: dialog.open() }
                    MenuItem { text: "Show popup"; onTriggered: popup.open() }
                    MenuSeparator {}
                    Menu {
                        title: "Submenu"
                        MenuItem { text: "Item 1" }
                        MenuItem { text: "Item 2"; checkable: true; checked: true }
                    }
                    MenuItem { text: "Quit"; onTriggered: Qt.quit() }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(win.width * 0.7, 300)
        height: win.height
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            Label { text: "Drawer"; font.pixelSize: 20 }
            ItemDelegate { text: "Item delegate 1"; Layout.fillWidth: true }
            ItemDelegate { text: "Item delegate 2"; Layout.fillWidth: true }
            ItemDelegate { text: "Close"; Layout.fillWidth: true; onClicked: drawer.close() }
            Item { Layout.fillHeight: true }
        }
    }

    Dialog {
        id: dialog
        title: "Dialog"
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Cancel | Dialog.Ok
        Label { text: "Modal dialog (corners: roundedScale ExtraLarge)" }
    }

    Popup {
        id: popup
        anchors.centerIn: Overlay.overlay
        padding: 16
        modal: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        Label { text: "Popup. Tap outside to close." }
    }

    // ---- content -----------------------------------------------------------
    ScrollView {
        id: scroll
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        GridLayout {
            width: scroll.availableWidth
            columns: win.wide ? 2 : 1
            columnSpacing: 12
            rowSpacing: 12

            // ===== Style settings =====
            Section {
                title: "Style settings"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Label { text: "Theme" }
                    ComboBox { id: themeBox; model: win.themeNames; currentIndex: 1; Layout.fillWidth: true }
                    Label { text: "Accent" }
                    ComboBox { id: accentBox; model: win.colorNames; currentIndex: 2; Layout.fillWidth: true }
                    Label { text: "Primary" }
                    ComboBox { id: primaryBox; model: win.colorNames; currentIndex: 18; Layout.fillWidth: true }
                    Label { text: "Rounded scale" }
                    ComboBox { id: roundBox; model: win.roundNames; currentIndex: 3; Layout.fillWidth: true }
                }
                RowLayout {
                    Label { text: "Elevation: " + Math.round(win.elev) }
                    Slider {
                        id: elevSlider
                        from: 0; to: 24; value: 2; stepSize: 1
                        Layout.fillWidth: true
                    }
                }
                CheckBox { id: filledCheck; text: "Filled text fields (containerStyle)" }
                CheckBox { id: capsCheck; text: "Uppercase button text" }
            }

            // ===== Buttons =====
            Section {
                title: "Buttons"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                Flow {
                    Layout.fillWidth: true
                    spacing: 8
                    Button { text: "Default"; Material.elevation: win.elev; font.capitalization: win.upperCase ? Font.AllUppercase : Font.MixedCase }
                    Button { text: "Highlighted"; highlighted: true; Material.elevation: win.elev; font.capitalization: win.upperCase ? Font.AllUppercase : Font.MixedCase }
                    Button { text: "Flat"; flat: true; font.capitalization: win.upperCase ? Font.AllUppercase : Font.MixedCase }
                    Button { text: "Checkable"; checkable: true; font.capitalization: win.upperCase ? Font.AllUppercase : Font.MixedCase }
                    Button { text: "Disabled"; enabled: false }
                    Button { text: "Accent"; Material.background: Material.accent; Material.foreground: "white" }
                    RoundButton { text: "+" }
                    RoundButton { text: "\u2713"; highlighted: true }
                    ToolButton { text: "\u2605" }
                }
                DelayButton {
                    text: "Hold to confirm"
                    delay: 1200
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    TabBar {
                        id: tabs
                        Layout.fillWidth: true
                        TabButton { text: "One" }
                        TabButton { text: "Two" }
                        TabButton { text: "Three" }
                    }
                }
                Label { text: "Tab: " + (tabs.currentIndex + 1); opacity: 0.7 }
            }

            // ===== Selection =====
            Section {
                title: "Selection"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                RowLayout {
                    CheckBox { text: "Check"; checked: true }
                    CheckBox { text: "Tri-state"; tristate: true; checkState: Qt.PartiallyChecked }
                    CheckBox { text: "Off"; enabled: false }
                }
                RowLayout {
                    RadioButton { text: "A"; checked: true }
                    RadioButton { text: "B" }
                    RadioButton { text: "C" }
                }
                RowLayout {
                    Switch { id: mainSwitch; text: "Switch" }
                    Switch { checked: true; text: "On" }
                    Switch { enabled: false; text: "Disabled" }
                }
                Label { text: "Main switch is " + (mainSwitch.checked ? "ON" : "OFF"); opacity: 0.7 }
            }

            // ===== Sliders / values =====
            Section {
                title: "Sliders and values"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                Slider { id: slider; Layout.fillWidth: true; from: 0; to: 100; value: 40 }
                Slider {
                    Layout.fillWidth: true
                    from: 0; to: 10; value: 5; stepSize: 1
                    snapMode: Slider.SnapAlways   // shows tick marks
                }
                RangeSlider {
                    Layout.fillWidth: true
                    from: 0; to: 100
                    first.value: 20; second.value: 70
                }
                ProgressBar { Layout.fillWidth: true; from: 0; to: 100; value: slider.value }
                ProgressBar { Layout.fillWidth: true; indeterminate: true }
                RowLayout {
                    SpinBox { from: 0; to: 100; value: 25; editable: true }
                    BusyIndicator { running: true }
                    Dial { from: 0; to: 100; value: slider.value; implicitWidth: 72; implicitHeight: 72 }
                    Tumbler { model: 24; visibleItemCount: 3; implicitHeight: 90; implicitWidth: 60 }
                }
            }

            // ===== Text input =====
            Section {
                title: "Text input"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Text field"
                    Material.containerStyle: win.containerStyle
                }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Password"
                    echoMode: TextInput.Password
                    Material.containerStyle: win.containerStyle
                }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "With error"
                    text: "wrong"
                    Material.containerStyle: win.containerStyle
                    Material.accent: Material.Red
                }
                TextArea {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    placeholderText: "Text area"
                    Material.containerStyle: win.containerStyle
                }
                ComboBox {
                    Layout.fillWidth: true
                    model: ["Alpha", "Beta", "Gamma"]
                }
                ComboBox {
                    Layout.fillWidth: true
                    editable: true
                    model: ["Editable 1", "Editable 2", "Editable 3"]
                }
            }

            // ===== Containers =====
            Section {
                title: "Containers"
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 1

                Pane {
                    Layout.fillWidth: true
                    Material.elevation: win.elev
                    Label { text: "Pane (elevation follows the slider)" }
                }
                Frame {
                    Layout.fillWidth: true
                    Label { text: "Frame" }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: "Dialog"
                        onClicked: dialog.open()
                    }
                    Button {
                        text: "Popup"
                        onClicked: popup.open()
                    }
                    Button {
                        text: "Tooltip"
                        ToolTip.visible: hovered || pressed
                        ToolTip.text: "ToolTip text"
                        ToolTip.delay: 300
                    }
                }
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140
                    clip: true
                    model: 12
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        text: "List item " + (index + 1)
                    }
                }
            }
        }
    }
}
