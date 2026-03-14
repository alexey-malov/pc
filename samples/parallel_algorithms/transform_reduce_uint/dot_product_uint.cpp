#include "dot_product_uint.h"
#include <execution>

uint64_t DotProductFor(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
{
	uint64_t result = 0;
	for (size_t i = 0; i < a.size(); ++i)
		result += uint64_t{ a[i] } * b[i];
	return result;
}

namespace
{
template <class ExecutionPolicy>
uint64_t DotProductImpl(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b,
	ExecutionPolicy&& policy)
{
	return std::transform_reduce(policy, a.begin(), a.end(),
		b.begin(), uint64_t{}, std::plus<uint64_t>(), std::multiplies<>());
}
} // namespace

uint64_t DotProductSeq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
{
	return DotProductImpl(a, b, std::execution::seq);
}

uint64_t DotProductPar(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
{
	return DotProductImpl(a, b, std::execution::par);
}

uint64_t DotProductUnseq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
{
	return DotProductImpl(a, b, std::execution::unseq);
}

uint64_t DotProductParUnseq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
{
	return DotProductImpl(a, b, std::execution::par_unseq);
}
