CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(DigitalBook)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)

ADD_DEFINITIONS(-Wall)

# Qt
FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_FOUND)
  SET(QT_USE_QTOPENGL TRUE)
  INCLUDE(${QT_USE_FILE})
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
ELSE()
  MESSAGE(FATAL_ERROR "Qt not found! Please make sure Qt is installed.")
ENDIF(QT_FOUND)

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

QT4_WRAP_UI(UI_SOURCES
  src/MainWindow.ui
)

QT4_WRAP_CPP(MOC_SOURCES
  src/BookWidget.h
  src/DepthAnalyzer.h
  src/DepthWidget.h
  src/KinectThread.h
  src/MainWindow.h
  src/VideoWidget.h
)

QT4_ADD_RESOURCES(RESOURCES
  resources.qrc
)

ADD_EXECUTABLE(DigitalBook WIN32 ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES} ${RESOURCES} resources.rc)
TARGET_LINK_LIBRARIES(DigitalBook ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${OPENNI_LIBRARIES})
