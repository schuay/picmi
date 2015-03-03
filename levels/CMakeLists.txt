set(picmi_LEVELS
    default.xml
)

file(GLOB picmi_LEVELS_DATA "${CMAKE_CURRENT_SOURCE_DIR}/default/*.xpm")

install(FILES ${picmi_LEVELS} DESTINATION ${KDE_INSTALL_DATADIR}/picmi/levels)
install(FILES ${picmi_LEVELS_DATA} DESTINATION ${KDE_INSTALL_DATADIR}/picmi/levels/default)

# vim:set ts=4 sw=4 et:
