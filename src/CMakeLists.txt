# ------------------------------------------------------------------------------
# build everything with these options
add_compile_options(
  -fdiagnostics-color=never
  -ffast-math
  -fno-omit-frame-pointer
  -rdynamic
  -g
  -O2
  -pipe
  -std=c++17
  -Wall
  -Werror
  -march=native
  -lm
  -lpthread
  -ldl
  )

# ------------------------------------------------------------------------------
# find sdl2 package : required for sdl2_canvas implementation
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

# ------------------------------------------------------------------------------
# rt-lib: implement core raytracer primitives f.e. canvas,
# matrix manipulation, shapes etc. etc.
set(RAYTRACER_LIB_SOURCES
  # ----------------------------------------------------------------------------
  # rendering related utilties
  raytracer_renderer.hpp
  single_threaded_renderer.cpp
  multi_threaded_renderer.cpp

  # ----------------------------------------------------------------------------
  # "everything" else
  execution_profiler.hpp
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
  uv_noise_texture.hpp
  uv_image_pattern.hpp
  uv_mapper.hpp
  uv_mapper.cpp
  cube.hpp
  cube.cpp
  cube_map_texture.hpp
  cube_map_texture.cpp
  cylinder.hpp
  cylinder.cpp
  stock_materials.hpp
  stock_materials.cpp
  cone.hpp
  cone.cpp
  group.hpp
  group.cpp
  triangle.hpp
  triangle.cpp
  mmapped_file_reader.hpp
  mmapped_file_reader.cpp
  obj_file_loader.hpp
  obj_file_loader.cpp
  obj_parse_result.hpp
  obj_parse_result.cpp
  )

add_library(rt-lib STATIC ${RAYTRACER_LIB_SOURCES})

# ------------------------------------------------------------------------------
# ... -DENABLE_EXECUTION_PROFILING ...
target_compile_definitions(rt-lib
  PUBLIC ENABLE_EXECUTION_PROFILING)

# ----------------------------------------------------------------------------
# execution profile disabled
target_include_directories(rt-lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE common
  PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(rt-lib
  PUBLIC common
  PRIVATE concurrent-queue
  PRIVATE pthread
  PRIVATE ${SDL2_LIBRARIES})

if (ENABLE_EXECUTION_PROFILING)
  target_include_directories(rt-lib
    PRIVATE TracyClient)

  target_link_libraries(rt-lib
    PRIVATE TracyClient)

  target_compile_definitions(rt-lib
    PUBLIC TRACY_ENABLE
    PUBLIC TRACY_CALLSTACK)

endif()

# ------------------------------------------------------------------------------
# chapter-02: drawing on a canvas.
#
# use drawing primitives to render path of cannonball from a virtual
# cannon.
add_executable(virtual-cannon virtual_cannon_main.cpp virtual_cannon.hpp virtual_cannon.cpp)
target_link_libraries(virtual-cannon PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-04: matrix-transformations.
#
# use matrices + matrix-transformations to draw a clock-dial
add_executable(clock-dial clock_dial_matxforms_main.cpp)
target_link_libraries(clock-dial PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-05: ray-sphere intersections.
#
# use ray-sphere intersection to draw silhouette of a sphere
add_executable(sphere-silhouette sphere_silhouette_main.cpp)
target_link_libraries(sphere-silhouette PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-06: light and shading.
#
# use phong-illumination to draw a realistic sphere. this is the
# multi-threaded version
add_executable(phong-sphere phong_sphere_main.cpp)
target_link_libraries(phong-sphere
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-07: making a scene; chapter-08: shadows
#
# render a scene with multiple objects, light sources and shadows.
#
# both single and multi threaded rendering is supported.
add_executable(making-a-scene render_world_main.cpp)
target_link_libraries(making-a-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-09: render with planes
#
# render a scene with multiple objects, light sources and shadows using planes
#
# both single and multi threaded rendering is supported.
add_executable(render-with-plane render_with_plane.cpp)
target_link_libraries(render-with-plane
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-10: render with patterns
#
# render a scene with multiple objects, light sources and shadows
# using planes and different patterns
#
# both single and multi threaded rendering is supported.
add_executable(render-with-patterns render_with_patterns.cpp)
target_link_libraries(render-with-patterns
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-10: render textures
#
# render a scene with perlin noise
#
# both single and multi threaded rendering is supported.
add_executable(render-textures render_textures.cpp)
target_link_libraries(render-textures
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-10: render with patterns
#
# render a scene with multiple objects, light sources and shadows
# using planes and different patterns
#
# both single and multi threaded rendering is supported.
add_executable(render-with-textures render_with_textures.cpp)
target_link_libraries(render-with-textures
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-10: render with canvas on sphere
#
# render a ppm texture on a sphere.
#
# both single and multi threaded rendering is supported.
add_executable(render-nasa-blue-earth render_nasa_blue_earth.cpp)
target_link_libraries(render-nasa-blue-earth
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-11: render with transparency and reflections
add_executable(render-with-reflections render_with_reflections.cpp)
target_link_libraries(render-with-reflections
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# chapter-11: render with transparency(t) and reflections(r) and refractions(r)
add_executable(render-with-t3r render_with_t3r.cpp)
target_link_libraries(render-with-t3r
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render nested glass spheres
add_executable(nested-glass-spheres render_nested_glass_spheres.cpp)
target_link_libraries(nested-glass-spheres
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render cuboid shapes and associated images

# ------------------------------------------------------------------------------
# cubes-align-check
add_executable(cube-align-check render_align_check_plane.cpp)
target_link_libraries(cube-align-check
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# cubes-checkered
add_executable(cube-checkered render_checkered_cubes.cpp)
target_link_libraries(cube-checkered
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# cube-skybox : map cross-format texture images on cube
add_executable(cube-skybox render_skybox.cpp)
target_link_libraries(cube-skybox
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# cube-skybox-movie : a movie of a rotating cube with cross-format
# texture images on cube
add_executable(cube-skybox-movie render_skybox_movie.cpp)
target_link_libraries(cube-skybox-movie
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# cube-scene : a simple scene with cubes as predominant shape
add_executable(cube-scene render_cube_scene.cpp)
target_link_libraries(cube-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# cube-lighting : playing with light inside a cube
add_executable(cube-light-scene render_cube_light_scene.cpp)
target_link_libraries(cube-light-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render scene with cylindrical shapes
add_executable(cylinder-scene render_cylinder_scene.cpp)
target_link_libraries(cylinder-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render sphere with various supported decorations
add_executable(decorated-spheres render_spheres_with_decorations.cpp)
target_link_libraries(decorated-spheres
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# textures on cylinders
add_executable(cylinder-texture-scene render_cylinder_texture_scene.cpp)
target_link_libraries(cylinder-texture-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render scene with conical shapes
add_executable(cone-scene render_cone_scene.cpp)
target_link_libraries(cone-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render scene with conical shapes
add_executable(profile-fib tracy-fib.cpp)

target_link_libraries(profile-fib
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render hexagon scene with groups
add_executable(hexagon-scene render_hexagon_scene.cpp)
target_link_libraries(hexagon-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render teapot scene
add_executable(obj-scene render_obj_scene.cpp)
target_link_libraries(obj-scene
  PRIVATE rt-lib)

# ------------------------------------------------------------------------------
# render a cessna
add_executable(cessna-scene render_cessna.cpp)
target_link_libraries(cessna-scene
  PRIVATE rt-lib)

