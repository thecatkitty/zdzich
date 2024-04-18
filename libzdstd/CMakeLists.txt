add_library(zdstd)
target_include_directories(zdstd PUBLIC include)
set_property(TARGET zdstd PROPERTY CXX_STANDARD 14)

add_subdirectory(src)
