CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(DigitalBook)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
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

# OpenNI
FIND_PACKAGE(OpenNI REQUIRED)
IF(OPENNI_FOUND)
  INCLUDE_DIRECTORIES(${OPENNI_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "OpenNI not found! Please make sure OpenNI is installed and OPENNI_DIR is set correctly.")
ENDIF(OPENNI_FOUND)

SET(SOURCES
  src/DepthAnalyzer.cpp
  src/DepthWidget.cpp
  src/KinectThread.cpp
  src/Main.cpp
  src/MainWindow.cpp
  src/OgreManager.cpp
  src/OgreWidget.cpp
  src/VideoWidget.cpp
)

QT4_WRAP_UI(UI_SOURCES
  src/MainWindow.ui
)

QT4_WRAP_CPP(MOC_SOURCES
  src/DepthAnalyzer.h
  src/DepthWidget.h
  src/KinectThread.h
  src/MainWindow.h
  src/OgreManager.h
  src/OgreWidget.h
  src/VideoWidget.h
)

QT4_ADD_RESOURCES(RESOURCES
  resources.qrc
)

ADD_EXECUTABLE(DigitalBook WIN32 ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES} ${RESOURCES} resources.rc)
TARGET_LINK_LIBRARIES(DigitalBook ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${Boost_LIBRARIES} ${OPENNI_LIBRARIES})
