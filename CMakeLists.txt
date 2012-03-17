project (DigitalBook)

cmake_minimum_required (VERSION 2.6)
# find qt4
find_package (Qt4 REQUIRED)
# include qt definitions
include (${QT_USE_FILE})
# include binary dir to be able find auto-generated ui_*.h files
# include source dir to be able to find header files from ui_*.h files
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
# add compiler definitions
ADD_DEFINITIONS( -Wall -O3 )
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
target_link_libraries (DigitalBook freenect freenect_sync OgreMain ${QT_LIBRARIES})
