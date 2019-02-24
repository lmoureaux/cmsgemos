add_cmsgemos_module(
    gemhardware_devices

    SOURCES
    utils/GEMCrateUtils.cc
    GEMHwDevice.cc
    HwGenericAMC.cc
    vfat/HwVFAT2.cc
    glib/HwGLIB.cc
    optohybrid/HwOptoHybrid.cc
)
target_link_libraries(gemhardware_devices PUBLIC gemutils)
target_link_libraries(gemhardware_devices PUBLIC cactus_uhal_uhal)

install(DIRECTORY include/gem DESTINATION include COMPONENT gemhardware_devices-devel PATTERN *.h)
