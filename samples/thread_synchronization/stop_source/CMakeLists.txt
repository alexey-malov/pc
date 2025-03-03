
# Add source to this project's executable.
add_executable (stop_source "stop_source.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET stop_source PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(stop_source PRIVATE /W4 /WX)
else()
  target_compile_options(stop_source PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
