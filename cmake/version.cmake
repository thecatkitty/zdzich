execute_process(
    COMMAND git describe --abbrev=0
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
    COMMAND git rev-list ${GIT_TAG}..HEAD --count
    OUTPUT_VARIABLE GIT_COMMITS
    OUTPUT_STRIP_TRAILING_WHITESPACE)

if(GIT_TAG STREQUAL "")
    set(GIT_TAG "6.4.0")
    set(GIT_COMMITS "0")
endif()

string(REPLACE "." "," VER_FILEVERSION "${GIT_TAG}.${GIT_COMMITS}")
if(GIT_COMMITS STREQUAL "0")
    set(VER_FILEVERSION_STR "${GIT_TAG}")
else()
    set(VER_FILEVERSION_STR "${GIT_TAG}-${GIT_COMMITS}")
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/version.h.in inc/generated/version.h @ONLY)
