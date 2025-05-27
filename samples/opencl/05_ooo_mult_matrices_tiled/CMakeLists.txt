# Add source to this project's executable.
add_executable (05_ooo_mult_matrices_tiled "05_ooo_mult_matrices_tiled.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 05_ooo_mult_matrices_tiled PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(05_ooo_mult_matrices_tiled PRIVATE /W4 /WX)
else()
	target_compile_options(05_ooo_mult_matrices_tiled PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

find_package(OpenCL REQUIRED)

target_compile_definitions(05_ooo_mult_matrices_tiled PRIVATE
	CL_TARGET_OPENCL_VERSION=300
	CL_HPP_TARGET_OPENCL_VERSION=300)
target_include_directories(05_ooo_mult_matrices_tiled PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(05_ooo_mult_matrices_tiled PRIVATE OpenCL::OpenCL)
