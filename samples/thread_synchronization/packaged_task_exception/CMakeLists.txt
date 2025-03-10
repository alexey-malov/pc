
# Add source to this project's executable.
add_executable (packaged_task_exception "packaged_task_exception.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET packaged_task_exception PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(packaged_task_exception PRIVATE /W4 /WX)
else()
  target_compile_options(packaged_task_exception PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
