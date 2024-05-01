add_subdirectory(lex)
add_subdirectory(text)

target_sources(zdc PRIVATE error.cpp)
target_sources(zdc PRIVATE ustring.cpp)
