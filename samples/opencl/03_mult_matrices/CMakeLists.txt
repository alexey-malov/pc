# Add source to this project's executable.
add_executable (03_mult_matrices "03_mult_matrices.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 03_mult_matrices PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(03_mult_matrices PRIVATE /W4 /WX)
else()
	target_compile_options(03_mult_matrices PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

find_package(OpenCL REQUIRED)

target_compile_definitions(03_mult_matrices PRIVATE
	CL_TARGET_OPENCL_VERSION=300
	CL_HPP_TARGET_OPENCL_VERSION=300)
target_include_directories(03_mult_matrices PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(03_mult_matrices PRIVATE OpenCL::OpenCL)
