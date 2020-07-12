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
# and a trivial 'virtual-cannon' for exercising vectors+points
set (VIRTUAL_CANNON_SOURCES
  virtual_cannon.hpp
  virtual_cannon.cpp)

add_executable(virtual-cannon virtual_cannon_main.cpp ${VIRTUAL_CANNON_SOURCES})
target_link_libraries(virtual-cannon PRIVATE common)
target_link_libraries(virtual-cannon PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# draw a clock-dial via matrix transformations
add_executable(clock-dial clock_dial_matxforms_main.cpp)
target_link_libraries(clock-dial PRIVATE common)
target_link_libraries(clock-dial PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# draw the sphere silhouette
add_executable(sphere-silhouette sphere_silhouette_main.cpp)
target_link_libraries(sphere-silhouette PRIVATE common)
target_link_libraries(sphere-silhouette PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# draw the sphere illuminated with phong-reflection model
add_executable(st-phong-sphere st_phong_sphere_main.cpp)
target_link_libraries(st-phong-sphere PRIVATE common)
target_link_libraries(st-phong-sphere PRIVATE raytracer_lib)

# ------------------------------------------------------------------------------
# multi-threaded phong-sphere
add_executable(mt-phong-sphere mt_phong_sphere_main.cpp)
target_link_libraries(mt-phong-sphere PRIVATE common)
target_link_libraries(mt-phong-sphere PRIVATE raytracer_lib)
target_link_libraries(mt-phong-sphere PRIVATE concurrentqueue)
target_link_libraries(mt-phong-sphere PRIVATE pthread)

# ------------------------------------------------------------------------------
# rendering a world: single+multi threaded
add_executable(render-world render_world_main.cpp)
target_link_libraries(render-world PRIVATE common)
target_link_libraries(render-world PRIVATE raytracer_lib)
target_link_libraries(render-world PRIVATE concurrentqueue)
target_link_libraries(render-world PRIVATE pthread)
