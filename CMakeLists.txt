cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(cmsgemos)

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

# xdaq dependencies
find_package(Threads REQUIRED)

# https://stackoverflow.com/a/27630120
function(prepend var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

# Needed for xdaq system-dependent includes
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM)

# Define xdaq libraries
function(import_xdaq_library lib_name)
    cmake_parse_arguments(ARG "" "" "DEPENDS" "" ${ARGN})

    find_library(${lib_name}_LOCATION ${lib_name} PATHS /opt/xdaq/lib)
    add_library(${lib_name} SHARED IMPORTED)
    set_property(
        TARGET ${lib_name}
        APPEND PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES /opt/xdaq/include)
    set_property(
        TARGET ${lib_name}
        APPEND PROPERTY
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES /opt/xdaq/include)
    set_property(
        TARGET ${lib_name}
        APPEND PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES /opt/xdaq/include/${SYSTEM})
    set_property(
        TARGET ${lib_name}
        APPEND PROPERTY
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES /opt/xdaq/include/${SYSTEM})
    set_property(
        TARGET ${lib_name}
        PROPERTY IMPORTED_LOCATION
        ${${lib_name}_LOCATION})

    # Dependencies aren't written into .so as they should be, so we need to
    # link explicitely
    set_property(
        TARGET ${lib_name}
        PROPERTY INTERFACE_LINK_LIBRARIES
        ${ARG_DEPENDS})
endfunction()

import_xdaq_library(asyncresolv)
import_xdaq_library(cgicc)
import_xdaq_library(config DEPENDS xcept)
import_xdaq_library(log4cplus)
import_xdaq_library(peer DEPENDS config toolbox xcept xoap ${CMAKE_THREAD_LIBS_INIT})
import_xdaq_library(logudpappender DEPENDS config log4cplus)
import_xdaq_library(logxmlappender DEPENDS config log4cplus)
import_xdaq_library(mimetic)
import_xdaq_library(toolbox DEPENDS asyncresolv cgicc log4cplus uuid ${CMAKE_THREAD_LIBS_INIT})
import_xdaq_library(tstoreclient)
import_xdaq_library(tstoreutils)
import_xdaq_library(tstore DEPENDS tstoreclient tstoreutils xalan-c)
import_xdaq_library(xalan-c)
import_xdaq_library(xcept DEPENDS config toolbox)
import_xdaq_library(xdata DEPENDS config mimetic toolbox xcept xerces-c xoap ${CMAKE_THREAD_LIBS_INIT})
import_xdaq_library(xerces-c)
import_xdaq_library(xgi DEPENDS cgicc config toolbox xcept xerces-c ${CMAKE_THREAD_LIBS_INIT})
import_xdaq_library(xoap DEPENDS config toolbox xcept xerces-c)
import_xdaq_library(xdaq DEPENDS config log4cplus logudpappender logxmlappender peer toolbox xcept xdata xerces-c xgi xoap)

# Python
find_package(PythonLibs 2.6 REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)

add_library(python UNKNOWN IMPORTED)
set_property(TARGET python APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python PROPERTY IMPORTED_LOCATION "${PYTHON_LIBRARIES}")

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem system unit_test_framework)

# MySQL
execute_process(COMMAND mysql_config --cflags
    OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs
    OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(mysqlclient SHARED IMPORTED)
set_property(TARGET mysqlclient PROPERTY INTERFACE_LINK_LIBRARIES ${MYSQL_LIBS})
set_property(TARGET mysqlclient PROPERTY INTERFACE_COMPILE_OPTIONS ${MYSQL_CFLAGS})
set_property(TARGET mysqlclient PROPERTY IMPORTED_LOCATION /usr/lib64/mysql/libmysqlclient.so)

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
    cmake_parse_arguments(ARG "" "" "SOURCES;GENERATED_SOURCES" "" ${ARGN})
    set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common/)
    prepend(SOURCES ${SRC_DIR} ${ARG_SOURCES})

    add_library(${name} SHARED ${SOURCES} ${ARG_GENERATED_SOURCES})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include )
    install(TARGETS ${name} LIBRARY DESTINATION lib COMPONENT ${name})
    install(DIRECTORY include/gem DESTINATION include COMPONENT ${name}-devel PATTERN *.h)
endfunction()

set(COMPONENTS gemutils gembase gemonlinedb)

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
