
# Add source to this project's executable.
add_executable (access-other-thread-data "access-other-thread-data.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET access-other-thread-data PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
