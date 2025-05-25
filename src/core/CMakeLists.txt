aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(core SHARED ${SRC_FILES})
target_link_libraries(core PRIVATE pch model)
