add_subdirectory(model)
add_subdirectory(core)
add_subdirectory(handlers)
add_subdirectory(driver)

add_executable(compiler L4Compiler.cpp)
target_link_libraries(compiler PRIVATE driver)