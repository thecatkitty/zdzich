add_executable(zdc)
set_property(TARGET zdc PROPERTY CXX_STANDARD 14)
target_link_libraries(zdc PRIVATE gwizd)

add_subdirectory(src)

if(DOS)
    target_link_options(zdc PRIVATE -Wl,--Map=zdc.map)
endif()

concatenate_files(zdc.mc ../errors.mc messages.mc)
add_messages(${CMAKE_CURRENT_BINARY_DIR}/messages.mc zdc "0409;0415" messages)
target_sources(zdc PRIVATE ${messages})

install(TARGETS zdc DESTINATION .)
