# Add source to this project's executable.
add_executable (lf_stack_1 "lf_stack_1.cpp" "LockFreeStack.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lf_stack_1 PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(lf_stack_1 PRIVATE /W4 /WX)
else()
  target_compile_options(lf_stack_1 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
