add_executable(lek05-03)
set_property(TARGET lek05-03 PROPERTY CXX_STANDARD 14)
target_link_libraries(lek05-03 zdstd)
target_sources(lek05-03 PRIVATE KURS/SAMPLE/LEKCJA05/lek05-03.cpp)

if (NOT DOS)
    function(add_zdc_program target source_file)
        list(REMOVE_AT ARGV 0 1)
        cmake_parse_arguments(ZDC_ARGS "" "ENCODING" "" ${ARGN})
        if(ZDC_ARGS_ENCODING)
            set(ZDC_ENCODING -e:${ZDC_ARGS_ENCODING})
        endif()

        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.com
            COMMAND $<TARGET_FILE:zdc>
                ${CMAKE_CURRENT_SOURCE_DIR}/${source_file}
                -o:${CMAKE_CURRENT_BINARY_DIR}/${target}.com
                ${ZDC_ENCODING}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            MAIN_DEPENDENCY ${source_file}
            DEPENDS zdc)

        add_custom_target(
            ${target} ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}.com zdc)
    endfunction()

    add_zdc_program(idzwiek2 idzwiek2.zdi)
    add_zdc_program(iklawiat iklawiat.zdi)
    add_zdc_program(imysz imysz.zdi)

    add_subdirectory(INC/SAVER)
    add_subdirectory(KURS)
    add_subdirectory(PRZYKLAD)
endif()
