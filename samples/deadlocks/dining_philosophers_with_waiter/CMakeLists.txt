﻿# CMakeList.txt : CMake project for stack_assignment, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (dining_philosophers_with_waiter "dining_philosophers_with_waiter.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dining_philosophers_with_waiter PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(dining_philosophers_with_waiter PRIVATE /W4 /WX)
else()
  target_compile_options(dining_philosophers_with_waiter PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# TODO: Add tests and install targets if needed.
