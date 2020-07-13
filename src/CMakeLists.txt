# project name
project(ray-tracer CXX)

#
# find sdl2 package : required for sdl2_canvas implementation
#
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

# all the sources that we are testing, go into it's own static library
set (RAYTRACER_LIB_SOURCES
  constants.hpp
  utils.hpp
  tuple.hpp
  tuple.cpp
  color.hpp
  color.cpp
  canvas.hpp
  canvas.cpp
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
  camera.cpp)

add_library(raytracer_lib STATIC ${RAYTRACER_LIB_SOURCES} )
target_include_directories(raytracer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(raytracer_lib PRIVATE common ${SDL2_LIBRARIES})

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
target_link_libraries(ch-08-making-a-scene PRIVATE concurrentqueue)
target_link_libraries(ch-08-making-a-scene PRIVATE pthread)
