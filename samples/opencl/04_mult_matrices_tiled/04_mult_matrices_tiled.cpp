#include <CL/cl.h>
// #define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>
#include <chrono>
#include <filesystem>
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
#define TILE_SIZE 16

__kernel void mult_matrices(
	const int rows1, const int columns1, const int columns2,
	__global const float* m1,
	__global const float* m2,
	__global float* result) {

	// Локальная память для хранения тайлов	1 и 2 матриц
	__local float m1sub[TILE_SIZE][TILE_SIZE];
	__local float m2sub[TILE_SIZE][TILE_SIZE];    

	const int rows2 = columns1;

	const int row = get_global_id(0);
	const int col = get_global_id(1);

	const int localRow = get_local_id(0);
	const int localCol = get_local_id(1);

	const int numTiles = (columns1 + TILE_SIZE - 1) / TILE_SIZE;

	float sum = 0.0f;
	for (int tile = 0; tile < numTiles; ++tile)
	{
		// Ряд и столбец матрицы, относящиеся к текущему тайлу
		int tiledRow = row;
		int tiledCol = tile * TILE_SIZE + localCol;
		// Загрузка нашего элемента в локальную память
		m1sub[localRow][localCol] = (tiledRow < rows1 && tiledCol < columns1)
			? m1[tiledRow * columns1 + tiledCol]
			: 0.0f;

		tiledRow = tile * TILE_SIZE + localRow;
		tiledCol = col;
		m2sub[localRow][localCol] = (tiledRow < rows2 && tiledCol < columns2)
			? m2[tiledRow * columns2 + tiledCol]
			: 0.0f;

		// Ждём, пока все потоки внутри группы загрузят свои данные в тайлы
		barrier(CLK_LOCAL_MEM_FENCE);

		// Перемножаем элементы тайла, соответствующие текущему потоку
		for (int i = 0; i < TILE_SIZE; ++i)
		{
			sum += m1sub[localRow][i] * m2sub[i][localCol];
		}

		// Ждём, пока все потоки внутри группы закончат перемножение
		barrier(CLK_LOCAL_MEM_FENCE);
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

namespace fs = std::filesystem;

CommandArgs ParseCommandLine(int argc, char* argv[])
{
	if (argc == 1 || (argc == 2 && argv[1] == "--help"sv))
	{
		std::cout << "Usage:\n"
				  << fs::path(argv[0]).filename() << " ROWS1 COLUMNS1 COLUMNS2";
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
	cl::Kernel kernel(program, "mult_matrices");

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

	const int tileSize = 16;

	// Увеличиваем глобальный размер до кратного tileSize
	cl::NDRange globalSize(
		(rows1 + tileSize - 1) / tileSize * tileSize,
		(columns2 + tileSize - 1) / tileSize * tileSize);
	// Локальный размер равен размеру тайла
	cl::NDRange localSize(tileSize, tileSize);

	queue.enqueueNDRangeKernel(kernel, /*offset*/ cl::NullRange, globalSize, localSize);

	queue.enqueueReadBuffer(buf3, /* blocking= */ CL_TRUE, /* offset= */ 0,
		sizeof(cl_float) * result.size(), result.data());

	queue.finish();

	const auto end = Clock::now();

	std::cout << "GPU Multiplication time: " << Seconds(end - start).count() << " seconds\n";

#if 1
	auto cpuResult = MultMatricesOnCPU(rows1, columns1, columns2);
	std::cout << cpuResult[0] << "\n";
#endif

	std::cout << result[0] << "\n";
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