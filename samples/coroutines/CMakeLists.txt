# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.30)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("coroutines")

# Include sub-projects.
add_subdirectory ("00_generator")
add_subdirectory ("01_coro_handle")
add_subdirectory ("02_resumable")
add_subdirectory ("04_custom_generator")
add_subdirectory ("05_custom_generator_2")
add_subdirectory ("06_custom_generator_3")
add_subdirectory ("07_simple_awaiter")
add_subdirectory ("08_simple_awaiter_2")
add_subdirectory ("09_simple_awaiter_3")
add_subdirectory ("10_continuation")
add_subdirectory ("11_task")
add_subdirectory ("12_boost_cobalt")
add_subdirectory ("13_delay")
add_subdirectory ("14_co_main")
add_subdirectory ("15_promise")
add_subdirectory ("16_cobalt_task")
add_subdirectory ("17_simple_http_client")
add_subdirectory ("18_simple_http_client_2")
add_subdirectory ("19_fairy_tale")
add_subdirectory ("04_task")
