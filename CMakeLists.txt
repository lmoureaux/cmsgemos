cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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

# https://stackoverflow.com/a/27630120
function(prepend var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

# xhal
# FIXME Use FindxHAL.cmake once provided
find_library(WISCRPCSVC_LOCATION wiscrpcsvc PATHS /opt/wiscrpcsvc/lib)
add_library(wiscrpcsvc SHARED IMPORTED)
set_property(TARGET wiscrpcsvc PROPERTY IMPORTED_LOCATION ${WISCRPCSVC_LOCATION})

find_library(XHAL_LOCATION xhal PATHS /opt/xhal/lib)
add_library(xhal SHARED IMPORTED)
set_property(
    TARGET xhal
    APPEND PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES /opt/xhal/include)
set_property(
    TARGET xhal
    APPEND PROPERTY
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES /opt/xhal/include)
set_property(TARGET xhal PROPERTY IMPORTED_LOCATION ${XHAL_LOCATION})
set_property(TARGET xhal PROPERTY INTERFACE_LINK_LIBRARIES wiscrpcsvc)

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
