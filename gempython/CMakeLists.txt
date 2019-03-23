add_cmsgemos_module(
    gempython
    NO_HEADERS

    SOURCES
    cmsgemos_gempython.cc
)
target_link_libraries(gempython PUBLIC gemhardware_devices)
target_link_libraries(gempython PUBLIC gemutils)
target_link_libraries(gempython PUBLIC ${Boost_PYTHON_LIBRARY})

# TODO Figure out how to install everything correctly
