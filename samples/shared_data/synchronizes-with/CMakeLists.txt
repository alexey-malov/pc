
# Add source to this project's executable.
add_executable (synchronizes-with "synchronizes-with.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET synchronizes-with PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
