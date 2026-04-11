#include "MeasureTime.h"
#include <vector>

constexpr size_t N = 8192;

void RowMajor(std::vector<std::vector<int>>& m)
{
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			m[i][j] += 1;
		}
	}
}

void ColumnMajor(std::vector<std::vector<int>>& m)
{
	for (size_t j = 0; j < N; ++j)
	{
		for (size_t i = 0; i < N; ++i)
		{
			m[i][j] += 1;
		}
	}
}

int main()
{
	std::vector<std::vector<int>> m(N, std::vector<int>(N));

	MeasureTime("row-major", RowMajor, m);
	MeasureTime("column-major", ColumnMajor, m);
}