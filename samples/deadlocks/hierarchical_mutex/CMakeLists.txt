# CMakeList.txt : CMake project for stack_assignment, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (hierarchical_mutex "hierarchical_mutex.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET hierarchical_mutex PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(hierarchical_mutex PRIVATE /W4 /WX)
else()
  target_compile_options(hierarchical_mutex PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# TODO: Add tests and install targets if needed.
