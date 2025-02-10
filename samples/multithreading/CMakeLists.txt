# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("multithreading")

# Include sub-projects.
add_subdirectory ("hello-world")
add_subdirectory ("hello-world-jthread")
add_subdirectory ("hello-world-detach")
add_subdirectory ("sync-stream")
add_subdirectory ("object-as-thread-function")
add_subdirectory ("dangling-references")
add_subdirectory ("thread-fn-params")
add_subdirectory ("thread-ownership")
add_subdirectory ("parallel-accumulate")
add_subdirectory ("thread-id")
