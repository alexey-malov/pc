# Add source to this project's executable.
add_executable (07_task "07_task.cpp"  "task.h" "sync_wait.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 07_task PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(07_task PRIVATE /W4 /WX)
else()
  target_compile_options(07_task PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
