#include "generadores.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <numeric>
#include <random>
#include <utility>

using namespace std;

namespace {

constexpr int VALOR_MAXIMO = 1000000;
constexpr int VALORES_UNICOS = 15;
constexpr double FRACCION_PERMUTADA = 0.05;

uint32_t entero_menor_que(mt19937& gen, uint32_t limite) {
    return static_cast<uint32_t>(gen()) % limite;
}

template <typename T>
void mezclar(vector<T>& v, mt19937& gen) {
    for (size_t i = v.size(); i > 1; --i) {
        const size_t j =
            entero_menor_que(gen, static_cast<uint32_t>(i));
        swap(v[i - 1], v[j]);
    }
}

vector<int> aleatorio(size_t n, mt19937& gen) {
    vector<int> v(n);
    for (int& x : v) {
        x = static_cast<int>(
            entero_menor_que(gen, static_cast<uint32_t>(VALOR_MAXIMO + 1)));
    }
    return v;
}

vector<int> ascendente(size_t n, mt19937& gen) {
    vector<int> v = aleatorio(n, gen);
    sort(v.begin(), v.end());
    return v;
}

vector<int> descendente(size_t n, mt19937& gen) {
    vector<int> v = aleatorio(n, gen);
    sort(v.begin(), v.end(), greater<int>());
    return v;
}

vector<int> casi_ordenado(size_t n, mt19937& gen) {
    vector<int> v = ascendente(n, gen);
    size_t k = static_cast<size_t>(
        ceil(FRACCION_PERMUTADA * static_cast<double>(n)));
    if (k > n) {
        k = n;
    }
    vector<size_t> posiciones(n);
    iota(posiciones.begin(), posiciones.end(), static_cast<size_t>(0));
    mezclar(posiciones, gen);
    posiciones.resize(k);
    vector<int> valores;
    valores.reserve(k);
    for (size_t pos : posiciones) {
        valores.push_back(v[pos]);
    }
    mezclar(valores, gen);
    for (size_t i = 0; i < k; ++i) {
        v[posiciones[i]] = valores[i];
    }
    return v;
}

vector<int> con_duplicados(size_t n, mt19937& gen) {
    vector<int> v(n);
    for (int& x : v) {
        x = static_cast<int>(
            entero_menor_que(gen, static_cast<uint32_t>(VALORES_UNICOS)));
    }
    return v;
}

}

const vector<Patron>& todos_los_patrones() {
    static const vector<Patron> patrones = {
        Patron::Aleatorio, Patron::Ascendente, Patron::Descendente,
        Patron::CasiOrdenado, Patron::Duplicados};
    return patrones;
}

string nombre_patron(Patron patron) {
    switch (patron) {
        case Patron::Aleatorio:
            return "aleatorio";
        case Patron::Ascendente:
            return "ascendente";
        case Patron::Descendente:
            return "descendente";
        case Patron::CasiOrdenado:
            return "casi_ordenado";
        case Patron::Duplicados:
            return "duplicados";
    }
    return "desconocido";
}

vector<int> generar(Patron patron, size_t n, unsigned int semilla) {
    mt19937 gen(semilla);
    switch (patron) {
        case Patron::Aleatorio:
            return aleatorio(n, gen);
        case Patron::Ascendente:
            return ascendente(n, gen);
        case Patron::Descendente:
            return descendente(n, gen);
        case Patron::CasiOrdenado:
            return casi_ordenado(n, gen);
        case Patron::Duplicados:
            return con_duplicados(n, gen);
    }
    return {};
}
