# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/adafruit_gfx_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/adafruit_gfx_autogen.dir/ParseCache.txt"
  "CMakeFiles/wave52_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/wave52_autogen.dir/ParseCache.txt"
  "adafruit_gfx_autogen"
  "third_party/QmlMaterial/CMakeFiles/qml_material_autogen.dir/AutogenUsed.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_material_autogen.dir/ParseCache.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_material_layouts_autogen.dir/AutogenUsed.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_material_layouts_autogen.dir/ParseCache.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_material_layoutsplugin_autogen.dir/AutogenUsed.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_material_layoutsplugin_autogen.dir/ParseCache.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_materialplugin_autogen.dir/AutogenUsed.txt"
  "third_party/QmlMaterial/CMakeFiles/qml_materialplugin_autogen.dir/ParseCache.txt"
  "third_party/QmlMaterial/qml_material_autogen"
  "third_party/QmlMaterial/qml_material_layouts_autogen"
  "third_party/QmlMaterial/qml_material_layoutsplugin_autogen"
  "third_party/QmlMaterial/qml_materialplugin_autogen"
  "third_party/QmlMaterial/third_party/CMakeFiles/material_color_autogen.dir/AutogenUsed.txt"
  "third_party/QmlMaterial/third_party/CMakeFiles/material_color_autogen.dir/ParseCache.txt"
  "third_party/QmlMaterial/third_party/material_color_autogen"
  "wave52_autogen"
  )
endif()
