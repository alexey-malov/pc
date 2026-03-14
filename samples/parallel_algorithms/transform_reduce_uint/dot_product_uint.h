#pragma once
#include <vector>
#include <cstdint>

// Реализации функций пришлось вынести в отдельный translation unit,
// Чтобы оптимизатор не смог их встроить и тем самым исказить результаты замеров.

uint64_t DotProductFor(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
uint64_t DotProductSeq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
uint64_t DotProductPar(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
uint64_t DotProductUnseq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
uint64_t DotProductParUnseq(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);
