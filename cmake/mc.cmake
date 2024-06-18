function(add_messages source_file default_lang output_var)
    cmake_path(GET source_file STEM source_name)
    if(WIN32)
        set(output_name "${source_name}.rc")
        add_custom_command(
            OUTPUT ${source_name}.rc ${source_name}.h
                ${source_name}_${default_lang}.bin
            COMMAND mc.exe
            ARGS
                -h ${CMAKE_CURRENT_BINARY_DIR}
                -r ${CMAKE_CURRENT_BINARY_DIR}
                -n ${CMAKE_CURRENT_SOURCE_DIR}/${source_file}
            MAIN_DEPENDENCY
                ${CMAKE_CURRENT_SOURCE_DIR}/${source_file})
    else()
        set(output_name "${source_name}_${default_lang}.c")
        add_custom_command(
            OUTPUT ${output_name}
            COMMAND python3
            ARGS
                ${CMAKE_CURRENT_LIST_DIR}/../tools/mconv.py
                -o ${CMAKE_CURRENT_BINARY_DIR}/${output_name}
                ${CMAKE_CURRENT_SOURCE_DIR}/${source_file}
                0x${default_lang}
            MAIN_DEPENDENCY
                ${CMAKE_CURRENT_SOURCE_DIR}/${source_file})
    endif()

    set_source_files_properties(${output_name} PROPERTIES GENERATED TRUE)
    set(${output_var} ${output_name} PARENT_SCOPE)
endfunction()
