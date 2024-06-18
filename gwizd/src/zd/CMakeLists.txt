add_subdirectory(gen)
add_subdirectory(lex)
add_subdirectory(par)
add_subdirectory(text)

target_sources(gwizd PRIVATE error.cpp)
target_sources(gwizd PRIVATE ustring.cpp)

if (WIN32)
	target_sources(gwizd PRIVATE messagew.cpp)
else()
	target_sources(gwizd PRIVATE messagep.cpp)
endif()
