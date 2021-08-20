cmake_minimum_required(VERSION 3.5.1)

project("Part2")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14 -pthread")
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(EIGEN3_INCLUDE_DIR /dir/to/eigen/eigen-eigen-26667be4f70b)
set(Eigen3_DIR /dir/to/eigen eigen-eigen-26667be4f70b/cmake)

find_package(Qt5Widgets REQUIRED)
#find_package(Eigen3 REQUIRED)

include_directories(include ${EIGEN3_INCLUDE_DIR})

#add new classes
set(PART2_SOURCES
    "src/main.cpp"
    "src/EDFhandler.cpp"
    "src/volume.cpp"
    "src/Acquisition/pose.cpp"
    "src/Acquisition/detectorWidget.cpp"
    "src/Gantry.cpp"
    "src/Raytracer.cpp"
    "src/Acquisition/Acquisition.cpp"
    "src/Reconstruction/Reconstruction.cpp"
    "src/Reconstruction/normalReconstruction.cpp"
    "src/Reconstruction/regularizedReconstruction.cpp"
    "src/Reconstruction/sliceViewer.cpp"
    "src/Reconstruction/projection.cpp"
    "src/Visualization/Visualization.cpp"
    "src/Visualization/imageViewer.cpp"
    "src/Visualization/TransferFunction.cpp"
    "src/Visualization/rendering.cpp"
    "src/MainWindow.cpp")
set(PART2_HEADERS
    "include/EDFhandler.h"
    "include/IOutilities.h"
    "include/volume.h"
    "include/Acquisition/pose.h"
    "include/Acquisition/detectorWidget.h"
    "include/Gantry.h"
    "include/Raytracer.h"
    "include/Acquisition/Acquisition.h"
    "include/Reconstruction/Reconstruction.h"
    "include/Reconstruction/base.h"
    "include/Reconstruction/normalReconstruction.h"
    "include/Reconstruction/regularizedReconstruction.h"
    "include/Reconstruction/sliceViewer.h"
    "include/Reconstruction/projection.h"
    "include/Visualization/Visualization.h"
    "include/Visualization/imageViewer.h"
    "include/Visualization/TransferFunction.h"
    "include/Visualization/rendering.h"
    "include/MainWindow.h")



add_executable(part2 ${PART2_SOURCES} ${PART2_HEADERS})

qt5_use_modules(part2 Widgets)
