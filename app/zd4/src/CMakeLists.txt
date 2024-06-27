target_sources(zd4 PRIVATE zd4.cpp)

if(NOT WIN32)
    target_sources(zd4 PRIVATE translations.cpp)
endif()

if(DOS)
    target_link_libraries(zd4 PRIVATE rstd)
endif()
