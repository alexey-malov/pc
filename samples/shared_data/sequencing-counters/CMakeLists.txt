
# Add source to this project's executable.
add_executable (sequencing-counters "sequencing-counters.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sequencing-counters PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
