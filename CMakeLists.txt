cmake_minimum_required(VERSION 3.5)

project(KinectToolBox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


add_compile_definitions(QT_FATAL_WARNINGS=1)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)



set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        pointcloudrenderer.cpp
        pointcloudrenderer.h
        pointcloudrenderer.ui
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KinectToolBox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        pointcloudrenderer.h pointcloudrenderer.cpp pointcloudrenderer.ui
        myfreenectdevice.h myfreenectdevice.cpp
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



target_include_directories(KinectToolBox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(KinectToolBox PUBLIC "/usr/include/libfreenect")
target_include_directories(KinectToolBox PUBLIC "/usr/include/libusb-1.0")

target_link_libraries(KinectToolBox PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


target_link_libraries(KinectToolBox PRIVATE Qt::OpenGLWidgets)
target_link_libraries(KinectToolBox PRIVATE OpenGL::GL OpenGL::GLU )
target_link_directories(KinectToolBox PUBLIC /usr/lib/libfreenect)
target_link_libraries(KinectToolBox PRIVATE freenect)
target_link_options(KinectToolBox  PUBLIC -fsanitize=address)
target_compile_options(KinectToolBox PUBLIC  -fsanitize=address)

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
