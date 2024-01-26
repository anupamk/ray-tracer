# ------------------------------------------------------------------------------
# list of all files making up tests in this directory. each file is a
# test executable
SET(RT_IO_TEST_SOURCES
  canvas_test.cpp
  phong_illumination_test.cpp
  world_test.cpp
  camera_test.cpp
  obj_file_parser_test.cpp
)

# ------------------------------------------------------------------------------
# generate + run the test targets
include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeBuildTestFunctions)
generate_all_test_targets("${RT_IO_TEST_SOURCES}")
run_all_tests("${RT_IO_TEST_SOURCES}")
