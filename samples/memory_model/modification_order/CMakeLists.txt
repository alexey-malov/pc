# Add source to this project's executable.
add_executable (modification_order "modification_order.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET modification_order PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(modification_order PRIVATE /W4 /WX)
else()
  target_compile_options(modification_order PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
