﻿# Add source to this project's executable.
add_executable (lazy_init_ts "lazy_init_ts.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lazy_init_ts PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(lazy_init_ts PRIVATE /W4 /WX)
else()
  target_compile_options(lazy_init_ts PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
