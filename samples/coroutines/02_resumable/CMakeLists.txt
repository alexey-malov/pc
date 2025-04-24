# Add source to this project's executable.
add_executable (02_resumable "02_resumable.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 02_resumable PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(02_resumable PRIVATE /W4 /WX)
else()
  target_compile_options(02_resumable PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
