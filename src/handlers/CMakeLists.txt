aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(handlers SHARED ${SRC_FILES})
target_compile_definitions(handlers PRIVATE model_EXPORTS)
target_precompile_headers(handlers REUSE_FROM model)
target_link_libraries(handlers PRIVATE model)

# speed up build
if(BUILD_UNITY)
  set_target_properties(handlers PROPERTIES UNITY_BUILD ON)
endif()
