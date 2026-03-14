#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>
#include "dot_product_uint.h"

int main()
{
	std::vector<uint32_t> a(100'000'000, 2u);
	std::vector<uint32_t> b(100'000'000, 3u);
	MeasureTime("for", DotProductFor, a, b);
	MeasureTime("seq", DotProductSeq, a, b);
	MeasureTime("par", DotProductPar, a, b);
	MeasureTime("unseq", DotProductUnseq, a, b);
	MeasureTime("par_unseq", DotProductParUnseq, a, b);
}
