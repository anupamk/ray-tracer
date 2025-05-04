# ------------------------------------------------------------------------------
# various shapes supported by the ray tracer
add_library(rt_shapes SHARED
  cone.cpp
  cone.hpp
  csg.cpp
  csg.hpp
  cube.cpp
  cube.hpp
  cylinder.cpp
  cylinder.hpp
  group.cpp
  group.hpp
  plane.cpp
  plane.hpp
  shape_interface.cpp
  shape_interface.hpp
  sphere.cpp
  sphere.hpp
  triangle.cpp
  triangle.hpp
  aabb.hpp
  aabb.cpp)

target_link_libraries(rt_shapes

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils
  PRIVATE rt_utils
  PRIVATE rt_primitives)

augment_execution_profiling(rt_shapes)

target_include_directories(rt_shapes PUBLIC ${CMAKE_SOURCE_DIR}/src)

# ------------------------------------------------------------------------------
# build tests
add_subdirectory(tests)
