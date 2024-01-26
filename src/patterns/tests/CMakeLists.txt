# ------------------------------------------------------------------------------
# list of all files making up tests in this directory. each file is a
# test executable
SET(RT_PATTERN_TEST_SOURCES
  pattern_test.cpp
  reflection_refraction_test.cpp
  uv_pattern_test.cpp
)

# ------------------------------------------------------------------------------
# generate + run the test targets
include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeBuildTestFunctions)
generate_all_test_targets("${RT_PATTERN_TEST_SOURCES}")
run_all_tests("${RT_PATTERN_TEST_SOURCES}")
