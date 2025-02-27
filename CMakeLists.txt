cmake_minimum_required(VERSION 3.16)
project(qt_simple_interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(qt_simple_interface
    main.cpp
    mainwindow.h
    mainwindow.cpp
)

target_link_libraries(qt_simple_interface PRIVATE Qt6::Widgets) 