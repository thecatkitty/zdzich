cmake_minimum_required(VERSION 3.16)

project(Zdzich CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

if(DOS)
    add_compile_options(-march=i8088)
    add_compile_options(-Os)
    add_compile_options(-fno-exceptions)
    add_compile_options(-fno-rtti)
endif()

add_subdirectory(libzdstd)
add_subdirectory(sample)
add_subdirectory(zdc)
