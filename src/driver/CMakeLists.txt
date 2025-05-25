aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(driver SHARED ${SRC_FILES})
target_compile_definitions(driver PRIVATE pch_EXPORTS)
target_precompile_headers(driver REUSE_FROM pch)
target_link_libraries(driver PRIVATE core handlers)