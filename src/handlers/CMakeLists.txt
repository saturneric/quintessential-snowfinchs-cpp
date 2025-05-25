aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(handlers SHARED ${SRC_FILES})
target_compile_definitions(handlers PRIVATE pch_EXPORTS)
target_precompile_headers(handlers REUSE_FROM pch)
target_link_libraries(handlers PRIVATE model)