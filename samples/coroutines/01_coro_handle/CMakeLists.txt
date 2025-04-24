# Add source to this project's executable.
add_executable (01_coro_handle "01_coro_handle.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 01_coro_handle PROPERTY CXX_STANDARD 23)
endif()

if(MSVC)
  target_compile_options(01_coro_handle PRIVATE /W4 /WX)
else()
  target_compile_options(01_coro_handle PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
