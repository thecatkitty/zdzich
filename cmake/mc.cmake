set(SCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR})

function(concatenate_files FIRST SECOND OUTPUT)
    add_custom_command(
        OUTPUT messages.mc
        COMMAND ${CMAKE_COMMAND} -E cat
            ${CMAKE_CURRENT_SOURCE_DIR}/${FIRST}
            ${CMAKE_CURRENT_SOURCE_DIR}/${SECOND}
            > ${OUTPUT}
        DEPENDS ${FIRST} ${SECOND})
endfunction()

function(add_messages source_file bin_prefix languages output_var)
    cmake_path(GET source_file STEM source_name)
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_BINARY_DIR
        BASE_DIRECTORY ${CMAKE_BINARY_DIR}
        OUTPUT_VARIABLE binary_dir)

    if(WIN32)
        set(output_name "${source_name}.rc")
        set(binary_names ${languages})
        list(TRANSFORM binary_names REPLACE "^(....)$" "${bin_prefix}_\\1.bin")
        add_custom_command(
            OUTPUT ${source_name}.rc ${source_name}.h ${binary_names}
            COMMAND mc.exe
            ARGS
                -h ${CMAKE_CURRENT_BINARY_DIR}
                -r ${CMAKE_CURRENT_BINARY_DIR}
                -n ${source_file}
            MAIN_DEPENDENCY
                ${source_file}
            COMMENT "Building message resource ${binary_dir}/${output_name}")

        set_source_files_properties(${output_name} PROPERTIES GENERATED TRUE)
        list(APPEND output_names ${output_name})
    else()
        foreach(language IN LISTS languages)
            if(DOS)
                set(dos_flag "--dos")
            endif()
            set(output_name "${source_name}_${language}.c")
            add_custom_command(
                OUTPUT ${output_name}
                COMMAND python3
                ARGS
                    ${SCRIPTS_DIR}/../tools/mconv.py
                    -o ${CMAKE_CURRENT_BINARY_DIR}/${output_name}
                    ${source_file}
                    0x${language}
                    ${dos_flag}
                MAIN_DEPENDENCY
                    ${source_file}
                COMMENT "Converting localized messages ${binary_dir}/${output_name}")

            set_source_files_properties(${output_name} PROPERTIES GENERATED TRUE)
            list(APPEND output_names ${output_name})
        endforeach()
    endif()

    set(${output_var} ${output_names} PARENT_SCOPE)
endfunction()
