# project name
project(ray-tracer-tests CXX)

# files making up the tests
set (RAYTRACER_TEST_SOURCES
  tuple_test.cpp)

# what does this build ?
add_executable(ray-tracer-test doctest_main.cpp ${RAYTRACER_TEST_SOURCES})
target_link_libraries(ray-tracer-test PRIVATE raytracer_lib doctest)
