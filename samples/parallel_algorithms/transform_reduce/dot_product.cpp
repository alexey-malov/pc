#include "dot_product.h"
#include <execution>

double DotProductFor(const std::vector<double>& a, const std::vector<double>& b)
{
	double result = 0.0;
	for (size_t i = 0; i < a.size(); ++i)
		result += a[i] * b[i];
	return result;
}

namespace
{
template <class ExecutionPolicy>
double DotProductImpl(const std::vector<double>& a, const std::vector<double>& b,
	ExecutionPolicy&& policy)
{
	return std::transform_reduce(policy, a.begin(), a.end(),
		b.begin(), 0.0, std::plus<>(), std::multiplies<>());
}
}

double DotProductSeq(const std::vector<double>& a, const std::vector<double>& b)
{
	return DotProductImpl(a, b, std::execution::seq);
}

double DotProductPar(const std::vector<double>& a, const std::vector<double>& b)
{
	return DotProductImpl(a, b, std::execution::par);
}

double DotProductUnseq(const std::vector<double>& a, const std::vector<double>& b)
{
	return DotProductImpl(a, b, std::execution::unseq);
}

double DotProductParUnseq(const std::vector<double>& a, const std::vector<double>& b)
{
	return DotProductImpl(a, b, std::execution::par_unseq);
}

