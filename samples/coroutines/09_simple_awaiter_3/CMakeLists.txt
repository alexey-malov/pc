# Add source to this project's executable.
add_executable (09_simple_awaiter_3 "09_simple_awaiter_3.cpp"    "09_simple_awaiter_3.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 09_simple_awaiter_3 PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(09_simple_awaiter_3 PRIVATE /W4 /WX)
else()
  target_compile_options(09_simple_awaiter_3 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
