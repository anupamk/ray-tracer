# ------------------------------------------------------------------------------
# doctest: a simple testing framework for c++
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/doctest)

# ------------------------------------------------------------------------------
# concurrentqueue : a concurrent, lockless mpmc queue from moodycamel
add_library(concurrent-queue INTERFACE)
target_include_directories(concurrent-queue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
