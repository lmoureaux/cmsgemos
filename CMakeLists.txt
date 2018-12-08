cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(cmsgemos)

macro(use_cxx11)
  if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif()
  else()
    set(CMAKE_CXX_STANDARD 11)
  endif()
endmacro(use_cxx11)

use_cxx11()

# Needed for xdaq system-dependent includes
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM)

# Define xdaq
add_library(xdaq SHARED IMPORTED)
set_property(TARGET xdaq APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES /opt/xdaq/include)
set_property(TARGET xdaq APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES /opt/xdaq/include)
set_property(TARGET xdaq APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES /opt/xdaq/include/${SYSTEM})
set_property(TARGET xdaq APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES /opt/xdaq/include/${SYSTEM})
set_property(TARGET xdaq PROPERTY IMPORTED_LOCATION /opt/xdaq/lib/libxdaq.so)

# Python
find_package(PythonLibs 2.6 REQUIRED)

add_library(python UNKNOWN IMPORTED)
set_property(TARGET python APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
set_property(TARGET python PROPERTY IMPORTED_LOCATION "${PYTHON_LIBRARIES}")

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

add_subdirectory(gemutils)
add_subdirectory(gembase)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION "${GIT_VERSION}")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${CMAKE_PROJECT_NAME}")
include(CPack)
