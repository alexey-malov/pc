﻿
# Add source to this project's executable.
add_executable (spurious_wakeup "spurious_wakeup.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET spurious_wakeup PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(spurious_wakeup PRIVATE /W4 /WX)
else()
  target_compile_options(spurious_wakeup PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
