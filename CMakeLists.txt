cmake_minimum_required(VERSION 3.12)
project(thread)

set(CMAKE_CXX_STANDARD 11)

add_executable(spp threader.cpp)
target_link_libraries(spp pthread)