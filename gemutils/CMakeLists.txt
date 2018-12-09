
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common/)
set(GEMUTILS_SOURCES
    version.cc
    Lock.cc
    soap/GEMSOAPToolBox.cc
    db/GEMDatabaseUtils.cc
)
prepend(GEMUTILS_SOURCES ${SRC_DIR} ${GEMUTILS_SOURCES})

add_library(gemutils SHARED ${GEMUTILS_SOURCES})
target_include_directories(gemutils PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(gemutils xdaq)
target_link_libraries(gemutils python)
install(TARGETS gemutils LIBRARY DESTINATION lib COMPONENT gemutils-bin)
install(DIRECTORY include/gem DESTINATION include COMPONENT gemutils-devel PATTERN *.h)
