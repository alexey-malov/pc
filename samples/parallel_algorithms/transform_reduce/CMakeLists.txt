# CMakeList.txt : CMake project for for_each, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (transform_reduce "transform_reduce.cpp" "MeasureTime.h" "dot_product.h" "dot_product.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET transform_reduce PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
