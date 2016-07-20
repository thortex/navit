add_definitions( -DMODULE=gui_qml2 )
include_directories(${CMAKE_CURRENT_BINARY_DIR})
module_add_library(gui_qml2 gui_qml2.cpp)
target_link_libraries(gui_qml2 Qt5::Widgets Qt5::Quick)
