# Add source to this project's executable.
add_executable (10_continuation "10_continuation.cpp"    "10_continuation.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 10_continuation PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(10_continuation PRIVATE /W4 /WX)
else()
  target_compile_options(10_continuation PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
