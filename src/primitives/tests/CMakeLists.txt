# ------------------------------------------------------------------------------
# list of all files making up tests in this directory. each file is a
# test executable
SET(RT_PRIMITIVES_TEST_SOURCES
  tuple_test.cpp
  color_test.cpp
  matrix_test.cpp
  matrix_transformations_test.cpp
  ray_test.cpp
  ray_transform_test.cpp
  point_light_test.cpp
)

# ------------------------------------------------------------------------------
# generate + run the test targets
include(BuildTestFunctions)
generate_all_test_targets("${RT_PRIMITIVES_TEST_SOURCES}")
run_all_tests("${RT_PRIMITIVES_TEST_SOURCES}")
