# -*- cmake -*-

# ------------------------------------------------------------------------------
# all functions used for building tests are defined here


# ------------------------------------------------------------------------------
# no user serviceable parts from here on ↓
add_compile_options(
  -Wno-unused-variable
  -Wno-unused-but-set-variable
)

# ------------------------------------------------------------------------------
# this function is called to set a 'TEST_NAME' variable (in the callee
# scope), with a value 'rt_file-name-without-extension'
#
# thus when "file_name == 'foo_abcd.cpp'", the test name will be
# called 'rt_foo_abcd'
#
# this common routine is used for both generating test targets, as
# well as running the tests.
function (____set_test_name_from_file_name file_name)
  get_filename_component(FNAME_SANS_EXT ${file_name} NAME_WLE)
  set(TEST_NAME "rt_${FNAME_SANS_EXT}" PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------------
# generate a single test target based on incoming file-name.
function (____generate_one_test_target file_name)

  # ----------------------------------------------------------------------------
  # step-1: setup test-name
  ____set_test_name_from_file_name(${file_name})

  # ----------------------------------------------------------------------------
  # step-2: build + link it
  add_executable(${TEST_NAME} ${file_name})

  target_link_libraries(${TEST_NAME}
    # --------------------------------------------------------------------------
    # 3rd-party
    PRIVATE doctest
    PRIVATE concurrent-queue

    # --------------------------------------------------------------------------
    # while testing the interfaces implemented in the current
    # sub-directory, some tests tend to use facilities from other
    # sub-directories.
    #
    # so, we end up linking against pretty much everything while
    # building these tests
    PRIVATE common_utils
    PRIVATE rt_utils
    PRIVATE rt_file_utils
    PRIVATE rt_thread_utils
    PRIVATE rt_primitives
    PRIVATE rt_shapes
    PRIVATE rt_patterns
    PRIVATE rt_io)

  # ----------------------------------------------------------------------------
  # add the test
  add_test(
    NAME ${TEST_NAME}
    COMMAND ${TEST_NAME}
  )

endfunction()

# ------------------------------------------------------------------------------
# generate a single test target based on incoming file-name.
function (____run_one_test_target file_name)

  # ----------------------------------------------------------------------------
  # step-1: setup test-name
  ____set_test_name_from_file_name(${file_name})

  # ----------------------------------------------------------------------------
  # run tests
  add_custom_command(
     TARGET ${TEST_NAME}
     COMMENT "running test '${TEST_NAME}'"
     POST_BUILD
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R '^${TEST_NAME}' --output-on-failure
   )

endfunction()

# ------------------------------------------------------------------------------
# all user callable functions are defined ↓

# ------------------------------------------------------------------------------
# generate all test-targets based on list of files making up the
# tests. each file corresponds to 1 unique executable
function (generate_all_test_targets FNAME_LIST)
  foreach (FNAME ${FNAME_LIST})
    ____generate_one_test_target(${FNAME})
  endforeach()
endfunction()

# ------------------------------------------------------------------------------
# run all test-targets based on list of files making up the
# tests.
function (run_all_tests FNAME_LIST)
  foreach (FNAME ${FNAME_LIST})
    ____run_one_test_target(${FNAME})
  endforeach()
endfunction()

