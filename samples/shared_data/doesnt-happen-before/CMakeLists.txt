﻿
# Add source to this project's executable.
add_executable (doesnt-happen-before "doesnt-happen-before.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET doesnt-happen-before PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
