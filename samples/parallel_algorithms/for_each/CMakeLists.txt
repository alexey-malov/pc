﻿# CMakeList.txt : CMake project for for_each, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (for_each "for_each.cpp" "MeasureTime.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET for_each PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
