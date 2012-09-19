# modified from FindGLFW.cmake

# Locate Awesomium library & headers
# This module defines the following variables:
# AWESOMIUM_LIBRARY, the name of the library;
# AWESOMIUM_INCLUDE_DIR, where to find the include files.
# AWESOMIUM_FOUND, true if both the AWESOMIUM_LIBRARY and AWESOMIUM_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# AWESOMIUM_ROOT which points to the root of the Awesomium library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw library:
#
# FIND_PACKAGE (Awesomium REQUIRED)
# INCLUDE_DIRECTORIES (${AWESOMIUM_INCLUDE_DIR})
# ADD_EXECUTABLE (executable ${EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${AWESOMIUM_LIBRARY})
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.

#Search for the include file...
FIND_PATH(AWESOMIUM_INCLUDE_PATH WebCore.h DOC "Path to AWESOMIUM include directory."
  HINTS
  $ENV{AWESOMIUM_ROOT}
  PATH_SUFFIX include #For finding the include file under the root of the AWESOMIUM expanded archive, typically on Windows.
  PATHS
  /usr/include/
  /usr/local/include/
  ${AWESOMIUM_ROOT_DIR}/include/
  ${AWESOMIUM_ROOT_DIR}/Awesomium/
  ${CMAKE_CURRENT_SOURCE_DIR}/Awesomium/
)

FIND_LIBRARY(AWESOMIUM_LIBRARY DOC "Absolute path to AWESOMIUM library."
  NAMES Awesomium libawesomium.so
  HINTS
  $ENV{AWESOMIUM_ROOT}
  PATH_SUFFIXES lib/win32 #For finding the library file under the root of the AWESOMIUM expanded archive, typically on Windows.
  PATHS
  /usr/local/lib
  /usr/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/Awesomium/
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/
  ${AWESOMIUM_ROOT_DIR}/Awesomium/
  ${AWESOMIUM_ROOT_DIR}/lib/
  ${AWESOMIUM_ROOT_DIR}/lib-msvc100/release # added by ptr
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AWESOMIUM DEFAULT_MSG AWESOMIUM_LIBRARY AWESOMIUM_INCLUDE_PATH)