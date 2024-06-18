add_executable(zdc)
set_property(TARGET zdc PROPERTY CXX_STANDARD 14)
target_link_libraries(zdc PRIVATE gwizd)

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
else()
	add_custom_command(
		OUTPUT zdc_en_US.c
		COMMAND python3
		ARGS
			${CMAKE_CURRENT_SOURCE_DIR}/../tools/mconv.py
			-o ${CMAKE_CURRENT_BINARY_DIR}/zdc_en_US.c
			${CMAKE_CURRENT_SOURCE_DIR}/src/zdc.mc
			1033
		MAIN_DEPENDENCY
			${CMAKE_CURRENT_SOURCE_DIR}/src/zdc.mc)

	set_source_files_properties(zdc_en_US.c PROPERTIES GENERATED TRUE)
	target_sources(zdc PRIVATE zdc_en_US.c)
endif()

install(TARGETS zdc DESTINATION .)
