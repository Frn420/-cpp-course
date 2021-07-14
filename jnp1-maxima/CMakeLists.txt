cmake_minimum_required(VERSION 3.17)
project(jnp1_maxima)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(jnp1_maxima
        function_maxima.h
        test_damiana.cc)
