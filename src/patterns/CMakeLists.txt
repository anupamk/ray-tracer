# ------------------------------------------------------------------------------
# raytracer supported patterns
add_library(rt_patterns SHARED
  align_check_pattern.hpp
  binary_pattern.hpp
  blended_pattern.hpp
  checkers_pattern.hpp
  cube_map_texture.cpp
  cube_map_texture.hpp
  gradient_pattern.hpp
  gradient_perlin_noise_pattern.hpp
  gradient_ring_pattern.hpp
  material.cpp
  material.hpp
  pattern_interface.cpp
  pattern_interface.hpp
  perlin_noise.hpp
  perlin_noise_pattern.hpp
  ring_pattern.hpp
  solid_pattern.hpp
  stock_materials.cpp
  stock_materials.hpp
  striped_pattern.hpp
  texture_2d_pattern.hpp
  uv_checkers.hpp
  uv_cube_map.hpp
  uv_image_pattern.hpp
  uv_mapper.cpp
  uv_mapper.hpp
  uv_noise_texture.hpp
  uv_pattern_interface.hpp)

target_link_libraries(rt_patterns

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils
  PRIVATE rt_utils
  PRIVATE rt_primitives
  PRIVATE rt_shapes)

augment_execution_profiling(rt_patterns)

target_include_directories(rt_patterns PUBLIC ${CMAKE_SOURCE_DIR}/src)



