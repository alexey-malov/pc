# Add source to this project's executable.
add_executable (07_simple_awaiter "07_simple_awaiter.cpp"   )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 07_simple_awaiter PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(07_simple_awaiter PRIVATE /W4 /WX)
else()
  target_compile_options(07_simple_awaiter PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
