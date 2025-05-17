# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://ftp.bktus.com/GoogleTest/googletest-1.17.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_FILES)

add_executable(compiler_test ${TEST_FILES})
target_precompile_headers(compiler_test PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>")
target_link_libraries(compiler_test PRIVATE core GTest::gtest)

include(GoogleTest)
gtest_discover_tests(compiler_test)
