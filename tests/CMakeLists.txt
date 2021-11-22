# ------------------------------------------------------------------------------
# list of all files making up the tests in this directory
SET(RT_TEST_SRC_FILES
  tuple_test.cpp
  color_test.cpp
  canvas_test.cpp
  matrix_test.cpp
  matrix_transformations_test.cpp
  ray_test.cpp
  sphere_test.cpp
  ray_transform_test.cpp
  point_light_test.cpp
  phong_illumination_test.cpp
  world_test.cpp
  camera_test.cpp
  plane_test.cpp
  pattern_test.cpp
  reflection_refraction_test.cpp
  cube_test.cpp
  uv_pattern_test.cpp
  cylinder_test.cpp
  cone_test.cpp)

# ------------------------------------------------------------------------------
# no user serviceable parts here ↓

# project name
project(ray-tracer-tests CXX)

add_compile_options(
  -Wno-unused-variable
  -Wno-unused-but-set-variable
)

# ------------------------------------------------------------------------------
# generate a single test-target based on the incoming file-name. test
# executable is called 'rt_<test-file-name>'
function (generate_one_test_target FNAME)

  get_filename_component(FNAME_SANS_EXT ${FNAME} NAME_WLE)
  set(TEST_NAME "rt_${FNAME_SANS_EXT}")

  add_executable(${TEST_NAME} ${FNAME} doctest_main.cpp)
  target_link_libraries(${TEST_NAME}
    PRIVATE rt-lib
    PRIVATE doctest)

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY tests)

endfunction()

# ------------------------------------------------------------------------------
# generate all test-targets based on list of files making up the
# tests. each file corresponds to 1 unique executable
function (generate_all_test_targets FNAME_LIST)
  foreach (FNAME ${FNAME_LIST})
    generate_one_test_target(${FNAME})
  endforeach()
endfunction()

# ------------------------------------------------------------------------------
# do-it: generate all the test targets
generate_all_test_targets("${RT_TEST_SRC_FILES}")
