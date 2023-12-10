
cmake_minimum_required(VERSION 3.14)

project(lesson_1b LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)

add_executable(lesson_1b
  main.cpp
  mainwindow.h mainwindow.cpp
  glwidget.h glwidget.cpp
  shaderprogram.cpp shaderprogram.h
  resources.qrc
)
target_link_libraries(lesson_1b PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
)

install(TARGETS lesson_1b
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
