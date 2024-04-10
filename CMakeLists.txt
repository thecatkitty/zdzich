cmake_minimum_required(VERSION 3.16)

project(Zdzich CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(libzdstd)
add_subdirectory(sample)
