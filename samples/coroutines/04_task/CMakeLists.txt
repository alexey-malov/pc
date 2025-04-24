# Add source to this project's executable.
add_executable (04_task "04_task.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 04_task PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(04_task PRIVATE /W4 /WX)
else()
  target_compile_options(04_task PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
