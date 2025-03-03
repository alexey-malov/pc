# Add source to this project's executable.
add_executable (waiting_stack "waiting_stack.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET waiting_stack PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(waiting_stack PRIVATE /W4 /WX)
else()
  target_compile_options(waiting_stack PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
