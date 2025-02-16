# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("shared_data")

# Include sub-projects.
add_subdirectory ("race_condition")
add_subdirectory ("access-other-thread-data")
add_subdirectory ("access-different-data")
add_subdirectory ("expr-evaluation-conflict")
add_subdirectory ("volatile-race-cond")
add_subdirectory ("sequencing-counters")
add_subdirectory ("synchronizes-with")
add_subdirectory ("sequenced-before")
add_subdirectory ("happens-before")
add_subdirectory ("shared-counter-with-mutex")
add_subdirectory ("ts-object")
add_subdirectory ("ts-counter")
add_subdirectory ("ts-stack-wrong")
add_subdirectory ("ts-stack-wrong2")
add_subdirectory ("ts-stack-wrong3")
add_subdirectory ("ts-stack-good")
