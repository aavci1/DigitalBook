CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10)

PROJECT(DigitalBook)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)

# Qt
FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_FOUND)
  SET(QT_USE_QTXML TRUE)
  INCLUDE(${QT_USE_FILE})
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
ELSE()
  MESSAGE(FATAL_ERROR "Qt not found! Please make sure Qt is installed.")
ENDIF(QT_FOUND)

# OGRE
SET(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/cmake/;${CMAKE_MODULE_PATH}")

FIND_PACKAGE(OGRE REQUIRED)
IF(OGRE_FOUND)
  INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "OGRE not found! Please make sure OGRE is installed and OGRE_HOME is set correctly.")
ENDIF(OGRE_FOUND)

# Boost
IF(WIN32)
  SET(OGRE_BOOST_COMPONENTS)
ELSE()
  SET(OGRE_BOOST_COMPONENTS system)
ENDIF(WIN32)

FIND_PACKAGE(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} REQUIRED)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
  ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
ELSE()
  MESSAGE(FATAL_ERROR "Boost not found! Please make sure Boost is installed and BOOST_ROOT is set correctly.")
ENDIF(Boost_FOUND)

# Freenect
FIND_PACKAGE(Freenect REQUIRED)
IF(FREENECT_FOUND)
  INCLUDE_DIRECTORIES(${FREENECT_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "Freenect not found! Please make sure Freenect is installed and FREENECT_DIR is set correctly.")
ENDIF(FREENECT_FOUND)

# add source files
set (SOURCE_FILES
  src/DepthAnalyzer.cpp
  src/DepthWidget.cpp
  src/KinectManager.cpp
  src/Main.cpp
  src/MainWindow.cpp
  src/OgreManager.cpp
  src/OgreWidget.cpp
  src/VideoWidget.cpp
)
# add ui files
qt4_wrap_ui (UI_FILES
  src/MainWindow.ui
)
# add headers to be moc'ed
qt4_wrap_cpp (MOC_HEADERS
  src/DepthAnalyzer.h
  src/DepthWidget.h
  src/KinectManager.h
  src/MainWindow.h
  src/OgreManager.h
  src/OgreWidget.h
  src/VideoWidget.h
)
# add resource files
qt4_add_resources (RESOURCE_FILES
  resources.qrc
)
# set up the executable target
add_executable (DigitalBook WIN32 ${SOURCE_FILES} ${UI_FILES} ${MOC_HEADERS} ${RESOURCE_FILES} resources.rc)
# link libraries
target_link_libraries (DigitalBook ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${Boost_LIBRARIES} ${FREENECT_LIBRARIES})
