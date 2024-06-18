add_library(gwizd)
target_include_directories(gwizd PUBLIC include)
target_include_directories(gwizd PUBLIC ../ext/expected/include)
set_property(TARGET gwizd PROPERTY CXX_STANDARD 14)

add_subdirectory(src)
