
# Add source to this project's executable.
add_executable (shared_future "shared_future.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET shared_future PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(shared_future PRIVATE /W4 /WX)
else()
  target_compile_options(shared_future PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
