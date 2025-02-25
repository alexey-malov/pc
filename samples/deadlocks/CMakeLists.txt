# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  cmake_policy(SET CMP0092 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("deadlocks")

# Include sub-projects.
add_subdirectory ("stack_assignment_segfault")
add_subdirectory ("stack_assignment_deadlock")
add_subdirectory ("nonrecursive_mutex_deadlock")
add_subdirectory ("dining_philosophers_deadlock")
add_subdirectory ("dining_philosophers_with_waiter")
add_subdirectory ("dining_philosophers_with_ordering")
add_subdirectory ("dining_philosophers_with_scoped_lock")
add_subdirectory ("stack_assignment_fix")
add_subdirectory ("hierarchical_mutex")
