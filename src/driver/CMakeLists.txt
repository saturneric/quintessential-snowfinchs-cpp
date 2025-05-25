aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_library(driver SHARED ${SRC_FILES})
target_link_libraries(driver PRIVATE pch core handlers)