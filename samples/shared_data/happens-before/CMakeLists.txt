
# Add source to this project's executable.
add_executable (happens-before "happens-before.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET happens-before PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
