
# Add source to this project's executable.
add_executable (broken_promise "broken_promise.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET broken_promise PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(broken_promise PRIVATE /W4 /WX)
else()
  target_compile_options(broken_promise PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
