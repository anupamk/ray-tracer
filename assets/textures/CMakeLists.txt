# -*- cmake -*-

# ------------------------------------------------------------------------------
# convert all textures from 'jpg -> (ascii) ppm', and copy them over
# to ${CMAKE_BINARY_DIR}/assets/textures/*
add_custom_target(cp_assets_textures
  COMMAND ${CMAKE_COMMAND} -E echo "* copying textures *"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/assets/textures
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/convert_cp_textures.sh -s ${CMAKE_CURRENT_SOURCE_DIR} -d ${CMAKE_BINARY_DIR}/assets/textures
)
