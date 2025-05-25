aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(model SHARED ${SRC_FILES})
target_compile_definitions(model PRIVATE pch_EXPORTS)
target_precompile_headers(model REUSE_FROM pch)
