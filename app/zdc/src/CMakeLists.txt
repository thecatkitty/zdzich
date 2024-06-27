target_sources(zdc PRIVATE zdc.cpp)

if(NOT WIN32)
    target_sources(zdc PRIVATE translations.cpp)
endif()

if(DOS)
    target_link_libraries(zdc PRIVATE rstd)
endif()
