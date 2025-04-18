# CMakeList.txt : CMake project for lf_stack_1, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (shared_ptr "shared_ptr.cpp" "shared_ptr.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET shared_ptr PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(shared_ptr PRIVATE /W4 /WX)
else()
  target_compile_options(shared_ptr PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
