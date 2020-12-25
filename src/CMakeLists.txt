# project name
project(ray-tracer CXX)

# ------------------------------------------------------------------------------
# find sdl2 package : required for sdl2_canvas implementation
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

# ------------------------------------------------------------------------------
# these files make the core of the ray-tracer. compiled into a static
# library, and then linked as required to produce different executables.
set (RAYTRACER_LIB_SOURCES
  constants.hpp
  utils.hpp
  tuple.hpp
  tuple.cpp
  color.hpp
  color.cpp
  canvas.hpp
  canvas.cpp
  canvas_ppm_writer.cpp
  canvas_ppm_reader.cpp
  matrix.hpp
  matrix.cpp
  matrix_transformations.hpp
  matrix_transformations.cpp
  ray.hpp
  ray.cpp
  shape_interface.hpp
  shape_interface.cpp
  sphere.hpp
  sphere.cpp
  intersection_record.hpp
  intersection_record.cpp
  point_light.hpp
  point_light.cpp
  material.hpp
  material.cpp
  phong_illumination.hpp
  phong_illumination.cpp
  world.hpp
  world.cpp
  intersection_info.hpp
  intersection_info.cpp
  camera.hpp
  camera.cpp
  plane.hpp
  plane.cpp
  pattern_interface.hpp
  pattern_interface.cpp
  binary_pattern.hpp
  solid_pattern.hpp
  striped_pattern.hpp
  gradient_pattern.hpp
  ring_pattern.hpp
  checkers_pattern.hpp
  gradient_ring_pattern.hpp
  blended_pattern.hpp
  perlin_noise.hpp
  perlin_noise_pattern.hpp
  texture_2d_pattern.hpp
  uv_pattern_interface.hpp
  uv_point.cpp
  uv_point.hpp
  uv_checkers.hpp
  uv_noise_texture.hpp)

add_library(raytracer_lib STATIC ${RAYTRACER_LIB_SOURCES})
target_include_directories(raytracer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(raytracer_lib PRIVATE common ${SDL2_LIBRARIES})

# ------------------------------------------------------------------------------
# these files make the core of rendering routines in the raytracer
set (RAYTRACER_RENDER_LIB_SOURCES
  raytracer_renderer.hpp
  single_threaded_renderer.cpp
  multi_threaded_renderer.cpp)

add_library(raytracer_render_lib STATIC ${RAYTRACER_RENDER_LIB_SOURCES})
target_include_directories(raytracer_render_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(raytracer_render_lib PRIVATE concurrentqueue)
target_link_libraries(raytracer_render_lib PRIVATE pthread)
target_link_libraries(raytracer_render_lib PRIVATE common)

# ------------------------------------------------------------------------------
# chapter-02: drawing on a canvas.
#
# use drawing primitives to render path of cannonball from a virtual cannon.
add_executable(ch-02-virtual-cannon virtual_cannon_main.cpp virtual_cannon.hpp virtual_cannon.cpp)
target_link_libraries(ch-02-virtual-cannon PRIVATE common)
target_link_libraries(ch-02-virtual-cannon PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# chapter-04: matrix-transformations.
#
# use matrices + matrix-transformations to draw a clock-dial
add_executable(ch-04-clock-dial clock_dial_matxforms_main.cpp)
target_link_libraries(ch-04-clock-dial PRIVATE common)
target_link_libraries(ch-04-clock-dial PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# chapter-05: ray-sphere intersections.
#
# use ray-sphere intersection to draw silhouette of a sphere
add_executable(ch-05-sphere-silhouette sphere_silhouette_main.cpp)
target_link_libraries(ch-05-sphere-silhouette PRIVATE common)
target_link_libraries(ch-05-sphere-silhouette PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# chapter-06: light and shading.
#
# use phong-illumination to draw a realistic sphere. this is the
# single-threaded version
add_executable(ch-06-st-phong-sphere st_phong_sphere_main.cpp)
target_link_libraries(ch-06-st-phong-sphere PRIVATE common)
target_link_libraries(ch-06-st-phong-sphere PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# chapter-06: light and shading.
#
# use phong-illumination to draw a realistic sphere. this is the
# multi-threaded version
add_executable(ch-06-mt-phong-sphere mt_phong_sphere_main.cpp)
target_link_libraries(ch-06-mt-phong-sphere PRIVATE common)
target_link_libraries(ch-06-mt-phong-sphere PRIVATE raytracer_lib)
target_link_libraries(ch-06-mt-phong-sphere PRIVATE concurrentqueue)
target_link_libraries(ch-06-mt-phong-sphere PRIVATE pthread)

# ------------------------------------------------------------------------------
# chapter-07: making a scene; chapter-08: shadows
#
# render a scene with multiple objects, light sources and shadows.
#
# both single and multi threaded rendering is supported.
add_executable(ch-08-making-a-scene render_world_main.cpp)
target_link_libraries(ch-08-making-a-scene PRIVATE common)
target_link_libraries(ch-08-making-a-scene PRIVATE raytracer_lib)
target_link_libraries(ch-08-making-a-scene PRIVATE raytracer_render_lib)

# ------------------------------------------------------------------------------
# chapter-09: render with planes
#
# render a scene with multiple objects, light sources and shadows using planes
#
# both single and multi threaded rendering is supported.
add_executable(ch-09-render-with-plane render_with_plane.cpp)
target_link_libraries(ch-09-render-with-plane PRIVATE common)
target_link_libraries(ch-09-render-with-plane PRIVATE raytracer_lib)
target_link_libraries(ch-09-render-with-plane PRIVATE raytracer_render_lib)

# ------------------------------------------------------------------------------
# chapter-10: render with patterns
#
# render a scene with multiple objects, light sources and shadows
# using planes and different patterns
#
# both single and multi threaded rendering is supported.
add_executable(ch-10-render-with-patterns render_with_patterns.cpp)
target_link_libraries(ch-10-render-with-patterns PRIVATE common)
target_link_libraries(ch-10-render-with-patterns PRIVATE raytracer_lib)
target_link_libraries(ch-10-render-with-patterns PRIVATE raytracer_render_lib)

# ------------------------------------------------------------------------------
# chapter-10: render textures
#
# render a scene with perlin noise
#
# both single and multi threaded rendering is supported.
add_executable(ch-10-render-textures render_textures.cpp)
target_link_libraries(ch-10-render-textures PRIVATE common)
target_link_libraries(ch-10-render-textures PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# chapter-10: render with patterns
#
# render a scene with multiple objects, light sources and shadows
# using planes and different patterns
#
# both single and multi threaded rendering is supported.
add_executable(ch-10-render-with-textures render_with_textures.cpp)
target_link_libraries(ch-10-render-with-textures PRIVATE common)
target_link_libraries(ch-10-render-with-textures PRIVATE raytracer_lib)
target_link_libraries(ch-10-render-with-textures PRIVATE raytracer_render_lib)

