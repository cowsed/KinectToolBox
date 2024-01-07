cmake_minimum_required(VERSION 3.26)

project(KinectToolBox VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ADDRESS_SANITIZE OFF)

add_compile_definitions(QT_FATAL_WARNINGS=1)


set(OpenGL_GL_PREFERENCE GLVND)

set(QT_MAJOR_VERSION="${QT_MAJOR_VERSION}")

find_package(OpenGL REQUIRED)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

message(QT_MAJOR_VERSION="${QT_MAJOR_VERSION}")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        pointcloudrenderer.cpp
        pointcloudrenderer.h
        pointcloudrenderer.ui
        pointcloudrenderer.h pointcloudrenderer.cpp pointcloudrenderer.ui
        myfreenectdevice.h myfreenectdevice.cpp
        videoplayer.h videoplayer.cpp videoplayer.ui
        aspectratiopixmaplabel.h aspectratiopixmaplabel.cpp aspectratiopixmaplabel.ui
        kinect_types.h kinect_types.cpp
        capturelist.h capturelist.cpp capturelist.ui
        capturepreview.h capturepreview.cpp capturepreview.ui

)
message(QT_MAJOR_VERSION="${QT_MAJOR_VERSION}")



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KinectToolBox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        filtering.h filtering.cpp
        Filtering/filterslot.h Filtering/filterslot.cpp Filtering/filterslot.ui
        Filtering/componentbinaryexpression.h Filtering/componentbinaryexpression.cpp Filtering/componentbinaryexpression.ui
        Filtering/andholder.h Filtering/andholder.cpp Filtering/andholder.ui
        Filtering/filterwidget.h Filtering/filterwidget.cpp Filtering/filterwidget.ui
        Filtering/orholder.h Filtering/orholder.cpp Filtering/orholder.ui
        threadworker.h threadworker.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET KinectToolBox APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(KinectToolBox SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(KinectToolBox
            ${PROJECT_SOURCES}
        )
    endif()
endif()



target_include_directories(KinectToolBox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(KinectToolBox PRIVATE "/usr/include/libfreenect")
target_include_directories(KinectToolBox PRIVATE "/usr/include/libusb-1.0")
target_include_directories(KinectToolBox PRIVATE /usr/include/pcl-1.14 /usr/include/eigen3)

target_link_directories(KinectToolBox PRIVATE /usr/lib/libfreenect)
target_link_libraries(KinectToolBox PRIVATE pcl_io pcl_common)

target_link_libraries(KinectToolBox PRIVATE Qt::Widgets)
target_link_libraries(KinectToolBox PRIVATE Qt::OpenGLWidgets)
target_link_libraries(KinectToolBox PRIVATE OpenGL::GL OpenGL::GLU )
target_link_libraries(KinectToolBox PRIVATE freenect)

target_link_libraries(KinectToolBox PRIVATE tbb)

add_definitions(${PCL_DEFINITIONS})



if (${ADDRESS_SANITIZE})
    target_link_options(KinectToolBox  PUBLIC -fsanitize=address)
    target_compile_options(KinectToolBox PUBLIC  -fsanitize=address)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.KinectToolBox)
endif()

set_target_properties(KinectToolBox PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS KinectToolBox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(KinectToolBox)
endif()
message(QT_MAJOR_VERSIONLAT="${QT_MAJOR_VERSION}")
