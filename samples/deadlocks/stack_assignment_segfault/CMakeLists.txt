﻿# CMakeList.txt : CMake project for stack_assignment, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (stack_assignment_segfault "stack_assignment_segfault.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET stack_assignment_segfault PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(stack_assignment_segfault PRIVATE /W4 /WX)
else()
  target_compile_options(stack_assignment_segfault PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# TODO: Add tests and install targets if needed.
