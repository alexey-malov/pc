# Add source to this project's executable.
add_executable (lazy_init_bench "lazy_init_bench.cpp" "MeasureTime.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lazy_init_bench PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(lazy_init_bench PRIVATE /W4 /WX)
else()
  target_compile_options(lazy_init_bench PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
