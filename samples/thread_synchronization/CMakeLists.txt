# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("thread_synchronization")

# Include sub-projects.
add_subdirectory ("busy_wait")
add_subdirectory ("wait_with_sleep")
add_subdirectory ("cv_wait_min")
add_subdirectory ("spurious_wakeup")
add_subdirectory ("wait_with_cv")
add_subdirectory ("waiting_stack")
add_subdirectory ("latch")
add_subdirectory ("barrier")
add_subdirectory ("stop_token")
add_subdirectory ("stop_source")
add_subdirectory ("stop_callback")
add_subdirectory ("future")
add_subdirectory ("packaged_task")
