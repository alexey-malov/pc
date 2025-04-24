# Add source to this project's executable.
add_executable (00_generator "00_generator.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 00_generator PROPERTY CXX_STANDARD 23)
endif()

if(MSVC)
  target_compile_options(00_generator PRIVATE /W4 /WX)
else()
  target_compile_options(00_generator PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
