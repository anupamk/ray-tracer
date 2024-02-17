# -*- cmake -*-

# ------------------------------------------------------------------------------
# this target copies all (wavefront) obj files
# ${CMAKE_BINARY_DIR}/assets/OBJ
add_custom_target(cp_assets_objs
  COMMAND ${CMAKE_COMMAND} -E echo "* copying OBJs *"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/assets/OBJ
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/*.obj ${CMAKE_BINARY_DIR}/assets/OBJ
)
