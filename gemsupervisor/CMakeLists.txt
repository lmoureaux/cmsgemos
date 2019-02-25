add_cmsgemos_module(
    gemsupervisor

    SOURCES
    version.cc
    tbutils/VFAT2XMLParser.cc
    GEMSupervisor.cc
    GEMSupervisorWeb.cc
    GEMSupervisorMonitor.cc
    GEMGlobalState.cc
)
target_link_libraries(gemsupervisor PUBLIC CACTUS::amc13_amc13)
target_link_libraries(gemsupervisor PUBLIC CACTUS::uhal_uhal)
target_link_libraries(gemsupervisor PUBLIC gembase)
target_link_libraries(gemsupervisor PUBLIC gemhardware_devices)
target_link_libraries(gemsupervisor PUBLIC gemhardware_managers)
target_link_libraries(gemsupervisor PUBLIC gemreadout)
target_link_libraries(gemsupervisor PUBLIC gemutils)
target_link_libraries(gemsupervisor PUBLIC xDAQ::xdaq2rc)

install(DIRECTORY include/gem DESTINATION include COMPONENT gemsupervisor-devel PATTERN *.h)
