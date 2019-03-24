cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

# Prevent in-source builds
include(FeatureSummary)
include(PreventInSourceBuilds)

project(cmsgemos)

enable_testing()

function(use_cxx11)
  if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11" PARENT_SCOPE)
    endif()
  else()
    set(CMAKE_CXX_STANDARD 11)
  endif()
endfunction(use_cxx11)

use_cxx11()

# Dependencies
find_package(CACTUS REQUIRED)
find_package(xDAQ 3.4 REQUIRED i2o tstore xdaq xdaq2rc)
find_package(xHAL REQUIRED)

# https://stackoverflow.com/a/27630120
function(prepend var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

# Python
find_package(PythonLibs 2.6 REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)

add_library(python UNKNOWN IMPORTED)
set_property(TARGET python APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python PROPERTY IMPORTED_LOCATION "${PYTHON_LIBRARIES}")

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem iostreams python system unit_test_framework)

# MySQL
find_package(MySQL REQUIRED)
add_library(mysqlclient INTERFACE)
set_property(TARGET mysqlclient PROPERTY INTERFACE_LINK_LIBRARIES ${MYSQL_LIBRARIES})
set_property(TARGET mysqlclient PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MYSQL_INCLUDE_DIR})

# Git version
find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty --always --tags
                    OUTPUT_VARIABLE GIT_VERSION
                    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(GIT_VERSION "unknown")
endif()
execute_process(COMMAND id --user --name OUTPUT_VARIABLE GEMDEVELOPER
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_VERSION=\\\"${GIT_VERSION}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGEMDEVELOPER=\\\"${GEMDEVELOPER}\\\"")

function(add_cmsgemos_module name)
    cmake_parse_arguments(ARG "NO_HEADERS" "" "SOURCES;GENERATED_SOURCES" "" ${ARGN})
    set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common/)
    prepend(SOURCES ${SRC_DIR} ${ARG_SOURCES})

    add_library(${name} SHARED ${SOURCES} ${ARG_GENERATED_SOURCES})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include )
    install(TARGETS ${name} LIBRARY DESTINATION lib COMPONENT ${name})
    if(NOT ARG_NO_HEADERS)
        install(DIRECTORY include/gem DESTINATION include COMPONENT ${name}-devel PATTERN *.h)
    endif()

    # Install HTML files
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/html)
        install(DIRECTORY html DESTINATION share/cmsgemos/htdocs/${name} COMPONENT ${name})
    endif()

    # Install XML files
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/xml)
        install(DIRECTORY xml DESTINATION share/${name} COMPONENT ${name})
    endif()
endfunction()

set(COMPONENTS gemutils gembase gemdaqmonitor gemhardware gemonlinedb gempython gemreadout gemsupervisor)

set(CPACK_COMPONENTS_ALL "${COMPONENTS}")
foreach(c ${COMPONENTS})
    add_subdirectory(${c})
    list(APPEND CPACK_COMPONENTS_ALL ${c}-devel)
endforeach()

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGING_INSTALL_PREFIX /opt/cmsgemos)
set(CPACK_PACKAGE_VERSION "${GIT_VERSION}")
set(CPACK_RPM_COMPONENT_INSTALL ON)
include(CPack)

feature_summary(WHAT ALL)
