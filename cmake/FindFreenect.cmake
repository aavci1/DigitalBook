# Find the free kinect library
# This module defines the following variables:
#  FREENECT_INCLUDE_DIRS - include directories for Freenect
#  FREENECT_LIBRARIES - libraries to link against Freenect
#  FREENECT_FOUND - true if Freenect has been found and can be used

find_path(FREENECT_INCLUDE_DIR libfreenect.h libfreenect_sync.h
  HINTS ENV FREENECT_DIR
  PATH_SUFFIXES include libfreenect
)

find_library(FREENECT_LIBRARY NAMES freenect
  HINTS ENV FREENECT_DIR
  PATH_SUFFIXES lib lib64
)

find_library(FREENECT_SYNC_LIBRARY NAMES freenect_sync
  HINTS ENV FREENECT_DIR
  PATH_SUFFIXES lib lib64
)

set(FREENECT_INCLUDE_DIRS ${FREENECT_INCLUDE_DIR})
set(FREENECT_LIBRARIES ${FREENECT_LIBRARY} ${FREENECT_SYNC_LIBRARY})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Freenect REQUIRED_VARS FREENECT_LIBRARY FREENECT_SYNC_LIBRARY FREENECT_INCLUDE_DIR)

mark_as_advanced(FREENECT_INCLUDE_DIR FREENECT_LIBRARY FREENECT_SYNC_LIBRARY)
