CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(DigitalBook)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

ADD_DEFINITIONS(-Wall)

# Qt
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)
FIND_PACKAGE(Qt5OpenGL)

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "OpenGL not found! Please make sure OpenGL is installed.")
ENDIF(OPENGL_FOUND)

# GLEW
FIND_PACKAGE(GLEW REQUIRED)
IF(GLEW_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "GLEW not found! Please make sure GLEW is installed and GLEW_DIR is set correctly")
ENDIF(GLEW_FOUND)

# OpenNI
FIND_PACKAGE(OpenNI REQUIRED)
IF(OPENNI_FOUND)
  INCLUDE_DIRECTORIES(${OPENNI_INCLUDE_DIR})
ELSE()
  MESSAGE(FATAL_ERROR "OpenNI not found! Please make sure OpenNI is installed and OPENNI_DIR is set correctly.")
ENDIF(OPENNI_FOUND)

SET(SOURCES
  src/BookWidget.cpp
  src/DepthAnalyzer.cpp
  src/DepthWidget.cpp
  src/KinectThread.cpp
  src/Main.cpp
  src/MainWindow.cpp
  src/Page.cpp
  src/VideoWidget.cpp
)

QT5_WRAP_UI(UI_SOURCES
  src/MainWindow.ui
)

QT5_ADD_RESOURCES(RESOURCES
  resources.qrc
)

ADD_EXECUTABLE(DigitalBook WIN32 ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES} ${RESOURCES})
QT5_USE_MODULES(DigitalBook Core Gui Widgets OpenGL)
TARGET_LINK_LIBRARIES(DigitalBook ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${OPENNI_LIBRARIES})
