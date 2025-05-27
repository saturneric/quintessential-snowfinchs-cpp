aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(driver SHARED ${SRC_FILES})
target_compile_definitions(driver PRIVATE model_EXPORTS)
target_precompile_headers(driver REUSE_FROM model)
target_link_libraries(driver PRIVATE core handlers)

# speed up build
if(BUILD_UNITY)
  set_target_properties(driver PROPERTIES UNITY_BUILD ON)
endif()