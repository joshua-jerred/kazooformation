project(kazooformation)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kazoo_translation_layer INTERFACE)
target_sources(kazoo_translation_layer INTERFACE
        src/kazoo_translation_layer/binary_stream.cpp
)
target_include_directories(kazoo_translation_layer INTERFACE src)

add_executable(kazoo_connect
        src/main.cpp
)

# ==== Unit tests ====
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

# lazy globbing (not best practice /shrug)
file(GLOB kazoo_translation_layer_test_sources "tests/kazoo_translation_layer/*.cpp")
add_executable(kazoo_translation_layer_test ${kazoo_translation_layer_test_sources})
target_link_libraries(kazoo_translation_layer_test
        GTest::GTest
        GTest::Main
        kazoo_translation_layer
)
gtest_discover_tests(kazoo_translation_layer_test)
target_include_directories(kazoo_translation_layer_test PRIVATE tests)