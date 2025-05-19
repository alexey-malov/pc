# Add source to this project's executable.
add_executable (01_list_devices "01_list_devices.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 01_list_devices PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(01_list_devices PRIVATE /W4 /WX)
else()
	target_compile_options(01_list_devices PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

find_package(OpenCL REQUIRED)

target_compile_definitions(01_list_devices PRIVATE
	CL_TARGET_OPENCL_VERSION=300
	CL_HPP_TARGET_OPENCL_VERSION=300)
target_include_directories(01_list_devices PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(01_list_devices PRIVATE OpenCL::OpenCL)
