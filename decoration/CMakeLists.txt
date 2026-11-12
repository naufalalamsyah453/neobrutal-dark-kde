set(CMAKE_AUTOMOC ON)

add_library(neobrutaldecoration MODULE
    neobrutaldecoration.cpp
    neobrutalbutton.cpp
)
set_target_properties(neobrutaldecoration PROPERTIES OUTPUT_NAME org.kde.neobrutal PREFIX "")
target_link_libraries(neobrutaldecoration PRIVATE
    Qt6::Core
    Qt6::Gui
    KF6::CoreAddons
    KDecoration3::KDecoration
)

install(TARGETS neobrutaldecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/${KDECORATION_PLUGIN_DIR})
