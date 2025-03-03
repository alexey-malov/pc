
# Add source to this project's executable.
add_executable (busy_wait "busy_wait.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET busy_wait PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(busy_wait PRIVATE /W4 /WX)
else()
  target_compile_options(busy_wait PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
