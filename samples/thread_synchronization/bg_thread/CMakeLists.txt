
# Add source to this project's executable.
add_executable (bg_thread "bg_thread.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bg_thread PROPERTY CXX_STANDARD 23)
endif()

if(MSVC)
  target_compile_options(bg_thread PRIVATE /W4 /WX)
else()
  target_compile_options(bg_thread PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
