#pragma once
#include <vector>

// Реализации функций пришлось вынести в отдельный translation unit,
// Чтобы оптимизатор не смог их встроить и тем самым исказить результаты замеров.

double DotProductFor(const std::vector<double>& a, const std::vector<double>& b);
double DotProductSeq(const std::vector<double>& a, const std::vector<double>& b);
double DotProductPar(const std::vector<double>& a, const std::vector<double>& b);
double DotProductUnseq(const std::vector<double>& a, const std::vector<double>& b);
double DotProductParUnseq(const std::vector<double>& a, const std::vector<double>& b);
