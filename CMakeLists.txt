cmake_minimum_required(VERSION 4.1)
project(connect4_ai)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto=auto -DNDEBUG -fopenmp -pthread")

add_library(connect4Lib INTERFACE)
target_include_directories(connect4Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(connect4Lib INTERFACE ../tiny-dnn)

add_executable(connect4_ai connect4.cpp)

target_link_libraries(connect4_ai PRIVATE -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -fopenmp)
target_link_libraries(connect4_ai PRIVATE connect4Lib)
