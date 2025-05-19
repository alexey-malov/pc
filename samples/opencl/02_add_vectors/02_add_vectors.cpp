#include <CL/cl.h>
// #define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <utility>

using Clock = std::chrono::high_resolution_clock;
using Seconds = std::chrono::duration<double>;

cl::Device SelectDevice()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.empty())
	{
		throw std::runtime_error("No OpenCL platforms found");
	}

	for (const auto& platform : platforms)
	{
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		if (!devices.empty())
		{
			return std::move(devices.front()); // Return the first device found
		}
	}

	throw std::runtime_error("No GPU devices found");
}

const char* KernelSource = R"CLC(
__kernel void vector_add(__global const float* a, // input vector a
                         __global const float* b, // input vector b
                         __global float* c,       // output vector c
                         const unsigned int count // number of elements
                         )
 {
    int i = get_global_id(0); // get the global thread ID
    c[i] = a[i] + b[i]; 
}
)CLC";

int main()
{
	try
	{
		auto device = SelectDevice();
		std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

		cl::Context context(device);
		cl::CommandQueue queue(context, device);

		// Размер векторов
		const size_t vectorSize = 1024 * 1024 * 256;

		// Исходные данные
		std::vector<cl_float> a(vectorSize, 1.0f); // вектор из 1.0
		std::vector<cl_float> b(vectorSize, 2.0f); // вектор из 2.0
		std::vector<cl_float> c(vectorSize, 0.0f); // выходной вектор

		// CL_MEM_READ_ONLY - память только для чтения
		// CL_MEM_COPY_HOST_PTR - скопировать данные из хоста в устройство.
		// Иначе буфер будет пустым, и придётся создавать отдельную команду
		cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(cl_float) * vectorSize, a.data());
		cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(cl_float) * vectorSize, b.data());

		// В этот буфер будет записан результат, поэтому не инициализируем мего данными
		cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * vectorSize);
	
		cl::Program program(context, KernelSource, /* build = */ true);
		cl::Kernel kernel(program, "vector_add");

		// Устанавливаем аргументы ядра
		kernel.setArg(0, bufferA);
		kernel.setArg(1, bufferB);
		kernel.setArg(2, bufferC);
		kernel.setArg(3, static_cast<cl_uint>(vectorSize));

		// Запускаем ядро
		// Создаём одномерный диапазон с количеством элементов, равным размеру вектора
		cl::NDRange global(vectorSize); // Общее число work item-ов

		const auto start = Clock::now();

		queue.enqueueNDRangeKernel(kernel, /*offset*/ cl::NullRange, global, cl::NullRange);

		queue.finish(); // Ждём завершения всех команд в очереди

		const auto end = Clock::now();

		const auto elapsed = end - start;
		std::cout << "Kernel execution time: " << Seconds(elapsed).count() << " seconds\n";

		// Читаем результат из устройства в хост
		queue.enqueueReadBuffer(bufferC, /* blocking= */ CL_TRUE, /* offset= */ 0,
			sizeof(cl_float) * vectorSize, c.data());
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