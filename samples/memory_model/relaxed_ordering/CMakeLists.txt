# Add source to this project's executable.
add_executable (relaxed_ordering "relaxed_ordering.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET relaxed_ordering PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(relaxed_ordering PRIVATE /W4 /WX)
else()
  target_compile_options(relaxed_ordering PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
