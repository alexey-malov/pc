# Add source to this project's executable.
add_executable (ts_queue_with_mutex "ts_queue_with_mutex.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ts_queue_with_mutex PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(ts_queue_with_mutex PRIVATE /W4 /WX)
else()
  target_compile_options(ts_queue_with_mutex PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
