# ------------------------------------------------------------------------------
# ray tracer io specific routines
add_library(rt_io SHARED
  camera.cpp
  camera.hpp
  camera_render.cpp
  canvas.cpp
  canvas.hpp
  canvas_ppm_reader.cpp
  canvas_ppm_writer.cpp
  obj_file_loader.cpp
  obj_file_loader.hpp
  obj_parse_result.cpp
  obj_parse_result.hpp
  phong_illumination.cpp
  phong_illumination.hpp
  wavefront-obj-file-format-notes.org
  world.cpp
  world.hpp
  render_params.hpp
  render_params.cpp
  xcb_display.hpp
  xcb_display.cpp)

target_link_libraries(rt_io
  # ----------------------------------------------------------------------------
  # 3rd-party libraries
  PRIVATE concurrent-queue
  PRIVATE pthread
  PRIVATE ${X11_LIBRARIES}

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils
  PRIVATE rt_utils
  PRIVATE rt_primitives
  PRIVATE rt_shapes)

augment_execution_profiling(rt_io)

target_include_directories(rt_io
  PUBLIC ${CMAKE_SOURCE_DIR}/src)

