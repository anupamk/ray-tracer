# ------------------------------------------------------------------------------
# list of all files making up the executables in this directory
SET(RT_EXECUTABLES_SOURCES
  render_virtual_cannon.cpp
  render_clock_dial.cpp
  render_sphere_silhouette.cpp
  render_phong_sphere.cpp
  render_world_main.cpp
  render_with_plane.cpp
  render_with_patterns.cpp
  render_textures.cpp
  render_with_textures.cpp
  render_nasa_blue_earth.cpp
  render_with_reflections.cpp
  render_with_t3r.cpp
  render_nested_glass_spheres.cpp
  render_align_check_plane.cpp
  render_checkered_cubes.cpp
  render_skybox.cpp
  render_skybox_movie.cpp
  render_cube_scene.cpp
  render_cube_light_scene.cpp
  render_cylinder_scene.cpp
  render_spheres_with_decorations.cpp
  render_cylinder_texture_scene.cpp
  render_cone_scene.cpp
  render_hexagon_scene.cpp
  render_teapot.cpp
  render_cessna.cpp
  render_newtons_cradle.cpp
  render_csg_dice.cpp
  render_chess_pawn.cpp
)

# ------------------------------------------------------------------------------
# list of all executables augmented with tracy profiler
SET(RT_TRACY_PROFILER_EXECUTABLE_SOURCES
  fibonacci.cpp
)

# ------------------------------------------------------------------------------
# no user serviceable parts from here on ↓

# ------------------------------------------------------------------------------
# generate all the release and tracy-profiler executable targets
include(CMakeBuildExecutablesFunctions)

generate_all_release_executable_targets("${RT_EXECUTABLES_SOURCES}")
generate_all_profiled_executable_targets("${RT_TRACY_PROFILER_EXECUTABLE_SOURCES}")
