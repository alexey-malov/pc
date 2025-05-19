#include <CL/cl.h>
// #define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <variant>

using Clock = std::chrono::high_resolution_clock;
using Seconds = std::chrono::duration<double>;
using namespace std::literals;

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
__kernel void mut_matrices(
	const int rows1, const int columns1, const int columns2,
	__global const float* m1,
	__global const float* m2,
	__global float* result) {
            
	const int row = get_global_id(0);
	const int col = get_global_id(1);
	int m1Index = row * columns1;
	int m2Index = col;

	float sum = 0.0f;
	for (int i = 0; i < columns1; ++i) {
		sum += m1[m1Index++] * m2[m2Index];
		m2Index += columns2;
	}
	result[row * columns2 + col] = sum;
}
)CLC";

struct MultArgs
{
	int rows1;
	int columns1;
	int columns2;
};

using CommandArgs = std::variant<std::monostate, MultArgs>;

CommandArgs ParseCommandLine(int argc, char* argv[])
{
	if (argc == 1 || (argc == 2 && argv[1] == "--help"sv))
	{
		std::cout << "Usage:\n"
				  << argv[0] << " ROWS1 COLUMNS1 COLUMNS2";
		return {};
	}
	if (argc == 4)
	{
		const int rows1 = std::stoi(argv[1]);
		const int columns1 = std::stoi(argv[2]);
		const int columns2 = std::stoi(argv[3]);
		if (rows1 < 1 || columns1 < 1 || columns2 < 1)
		{
			throw std::runtime_error("Invalid matrix sizes");
		}
		return MultArgs{ .rows1 = rows1, .columns1 = columns1, .columns2 = columns2 };
	}
	throw std::runtime_error("Invalid command line");
}

std::vector<cl_float> MultMatricesOnCPU(int rows1, int columns1, int columns2)
{
	const int rows2 = columns1;
	std::vector<cl_float> m1(rows1 * columns1, 1.0f);
	std::vector<cl_float> m2(rows2 * columns2, 2.0f);
	std::vector<cl_float> result(rows2 * columns2, 0.0); // Результат

	const auto start = Clock::now();
	for (int i = 0; i < rows1; ++i)
	{
		for (int j = 0; j < columns2; ++j)
		{
			int m1Index = i * columns1;
			int m2Index = j;
			float sum = 0.0f;
			for (int k = 0; k < columns1; ++k)
			{
				sum += m1[m1Index++] * m2[m2Index];
				m2Index += columns2;
			}
			result[i * columns2 + j] = sum;
		}
	}
	const auto end = Clock::now();
	std::cout << "CPU Multiplication time: " << Seconds(end - start).count() << " seconds\n";

	return result;
}

void MultMatrices(int rows1, int columns1, int columns2)
{
	const int rows2 = columns1;

	std::vector<cl_float> m1(rows1 * columns1, 1.0f);
	std::vector<cl_float> m2(rows2 * columns2, 2.0f);
	std::vector<cl_float> result(rows2 * columns2, 0.0); // Результат

	auto device = SelectDevice();
	std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	cl::Program program(context, KernelSource, /* build = */ true);
	cl::Kernel kernel(program, "mut_matrices");

	const auto start = Clock::now();

	cl::Buffer buf1(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(cl_float) * m1.size(), m1.data());
	cl::Buffer buf2(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(cl_float) * m2.size(), m2.data());
	cl::Buffer buf3(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * result.size());

	kernel.setArg(0, rows1);
	kernel.setArg(1, columns1);
	kernel.setArg(2, columns2);
	kernel.setArg(3, buf1);
	kernel.setArg(4, buf2);
	kernel.setArg(5, buf3);

	// Итерационное пространство задачи равно размеру итоговой матрицы
	cl::NDRange globalSize(rows1, columns2);

	queue.enqueueNDRangeKernel(kernel, /*offset*/ cl::NullRange, globalSize, cl::NullRange);

	queue.enqueueReadBuffer(buf3, /* blocking= */ CL_TRUE, /* offset= */ 0,
		sizeof(cl_float) * result.size(), result.data());

	queue.finish();

	const auto end = Clock::now();

	std::cout << "GPU Multiplication time: " << Seconds(end - start).count() << " seconds\n";

	auto cpuResult = MultMatricesOnCPU(rows1, columns1, columns2);

	std::cout << result[0] << "\n";
	std::cout << cpuResult[0] << "\n";
}

int main(int argc, char* argv[])
{
	try
	{
		auto args = ParseCommandLine(argc, argv);
		if (const auto multArgs = std::get_if<MultArgs>(&args))
		{
			MultMatrices(multArgs->rows1, multArgs->columns1, multArgs->columns2);
		}
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