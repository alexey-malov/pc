# Add source to this project's executable.
add_executable (atomic_is_lock_free "atomic_is_lock_free.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET atomic_is_lock_free PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(atomic_is_lock_free PRIVATE /W4 /WX /arch:AVX2)
else()
  target_compile_options(atomic_is_lock_free PRIVATE -Wall -Wextra -Wpedantic -Werror -mavx2)
endif()

# TODO: Add tests and install targets if needed.
