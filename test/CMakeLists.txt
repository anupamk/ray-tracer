# project name
project(ray-tracer-tests CXX)

add_compile_options(
  -Wno-unused-variable
  -Wno-unused-but-set-variable
)

# files making up the tests
set (RAYTRACER_TEST_SOURCES
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
  plane_test.cpp)

# what does this build ?
add_executable(ray-tracer-test doctest_main.cpp ${RAYTRACER_TEST_SOURCES})
target_link_libraries(ray-tracer-test PRIVATE raytracer_lib doctest common)
