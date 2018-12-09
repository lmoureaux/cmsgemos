add_cmsgemos_module(gemutils SOURCES
    version.cc
    Lock.cc
    GEMRegisterUtils.cc
    soap/GEMSOAPToolBox.cc
    db/GEMDatabaseUtils.cc
)
target_link_libraries(gemutils xdaq)
target_link_libraries(gemutils python)

install(DIRECTORY include/gem DESTINATION include COMPONENT gemutils-devel PATTERN *.h)
