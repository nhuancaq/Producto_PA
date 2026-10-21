#ifndef ALGORITMOS_HPP
#define ALGORITMOS_HPP

#include <vector>

#include "metricas.hpp"

template <bool Contar>
void insercion(std::vector<int>& a, Metricas& m);

template <bool Contar>
void quicksort(std::vector<int>& a, Metricas& m);

#endif
