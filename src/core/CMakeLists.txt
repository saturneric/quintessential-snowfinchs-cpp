aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(core SHARED ${SRC_FILES})
target_compile_definitions(core PRIVATE pch_EXPORTS)
target_precompile_headers(core REUSE_FROM pch)
target_link_libraries(core PRIVATE model)