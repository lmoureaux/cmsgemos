add_cmsgemos_module(gemutils SOURCES
    version.cc
    Lock.cc
    soap/GEMSOAPToolBox.cc
    db/GEMDatabaseUtils.cc
)
target_link_libraries(gemutils xdaq)
target_link_libraries(gemutils python)

install(TARGETS gemutils LIBRARY DESTINATION lib COMPONENT gemutils-bin)
install(DIRECTORY include/gem DESTINATION include COMPONENT gemutils-devel PATTERN *.h)
