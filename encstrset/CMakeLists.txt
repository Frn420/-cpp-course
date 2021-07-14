cmake_minimum_required(VERSION 3.17)
project(encstrset)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        encstrset.cc
        encstrset.h
        encstrset_test1.c)

add_executable(encstrset ${SOURCE_FILES} encstrset_test1.c)