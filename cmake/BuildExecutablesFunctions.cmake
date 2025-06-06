# -*- cmake -*-

# ------------------------------------------------------------------------------
# all functions used for building different kinds of executables are
# defined here.

# ------------------------------------------------------------------------------
# generate *one* executable-target based on the incoming
# file-name, with the executable which will be called
# '<executable-file-name>'.
#
# thus, for example, for a file named 'render_foo.cpp' we get an
# executable called 'render_foo' etc.
function (generate_one_release_executable_target FNAME)

  get_filename_component(FNAME_SANS_EXT ${FNAME} NAME_WLE)
  set(EXECUTABLE_NAME "${FNAME_SANS_EXT}")

  add_executable(${EXECUTABLE_NAME} ${FNAME})

  target_link_libraries(${EXECUTABLE_NAME}
    # ----------------------------------------------------------------------------
    # 3rd-party libraries
    PRIVATE concurrent-queue
    PRIVATE pthread
    PRIVATE ${SDL2_LIBRARIES}

    # ----------------------------------------------------------------------------
    # our libraries
    PRIVATE rt_io
    PRIVATE common_utils
    PRIVATE rt_utils
    PRIVATE rt_file_utils
    PRIVATE rt_thread_utils
    PRIVATE rt_primitives
    PRIVATE rt_shapes
    PRIVATE rt_patterns)

endfunction()

# ------------------------------------------------------------------------------
# generate *all* executable-targets based on list of files making up
# the executables.
#
# one file means one executable.
function (generate_all_release_executable_targets FNAME_LIST)

  foreach (FNAME ${FNAME_LIST})
    generate_one_release_executable_target(${FNAME})
  endforeach()

endfunction()

# ------------------------------------------------------------------------------
# generate a single tracy profiled executable-target based on the
# incoming file-name, which will be called '<tracy-executable-file-name>'
#
# so, for a file named 'render_foo.cpp' we get an executable called
# 'tracy_render_foo' etc.
function (generate_one_profiled_executable_target FNAME)

  get_filename_component(FNAME_SANS_EXT ${FNAME} NAME_WLE)
  set(TRACY_EXECUTABLE_NAME "tracy_${FNAME_SANS_EXT}")

  add_executable(${TRACY_EXECUTABLE_NAME} ${FNAME})

  target_link_libraries(${TRACY_EXECUTABLE_NAME}
    # ----------------------------------------------------------------------------
    # 3rd-party libraries
    PRIVATE concurrent-queue
    PRIVATE pthread
    PRIVATE ${SDL2_LIBRARIES}

    # ----------------------------------------------------------------------------
    # our libraries
    PRIVATE rt_io
    PRIVATE common_utils
    PRIVATE rt_utils
    PRIVATE rt_file_utils
    PRIVATE rt_thread_utils
    PRIVATE rt_primitives
    PRIVATE rt_shapes
    PRIVATE rt_patterns
  )

  augment_execution_profiling(${TRACY_EXECUTABLE_NAME})

endfunction()

# ------------------------------------------------------------------------------
# generate all executable-targets based on list of files making up the
# tests. each file corresponds to 1 unique executable
function (generate_all_profiled_executable_targets FNAME_LIST)

  foreach (FNAME ${FNAME_LIST})
    generate_one_profiled_executable_target(${FNAME})
  endforeach()

endfunction()
