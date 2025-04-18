# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("lock_free")

# Include sub-projects.
add_subdirectory ("stack")
add_subdirectory ("lf_stack_1")
add_subdirectory ("lf_stack_2")
add_subdirectory ("lf_stack_3_hp")
add_subdirectory ("lf_stack_4_asp")
add_subdirectory ("shared_ptr")
