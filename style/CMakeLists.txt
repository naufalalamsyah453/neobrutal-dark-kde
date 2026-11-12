add_library(neobrutalstyle MODULE
    neobrutalstyle.cpp
    neobrutalstyleplugin.cpp
)
set_target_properties(neobrutalstyle PROPERTIES OUTPUT_NAME neobrutal PREFIX "")
target_link_libraries(neobrutalstyle PRIVATE Qt6::Widgets)

install(TARGETS neobrutalstyle DESTINATION ${NEOBRUTAL_QT_PLUGIN_DIR}/styles)
install(FILES neobrutal.themerc DESTINATION ${NEOBRUTAL_DATA_DIR}/kstyle/themes)
