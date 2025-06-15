# Add source to this project's executable.
add_executable (20_fairy_tale "20_fairy_tale.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 20_fairy_tale PROPERTY CXX_STANDARD 23)
endif()

if(MSVC)
  target_compile_options(20_fairy_tale PRIVATE /W4 /WX)
else()
  target_compile_options(20_fairy_tale PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
