cmake_minimum_required(VERSION 3.28)

# TODO set up two projects, one to run main.cpp using C++11, and one to run main.cpp using C++17.
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(defer_tests VERSION 1.1.0 LANGUAGES CXX)
add_subdirectory(hng/defer)
add_executable(defer_tests src/main.cpp)

#target_compile_features(defer_tests PRIVATE cxx_std_17)
target_compile_features(defer_tests PRIVATE cxx_std_11)

target_link_libraries(defer_tests PRIVATE defer)

#target_include_directories(defers_tests PRIVATE include)

if(MSVC)
  target_compile_options(defer_tests PRIVATE /W4 /WX)
else()
  target_compile_options(defer_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
