
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common/)
set(GEMBASE_SOURCES
    version.cc
    GEMApplication.cc
    GEMFSMApplication.cc
    GEMFSM.cc
    GEMWebApplication.cc
    GEMMonitor.cc
    utils/GEMInfoSpaceToolBox.cc
)
prepend(GEMBASE_SOURCES ${SRC_DIR} ${GEMBASE_SOURCES})

add_library(gembase SHARED ${GEMBASE_SOURCES})
target_include_directories(gembase PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(gembase gemutils xdaq)
install(TARGETS gembase LIBRARY DESTINATION lib COMPONENT gembase)
install(DIRECTORY html DESTINATION /opt/xdaq/htdocs/cmsgemos/gembase COMPONENT gembase)
install(DIRECTORY xml DESTINATION share/gembase COMPONENT gembase)
install(DIRECTORY include/gem DESTINATION include COMPONENT gembase-devel PATTERN *.h)
