cmake_minimum_required(VERSION 3.8)
project(emeocv)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Config.cpp
        Config.h
        Directory.cpp
        Directory.h
        ImageInput.cpp
        ImageInput.h
        ImageProcessor.cpp
        ImageProcessor.h
        KNearestOcr.cpp
        KNearestOcr.h
        main.cpp
        MySQLDatabase.cpp
        MySQLDatabase.h
        Plausi.cpp
        Plausi.h)

add_executable(emeocv ${SOURCE_FILES})