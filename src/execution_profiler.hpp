#pragma once

/*
 * just defines trivial wrappers for commonly used tracy profiler annoation
 * macros.
 **/

#ifdef ENABLE_EXECUTION_PROFILING

#include "3rd-party/tracy/Tracy.hpp"

#define PROFILE_SCOPE ZoneScopedN(__PRETTY_FUNCTION__)

#else

#define PROFILE_SCOPE

#endif
