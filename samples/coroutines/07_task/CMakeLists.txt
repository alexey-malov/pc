# Add source to this project's executable.
add_executable (07_lazy "07_task.cpp"  "task.h" "sync_wait.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 07_lazy PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(07_lazy PRIVATE /W4 /WX)
else()
  target_compile_options(07_lazy PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
