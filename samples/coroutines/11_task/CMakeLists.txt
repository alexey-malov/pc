# Add source to this project's executable.
add_executable (11_task "11_task.cpp"  "task.h" "sync_wait.h" "awaitable_traits.h" "manual_reset_event.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 11_task PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(11_task PRIVATE /W4 /WX)
else()
  target_compile_options(11_task PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
