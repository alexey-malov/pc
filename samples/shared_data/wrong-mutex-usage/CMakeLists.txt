
# Add source to this project's executable.
add_executable (wrong-mutex-usage "wrong-mutex-usage.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET wrong-mutex-usage PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
