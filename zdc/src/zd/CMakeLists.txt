add_subdirectory(gen)
add_subdirectory(lex)
add_subdirectory(par)
add_subdirectory(text)

target_sources(zdc PRIVATE error.cpp)
target_sources(zdc PRIVATE ustring.cpp)
