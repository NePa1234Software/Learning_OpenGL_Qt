
cmake_minimum_required(VERSION 3.14)

project(lesson_3a LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)

add_executable(lesson_3a
  main.cpp
  mainwindow.h mainwindow.cpp
  glwidget.h glwidget.cpp
  shaderprogram.cpp shaderprogram.h
  texture2D.cpp texture2D.h
  camera.cpp camera.h
  resources.qrc
)
target_link_libraries(lesson_3a PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
)

install(TARGETS lesson_3a
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
