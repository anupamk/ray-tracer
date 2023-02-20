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
  triangle.hpp)

target_link_libraries(rt_shapes
  PRIVATE common_utils
  PRIVATE rt_utils
  PRIVATE rt_primitives)

augment_execution_profiling(rt_shapes)

target_include_directories(rt_shapes PUBLIC ${CMAKE_SOURCE_DIR}/src)

