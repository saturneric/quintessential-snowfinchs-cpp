aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(model SHARED ${SRC_FILES})
target_precompile_headers(model PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>")

# speed up build
if(BUILD_UNITY)
  set_target_properties(model PROPERTIES UNITY_BUILD ON)
endif()