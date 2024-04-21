add_executable(zdc)
target_include_directories(zdc PRIVATE include)
set_property(TARGET zdc PROPERTY CXX_STANDARD 14)

add_subdirectory(src)

install(TARGETS zdc DESTINATION .)
