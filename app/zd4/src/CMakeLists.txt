target_sources(zd4 PRIVATE main.cpp)
target_sources(zd4 PRIVATE arguments.cpp)
target_sources(zd4 PRIVATE clear.c)
target_sources(zd4 PRIVATE list_zdi.c)

if(NOT WIN32)
    target_sources(zd4 PRIVATE translations.cpp)
endif()

if(DOS)
    target_link_libraries(zd4 PRIVATE -li86)
    target_link_libraries(zd4 PRIVATE rstd)
endif()
