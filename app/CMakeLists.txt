    cmake_minimum_required(VERSION 3.21)
    project(wave52 LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 20) # Note: QmlMaterial require C++20
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(QT_QML_GENERATE_QMLLS_INI ON)
    set(QT_NO_PRIVATE_MODULE_WARNING ON) # warning off

    find_package(Qt6 6.5 REQUIRED COMPONENTS
            Core Gui Qml Quick QuickControls2 Bluetooth)

    find_package(Qt6 REQUIRED COMPONENTS ShaderTools)
    if (NOT TARGET Qt6::QuickShapesPrivate)
        find_package(Qt6 REQUIRED COMPONENTS QuickShapesPrivate)
    endif ()
    if (NOT TARGET Qt6::QuickPrivate)
        find_package(Qt6 REQUIRED COMPONENTS QuickPrivate)
    endif ()

    qt_standard_project_setup(REQUIRES 6.5)

    if (QT_KNOWN_POLICY_QTP0004)
        qt_policy(SET QTP0004 NEW)
    endif ()
    if (QT_KNOWN_POLICY_QTP0001)
        qt_policy(SET QTP0001 NEW)
    endif ()
    include(FetchContent)

    set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml_modules") # qml shared folder on build
    set(QM_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(QM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/QmlMaterial)

    # Adafruit GFX + my Arduino shim
    add_library(adafruit_gfx STATIC
            third_party/adafruit-gfx-library/Adafruit_GFX.cpp
            third_party/arduino_shim/Print.cpp
    )

    target_include_directories(adafruit_gfx PUBLIC
            ${CMAKE_SOURCE_DIR}/third_party/arduino_shim
            ${CMAKE_SOURCE_DIR}/third_party/adafruit-gfx-library
    )

    target_compile_definitions(adafruit_gfx PUBLIC
            ARDUINO=10819
    )

    qt_add_executable(wave52
            src/main.cpp
            src/AppController.cpp
            src/AppController.h
            src/BleClient.cpp
            src/BleClient.h
            src/Models/WidgetSerializer.cpp
            src/Models/WidgetSerializer.h
            src/Models/WidgetListModel.cpp
            src/Models/WidgetListModel.h
            src/Image/EinkCanvas.cpp
            src/Image/EinkCanvas.h
            src/Image/ImageConverter.cpp
            src/Image/ImageConverter.h
            src/Image/PreviewProvider.cpp
            src/Image/PreviewProvider.h
            src/Image/ScenePreview.cpp
            src/Image/ScenePreview.h

            src/Widgets/Widget.cpp
            src/Widgets/Widget.h
            src/Widgets/TextBox.cpp
            src/Widgets/TextBox.h

    )
    qt_add_resources(wave52 "conf"
            PREFIX "/"
            FILES qtquickcontrols2.conf
    )

    set_source_files_properties(qml/BreakPoint.qml
            PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

    qt_add_qml_module(wave52
            URI Wave52
            VERSION 1.0
            IMPORT_PATH "${QT_QML_OUTPUT_DIRECTORY}"   # for qmllint/qmlls lint Qcm.Material
            QML_FILES
            qml/Main.qml
            qml/BreakPoint.qml
            qml/Page/HomePage.qml
            qml/Page/ConnectPage.qml
            qml/Page/CanvasPage.qml
            qml/Page/SettingsPage.qml
            qml/Page/PreviewCard.qml
            qml/Page/SearchChip.qml
            qml/Page/WidgetEditor.qml
            qml/Page/BGPropsEditor.qml
            qml/Page/DeviceCmd.qml
            qml/Widgets/BottomNavBar.qml
            qml/Widgets/MaterialTextArea.qml
            qml/Widgets/Preview.qml
            qml/Widgets/ControlPane.qml
            qml/Widgets/ExpandableList.qml
            qml/Widgets/LabeledSlider.qml
    )

    target_include_directories(wave52 PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/third_party/arduino_shim
            ${CMAKE_SOURCE_DIR}/third_party/adafruit-gfx-library
            ${CMAKE_SOURCE_DIR}/third_party/GxEPD_shim
    )

    target_link_libraries(wave52 PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Qml
            Qt6::Quick
            Qt6::QuickControls2
            Qt6::Bluetooth
            adafruit_gfx
            qml_material
    )
    add_dependencies(wave52 qml_materialplugin qml_material_layoutsplugin)

    if (NOT ANDROID)
        target_compile_definitions(wave52 PRIVATE
                QT_QML_OUTPUT_DIRECTORY="${QT_QML_OUTPUT_DIRECTORY}")
    endif ()

    if (QML_MATERIAL_BUILD_TYPE MATCHES STATIC)
        target_link_libraries(wave52 PRIVATE qml_materialplugin)
        target_compile_definitions(wave52 PRIVATE QML_STATIC)
        qt_import_qml_plugins(wave52)
    endif ()

    # Android
    if (ANDROID)
        set_target_properties(wave52 PROPERTIES
                QT_ANDROID_MIN_SDK_VERSION 31
                QT_ANDROID_TARGET_SDK_VERSION 34
                QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
        )
    endif ()

    if (MSVC)
        target_compile_options(wave52 PRIVATE /utf-8)
    endif ()