﻿# CMakeList.txt : CMake project for hello-world, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (thread-id "thread-id.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET thread-id PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
