pragma Singleton
import QtQuick
import Qcm.Material as MD

QtObject {
    // property string text: "MyQmlSingleton"

    readonly property bool isLarge: MD.MProp.size.isExpanded || MD.MProp.size.isLarge || MD.MProp.size.isExtraLarge
    readonly property bool isMedium: MD.MProp.size.isMedium
    readonly property bool isCompact: MD.MProp.size.isCompact

    readonly property bool isAndroid: Qt.platform.os === "android"
    readonly property bool isDesktop: ["linux", "windows", "osx"].includes(Qt.platform.os)
}