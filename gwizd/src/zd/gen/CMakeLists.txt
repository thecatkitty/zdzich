target_sources(gwizd PRIVATE text_generator.cpp)
target_sources(gwizd PRIVATE x86_assembler.cpp)
target_sources(gwizd PRIVATE zd4_generator.cpp)
target_sources(gwizd PRIVATE zd4_section.cpp)

add_subdirectory(zd4_builtins)
