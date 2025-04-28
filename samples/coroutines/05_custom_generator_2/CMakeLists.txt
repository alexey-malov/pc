# Add source to this project's executable.
add_executable (05_custom_generator_2 "05_custom_generator_2.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 05_custom_generator_2 PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(05_custom_generator_2 PRIVATE /W4 /WX)
else()
  target_compile_options(05_custom_generator_2 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
