# Find OpenNI library
# This module defines the following variables:
#  OPENNI_INCLUDE_DIRS - include directories for OpenNI
#  OPENNI_LIBRARIES - libraries to link against OpenNI
#  OPENNI_FOUND - true if OpenNI has been found and can be used

find_path(OPENNI_INCLUDE_DIR OpenNI.h
  HINTS ENV OPENNI_DIR
  PATH_SUFFIXES Include include ni
)

find_library(OPENNI_LIBRARY NAMES OpenNI2
  HINTS ENV OPENNI_DIR
  PATH_SUFFIXES lib lib64
)

set(OPENNI_INCLUDE_DIRS ${OPENNI_INCLUDE_DIR})
set(OPENNI_LIBRARIES ${OPENNI_LIBRARY})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(OpenNI REQUIRED_VARS OPENNI_LIBRARY OPENNI_INCLUDE_DIR)

mark_as_advanced(OPENNI_INCLUDE_DIR OPENNI_LIBRARY)
