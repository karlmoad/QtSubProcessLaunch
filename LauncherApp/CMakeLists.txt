cmake_minimum_required(VERSION 3.16)
project(LauncherApp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if (DEFINED ENV{QT_DIR})
    message(STATUS "QT CMAKE DIR....$ENV{QT_DIR}")
else()
    message(FATAL_ERROR "QT_DIR environment variable is unset. \n  ")
endif()

set(CMAKE_PREFIX_PATH $ENV{QT_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

add_executable(LauncherApp MACOSX_BUNDLE main.cpp MainWindow.cpp MainWindow.h Engine.cpp Engine.h) #HttpRequest.cpp HttpRequest.h)

target_link_libraries(LauncherApp Qt5::Core Qt5::Widgets Qt5::Network)