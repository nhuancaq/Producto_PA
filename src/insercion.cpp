#include <cstddef>

#include "algoritmos.hpp"

template <bool Contar>
void insercion(std::vector<int>& a, Metricas& m) {
    const std::size_t n = a.size();
    for (std::size_t i = 1; i < n; ++i) {
        const int clave = a[i];
        std::size_t j = i;
        while (j > 0) {
            if constexpr (Contar) {
                ++m.comparaciones;
            }
            if (a[j - 1] > clave) {
                a[j] = a[j - 1];
                if constexpr (Contar) {
                    ++m.intercambios;
                }
                --j;
            } else {
                break;
            }
        }
        a[j] = clave;
    }
}

template void insercion<true>(std::vector<int>&, Metricas&);
template void insercion<false>(std::vector<int>&, Metricas&);
