
# Add source to this project's executable.
add_executable (stack "stack.cpp" "stack.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET stack PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(stack PRIVATE /W4 /WX)
else()
  target_compile_options(stack PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
