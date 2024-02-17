# ------------------------------------------------------------------------------
# list of all files making up tests in this directory. each file is a
# test executable
SET(RT_SHAPE_TEST_SOURCES
  sphere_test.cpp
  plane_test.cpp
  cube_test.cpp
  cylinder_test.cpp
  cone_test.cpp
  group_test.cpp
  triangle_test.cpp
  csg_test.cpp
)

# ------------------------------------------------------------------------------
# generate + run the test targets
include(BuildTestFunctions)
generate_all_test_targets("${RT_SHAPE_TEST_SOURCES}")
run_all_tests("${RT_SHAPE_TEST_SOURCES}")
