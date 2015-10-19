cmake_minimum_required(VERSION 3.3)
project(liquibook)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("/Users/chao/ClionProjects/boost_1_58_0/include")
include_directories("/Users/chao/ClionProjects/liquibook/src/")
set(SOURCE_FILES main.cpp)
add_executable(liquibook ${SOURCE_FILES})