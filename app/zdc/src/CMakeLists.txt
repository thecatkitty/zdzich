target_sources(zdc PRIVATE zdc.cpp)

if(DOS)
    target_link_libraries(zdc PRIVATE rstd)
endif()
