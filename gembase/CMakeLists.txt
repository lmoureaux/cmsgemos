add_cmsgemos_module(gembase SOURCES
    version.cc
    GEMApplication.cc
    GEMFSMApplication.cc
    GEMFSM.cc
    GEMWebApplication.cc
    GEMMonitor.cc
    utils/GEMInfoSpaceToolBox.cc
)
target_link_libraries(gembase gemutils)
target_link_libraries(gembase xdaq)

install(DIRECTORY html DESTINATION /opt/xdaq/htdocs/cmsgemos/gembase COMPONENT gembase)
install(DIRECTORY xml DESTINATION share/gembase COMPONENT gembase)
