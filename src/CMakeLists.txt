aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(core SHARED ${SRC_FILES})
target_precompile_headers(core PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>")