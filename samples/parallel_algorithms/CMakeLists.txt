# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("parallel_algorithms")

# Include sub-projects.
add_subdirectory ("for_each")
add_subdirectory ("sort")
add_subdirectory ("small_task")
add_subdirectory ("overhead")
add_subdirectory ("mutex_contention")
add_subdirectory ("atomic_contention")
add_subdirectory ("parallel_reduce")
add_subdirectory ("transform_reduce")
add_subdirectory ("transform_reduce_uint")
add_subdirectory ("cache_locality")
add_subdirectory ("cache_line")
add_subdirectory ("random_vs_seq")
add_subdirectory ("cache_contention")
add_subdirectory ("false_sharing")
add_subdirectory ("no_false_sharing")
add_subdirectory ("count_primes")
add_subdirectory ("oversubscription")
