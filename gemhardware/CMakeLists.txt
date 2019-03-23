add_cmsgemos_module(
    gemhardware_devices

    SOURCES
    utils/GEMCrateUtils.cc
    GEMHwDevice.cc
    HwGenericAMC.cc
    glib/HwGLIB.cc
    optohybrid/HwOptoHybrid.cc
)
target_link_libraries(gemhardware_devices PUBLIC gemutils)
target_link_libraries(gemhardware_devices PUBLIC CACTUS::uhal_uhal)
target_link_libraries(gemhardware_devices PUBLIC xHAL::xhal_core)

add_cmsgemos_module(
    gemhardware_managers

    SOURCES
    version.cc
    utils/GEMCrateUtils.cc
    vfat/VFAT2Manager.cc
    vfat/VFAT2ControlPanelWeb.cc
    amc13/AMC13Manager.cc
    amc13/AMC13ManagerWeb.cc
    amc13/AMC13Readout.cc
    glib/GLIBManager.cc
    glib/GLIBManagerWeb.cc
    glib/GLIBMonitor.cc
    optohybrid/OptoHybridManager.cc
    optohybrid/OptoHybridManagerWeb.cc
    optohybrid/OptoHybridMonitor.cc
)
target_link_libraries(gemhardware_managers PUBLIC gemhardware_devices)
target_link_libraries(gemhardware_managers PUBLIC gemreadout)
target_link_libraries(gemhardware_managers PUBLIC CACTUS::amc13_amc13)

install(DIRECTORY include/gem DESTINATION include COMPONENT gemhardware-devel PATTERN *.h)
