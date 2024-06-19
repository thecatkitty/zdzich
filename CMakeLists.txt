cmake_minimum_required(VERSION 3.20)

project(Zdzich CXX C)

# Compiler dependent tweaks
if(MSVC)
    add_compile_options(/utf-8)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-switch)
endif()

if(DOS)
    add_compile_options(-march=i8088)
    add_compile_options(-mcmodel=small)
    add_compile_options(-mnewlib-nano-stdio)
    add_compile_options(-Os)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

    add_link_options(-mcmodel=small)
    add_link_options(-mnewlib-nano-stdio)

    include_directories(BEFORE SYSTEM ext/nicetia16/include/c++/6.3.0)
    link_directories(BEFORE ext/nicetia16/lib)
endif()

include(cmake/mc.cmake)

# Subprojects
add_subdirectory(app)
add_subdirectory(gwizd)

if(NOT DOS)
    add_subdirectory(libzdstd)
    add_subdirectory(sample)
endif()
