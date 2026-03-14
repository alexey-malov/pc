# Add source to this project's executable.
add_executable (async_task "async_task.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET async_task PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(async_task PRIVATE /W4 /WX)
else()
  target_compile_options(async_task PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
