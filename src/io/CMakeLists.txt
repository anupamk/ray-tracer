# ------------------------------------------------------------------------------
# ray tracer io specific routines
add_library(rt_io SHARED
  camera.cpp
  camera.hpp
  canvas.cpp
  canvas.hpp
  canvas_ppm_reader.cpp
  canvas_ppm_writer.cpp
  multi_threaded_renderer.cpp
  multi_threaded_work_generator.hpp
  multi_threaded_work_generator.cpp
  obj_file_loader.cpp
  obj_file_loader.hpp
  obj_parse_result.cpp
  obj_parse_result.hpp
  phong_illumination.cpp
  phong_illumination.hpp
  raytracer_renderer.hpp
  single_threaded_renderer.cpp
  wavefront-obj-file-format-notes.org
  world.cpp
  world.hpp)

target_link_libraries(rt_io
  PUBLIC common_utils
  PUBLIC rt_utils
  PUBLIC rt_primitives
  PUBLIC rt_shapes
  PUBLIC rt_patterns
  PUBLIC concurrent-queue
  PUBLIC pthread
  PUBLIC ${SDL2_LIBRARIES})

augment_execution_profiling(rt_io)

target_include_directories(rt_io PUBLIC ${CMAKE_SOURCE_DIR}/src)

