# Add source to this project's executable.
add_executable (03_coro_internals "03_coro_internals.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 03_coro_internals PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(03_coro_internals PRIVATE /W4 /WX)
else()
  target_compile_options(03_coro_internals PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
