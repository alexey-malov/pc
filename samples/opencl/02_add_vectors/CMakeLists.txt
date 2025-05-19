# Add source to this project's executable.
add_executable (02_add_vectors "02_add_vectors.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 02_add_vectors PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(02_add_vectors PRIVATE /W4 /WX)
else()
	target_compile_options(02_add_vectors PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

find_package(OpenCL REQUIRED)

target_compile_definitions(02_add_vectors PRIVATE
	CL_TARGET_OPENCL_VERSION=300
	CL_HPP_TARGET_OPENCL_VERSION=300)
target_include_directories(02_add_vectors PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(02_add_vectors PRIVATE OpenCL::OpenCL)
