# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("memory_model")

# Include sub-projects.
add_subdirectory ("lazy_init")
add_subdirectory ("lazy_init_ts")
add_subdirectory ("lazy_init_bench")
add_subdirectory ("lazy_init_ts_bench")
add_subdirectory ("lazy_init_dcl_wrong_bench")
add_subdirectory ("call_once")
add_subdirectory ("lazy_init_with_call_once")
add_subdirectory ("lazy_init_atomic_dcl_bench")
add_subdirectory ("lazy_init_atomic_dcl_optimized")
add_subdirectory ("object_storage")
add_subdirectory ("modification_order")
add_subdirectory ("atomic_is_lock_free")
add_subdirectory ("atomic_flag")
add_subdirectory ("lazy_init_ts_spin_bench")
