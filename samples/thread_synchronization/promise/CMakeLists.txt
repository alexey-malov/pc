
# Add source to this project's executable.
add_executable (promise "promise.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET promise PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(promise PRIVATE /W4 /WX)
else()
  target_compile_options(promise PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
