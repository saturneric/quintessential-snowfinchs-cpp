aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(core SHARED ${SRC_FILES})
target_compile_definitions(core PRIVATE model_EXPORTS)
target_precompile_headers(core REUSE_FROM model)
target_link_libraries(core PRIVATE model)

# speed up build
if(BUILD_UNITY)
  set_target_properties(core PROPERTIES UNITY_BUILD ON)
endif()