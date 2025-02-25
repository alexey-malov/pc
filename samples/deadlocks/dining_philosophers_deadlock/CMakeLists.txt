# CMakeList.txt : CMake project for stack_assignment, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (dining_philosophers_deadlock "dining_philosophers_deadlock.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dining_philosophers_deadlock PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(dining_philosophers_deadlock PRIVATE /W4 /WX)
else()
  target_compile_options(dining_philosophers_deadlock PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# TODO: Add tests and install targets if needed.
