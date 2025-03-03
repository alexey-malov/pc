
# Add source to this project's executable.
add_executable (cv_wait_min "cv_wait_min.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cv_wait_min PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(cv_wait_min PRIVATE /W4 /WX)
else()
  target_compile_options(cv_wait_min PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
