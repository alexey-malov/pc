# Add source to this project's executable.
add_executable (06_custom_generator_3 "06_custom_generator_3.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 06_custom_generator_3 PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(06_custom_generator_3 PRIVATE /W4 /WX)
else()
  target_compile_options(06_custom_generator_3 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
