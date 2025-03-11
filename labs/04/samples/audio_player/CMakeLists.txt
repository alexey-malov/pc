# CMakeList.txt : CMake project for audio_player, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (audio_player "audio_player.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET audio_player PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
