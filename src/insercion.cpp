#include <cstddef>

#include "algoritmos.hpp"

using namespace std;

template <bool Contar>
void insercion(vector<int>& a, Metricas& m) {
    const size_t n = a.size();
    for (size_t i = 1; i < n; ++i) {
        const int clave = a[i];
        size_t j = i;
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

template void insercion<true>(vector<int>&, Metricas&);
template void insercion<false>(vector<int>&, Metricas&);
