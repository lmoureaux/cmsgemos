add_cmsgemos_module(
    gemdaqmonitor

    SOURCES
    DaqMonitor.cc
    ShelfMonitor.cc
    ShelfMonitorWeb.cc
    version.cc
)
target_link_libraries(gemdaqmonitor PUBLIC gembase)
target_link_libraries(gemdaqmonitor PUBLIC gemhardware_devices)
target_link_libraries(gemdaqmonitor PUBLIC gemutils)
target_link_libraries(gemdaqmonitor PUBLIC xHAL::xhal_core)
target_link_libraries(gemdaqmonitor PUBLIC ${boost_IOSTREAMS_LIBRARY})

install(DIRECTORY include/gem DESTINATION include COMPONENT gemdaqmonitor-devel PATTERN *.h)
