# ------------------------------------------------------------------------------
# ray tracer primitives f.e color, ray, tuple etc. etc.
add_library(rt_primitives SHARED
  color.cpp
  color.hpp
  color_pallette.hpp
  intersection_info.cpp
  intersection_info.hpp
  intersection_record.cpp
  intersection_record.hpp
  matrix.cpp
  matrix.hpp
  matrix_transformations.cpp
  matrix_transformations.hpp
  point_light.cpp
  point_light.hpp
  ray.cpp
  ray.hpp
  tuple.cpp
  tuple.hpp
  uv_point.cpp
  uv_point.hpp)

target_link_libraries(rt_primitives
  PRIVATE common_utils
  PRIVATE rt_utils)

augment_execution_profiling(rt_primitives)

target_include_directories(rt_primitives PUBLIC ${CMAKE_SOURCE_DIR}/src)
