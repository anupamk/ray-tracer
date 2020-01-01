# project name
project(ray-tracer CXX)

# the main program
add_executable(ray-tracer raytracer_main.cpp)
target_link_libraries(ray-tracer PRIVATE common)
