add_library(pch SHARED pch.cpp)
target_precompile_headers(pch PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>")