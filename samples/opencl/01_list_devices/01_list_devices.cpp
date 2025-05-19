#include <CL/cl.h>
#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/opencl.hpp>
#include <iostream>
#include <stdexcept>
#include <utility>


void PrintDeviceType(cl_device_type type)
{
	std::cout << "Device type: ";
	std::pair<cl_device_type, std::string_view> deviceTypes[] = {
		{ CL_DEVICE_TYPE_CPU, "CPU" },
		{ CL_DEVICE_TYPE_GPU, "GPU" },
		{ CL_DEVICE_TYPE_ACCELERATOR, "Accelerator" },
		{ CL_DEVICE_TYPE_CUSTOM, "Custom" },
	};

	for (const auto& [deviceType, name] : deviceTypes)
	{
		if (type & deviceType)
		{
			std::cout << name << " ";
		}
	}
}

void ListPlatformDevices(const cl::Platform& platform)
{
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (devices.empty())
	{
		throw std::runtime_error("No OpenCL devices found");
	}
	for (const auto& device : devices)
	{
		std::string deviceName;
		device.getInfo(CL_DEVICE_NAME, &deviceName);
		std::cout << "  Device: " << deviceName << ". ";
		cl_device_type deviceType;
		device.getInfo(CL_DEVICE_TYPE, &deviceType);
		PrintDeviceType(deviceType);
		std::cout << "\n";
	}
}

void ListPlatformsAndDevices()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.empty())
	{
		throw std::runtime_error("No OpenCL platforms found");
	}

	for (const auto& platform : platforms)
	{
		std::string platformName;
		platform.getInfo(CL_PLATFORM_NAME, &platformName);
		std::cout << "Platform: " << platformName << "\n";
		ListPlatformDevices(platform);
	}
}

int main()
{
	try
	{
		ListPlatformsAndDevices();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Unknown error\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}