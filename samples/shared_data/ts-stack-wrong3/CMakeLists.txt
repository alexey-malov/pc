﻿
# Add source to this project's executable.
add_executable (ts-stack-wrong3 "ts-stack-wrong3.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ts-stack-wrong3 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
