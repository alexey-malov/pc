# CMakeList.txt : CMake project for audio_player, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (audio_player "audio_player.cpp"  "miniaudio.c" "miniaudio.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET audio_player PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  target_compile_options(audio_player PRIVATE /W4 /WX)
else()
  target_compile_options(audio_player PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# TODO: Add tests and install targets if needed.
