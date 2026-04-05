cmake_minimum_required(VERSION 3.16)
project(Sketch3D VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

add_executable(Sketch3D
    main.cpp
    mainwindow.cpp
    mainwindow.h
    sketchwidget.cpp
    sketchwidget.h
    shape.cpp
    shape.h
    viewer3D.cpp
    viewer3D.h
)

target_link_libraries(Sketch3D PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

if(WIN32)
    target_link_libraries(Sketch3D PRIVATE opengl32 glu32)
elseif(APPLE)
    find_library(OPENGL_LIB OpenGL REQUIRED)
    target_link_libraries(Sketch3D PRIVATE ${OPENGL_LIB})
else()
    target_link_libraries(Sketch3D PRIVATE GL GLU)
endif()

# Suppress deprecated OpenGL warnings on macOS
if(APPLE)
    target_compile_definitions(Sketch3D PRIVATE GL_SILENCE_DEPRECATION)
    target_compile_options(Sketch3D PRIVATE -Wno-deprecated-declarations)
endif()
