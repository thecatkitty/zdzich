target_sources(zdc PRIVATE zdc.cpp)

if(DOS)
    target_sources(zdc PRIVATE rstd.c)
endif()