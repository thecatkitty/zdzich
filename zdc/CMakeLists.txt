add_executable(zdc)
target_include_directories(zdc PRIVATE include)
target_include_directories(zdc PRIVATE ../ext/expected/include)
set_property(TARGET zdc PROPERTY CXX_STANDARD 14)

add_subdirectory(src)

if(DOS)
    target_link_options(zdc PRIVATE -Wl,--Map=zdc.map)
endif()

install(TARGETS zdc DESTINATION .)
