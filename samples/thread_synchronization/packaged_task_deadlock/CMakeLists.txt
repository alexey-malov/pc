
# Add source to this project's executable.
add_executable (packaged_task_deadlock "packaged_task_deadlock.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET packaged_task_deadlock PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(packaged_task_deadlock PRIVATE /W4 /WX)
else()
  target_compile_options(packaged_task_deadlock PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
