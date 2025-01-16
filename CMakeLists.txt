project(kazooformation)
cmake_minimum_required(VERSION 3.17)

add_library(kazoo_translation_layer INTERFACE)
target_sources(kazoo_translation_layer INTERFACE
  # src/translation_layer.cpp
)
target_include_directories(kazoo_translation_layer INTERFACE src)

add_executable(kazoo_connect
  src/main.cpp
)

# ==== Unit tests ====
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB unit_test_sources "tests/*.cpp")
add_executable(kazooformation_test ${unit_test_sources})
target_link_libraries(kazooformation_test 
  GTest::GTest
  GTest::Main
  kazoo_translation_layer
)
gtest_discover_tests(kazooformation_test)