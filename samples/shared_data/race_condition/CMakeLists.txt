﻿
# Add source to this project's executable.
add_executable (race_condition "race_condition.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET race_condition PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
