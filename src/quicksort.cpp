#include <cstddef>
#include <utility>

#include "algoritmos.hpp"

using namespace std;

namespace {

using Indice = ptrdiff_t;

template <bool Contar>
int mediana_de_tres(const vector<int>& a, Indice bajo, Indice medio,
                    Indice alto, Metricas& m) {
    const int x = a[bajo];
    const int y = a[medio];
    const int z = a[alto];
    int comparaciones = 1;
    int resultado;
    if (x < y) {
        ++comparaciones;
        if (y < z) {
            resultado = y;
        } else {
            ++comparaciones;
            resultado = (x < z) ? z : x;
        }
    } else {
        ++comparaciones;
        if (x < z) {
            resultado = x;
        } else {
            ++comparaciones;
            resultado = (y < z) ? z : y;
        }
    }
    if constexpr (Contar) {
        m.comparaciones += comparaciones;
    }
    return resultado;
}

template <bool Contar>
Indice particionar(vector<int>& a, Indice bajo, Indice alto, Metricas& m) {
    const Indice medio = bajo + (alto - bajo) / 2;
    const int pivote = mediana_de_tres<Contar>(a, bajo, medio, alto, m);
    Indice i = bajo - 1;
    Indice j = alto + 1;
    while (true) {
        do {
            ++i;
            if constexpr (Contar) {
                ++m.comparaciones;
            }
        } while (a[i] < pivote);
        do {
            --j;
            if constexpr (Contar) {
                ++m.comparaciones;
            }
        } while (a[j] > pivote);
        if (i >= j) {
            return j;
        }
        swap(a[i], a[j]);
        if constexpr (Contar) {
            ++m.intercambios;
        }
    }
}

template <bool Contar>
void ordenar(vector<int>& a, Indice bajo, Indice alto, int profundidad,
             Metricas& m) {
    if (bajo >= alto) {
        return;
    }
    if constexpr (Contar) {
        if (profundidad > m.profundidad_maxima) {
            m.profundidad_maxima = profundidad;
        }
    }
    const Indice corte = particionar<Contar>(a, bajo, alto, m);
    ordenar<Contar>(a, bajo, corte, profundidad + 1, m);
    ordenar<Contar>(a, corte + 1, alto, profundidad + 1, m);
}

}

template <bool Contar>
void quicksort(vector<int>& a, Metricas& m) {
    if (a.size() < 2) {
        return;
    }
    ordenar<Contar>(a, 0, static_cast<Indice>(a.size()) - 1, 1, m);
}

template void quicksort<true>(vector<int>&, Metricas&);
template void quicksort<false>(vector<int>&, Metricas&);
