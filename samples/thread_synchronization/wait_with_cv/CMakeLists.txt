
# Add source to this project's executable.
add_executable (wait_with_cv "wait_with_cv.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET wait_with_cv PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(wait_with_cv PRIVATE /W4 /WX)
else()
  target_compile_options(wait_with_cv PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
