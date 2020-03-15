# project name
project(ray-tracer CXX)

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
  )

add_library(raytracer_lib STATIC ${RAYTRACER_LIB_SOURCES})
target_link_libraries(raytracer_lib PRIVATE common)
target_include_directories(raytracer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# the main program
add_executable(ray-tracer raytracer_main.cpp)
target_link_libraries(ray-tracer PRIVATE common)
target_link_libraries(ray-tracer PRIVATE raytracer_lib)

#
# and a trivial 'virtual-cannon' for exercising vectors+points
#
set (VIRTUAL_CANNON_SOURCES
  virtual_cannon.hpp
  virtual_cannon.cpp)

add_executable(virtual-cannon virtual_cannon_main.cpp ${VIRTUAL_CANNON_SOURCES})
target_link_libraries(virtual-cannon PRIVATE common)
target_link_libraries(virtual-cannon PRIVATE raytracer_lib)
