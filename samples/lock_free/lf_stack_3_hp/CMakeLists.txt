# Add source to this project's executable.
add_executable (lf_stack_3_hp "lf_stack_3_hp.cpp" "LockFreeStack.h" "HazardPointer.h" "HazardPointer.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lf_stack_3_hp PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(lf_stack_3_hp PRIVATE /W4 /WX)
else()
  target_compile_options(lf_stack_3_hp PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
