add_executable(zdc)
target_include_directories(zdc PRIVATE include)
target_include_directories(zdc PRIVATE ../ext/expected/include)
set_property(TARGET zdc PROPERTY CXX_STANDARD 14)

add_subdirectory(src)

if(DOS)
    target_link_options(zdc PRIVATE -Wl,--Map=zdc.map)
endif()

if(WIN32)
	add_custom_command(
		OUTPUT zdc.rc zdc.h
			zdc_en_US.bin
		COMMAND mc.exe
		ARGS
			-h ${CMAKE_CURRENT_BINARY_DIR}
			-r ${CMAKE_CURRENT_BINARY_DIR}
			-n ${CMAKE_CURRENT_SOURCE_DIR}/src/zdc.mc
		MAIN_DEPENDENCY
			${CMAKE_CURRENT_SOURCE_DIR}/src/zdc.mc)

	set_source_files_properties(zdc.rc PROPERTIES GENERATED TRUE)
	target_sources(zdc PRIVATE zdc.rc)
endif()

install(TARGETS zdc DESTINATION .)
