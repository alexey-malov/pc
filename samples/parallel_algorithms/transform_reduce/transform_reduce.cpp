#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>
#include "dot_product.h"

int main()
{
	std::vector<double> a(100'000'000, 1.5);
	std::vector<double> b(100'000'000, 2.0);
	MeasureTime("for", DotProductFor, a, b);
	MeasureTime("seq", DotProductSeq, a, b);
	MeasureTime("par", DotProductPar, a, b);
	MeasureTime("unseq", DotProductUnseq, a, b);
	MeasureTime("par_unseq", DotProductParUnseq, a, b);
}
