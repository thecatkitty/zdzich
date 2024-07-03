add_executable(zd4)
set_property(TARGET zd4 PROPERTY CXX_STANDARD 14)
target_link_libraries(zd4 PRIVATE gwizd)
add_dependencies(zd4 version_info)

add_subdirectory(src)

if(DOS)
    target_link_options(zd4 PRIVATE -Wl,--Map=zd4.map)
endif()

concatenate_files(zd4.mc ../errors.mc messages.mc)
add_messages(${CMAKE_CURRENT_BINARY_DIR}/messages.mc zd4 "0415" messages)
target_sources(zd4 PRIVATE ${messages})

install(TARGETS zd4 DESTINATION .)
