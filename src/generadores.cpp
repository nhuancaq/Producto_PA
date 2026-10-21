#include "generadores.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <numeric>
#include <random>
#include <utility>

namespace {

constexpr int VALOR_MAXIMO = 1000000;
constexpr int VALORES_UNICOS = 15;
constexpr double FRACCION_PERMUTADA = 0.05;

std::uint32_t entero_menor_que(std::mt19937& gen, std::uint32_t limite) {
    return static_cast<std::uint32_t>(gen()) % limite;
}

template <typename T>
void mezclar(std::vector<T>& v, std::mt19937& gen) {
    for (std::size_t i = v.size(); i > 1; --i) {
        const std::size_t j =
            entero_menor_que(gen, static_cast<std::uint32_t>(i));
        std::swap(v[i - 1], v[j]);
    }
}

std::vector<int> aleatorio(std::size_t n, std::mt19937& gen) {
    std::vector<int> v(n);
    for (int& x : v) {
        x = static_cast<int>(
            entero_menor_que(gen, static_cast<std::uint32_t>(VALOR_MAXIMO + 1)));
    }
    return v;
}

std::vector<int> ascendente(std::size_t n, std::mt19937& gen) {
    std::vector<int> v = aleatorio(n, gen);
    std::sort(v.begin(), v.end());
    return v;
}

std::vector<int> descendente(std::size_t n, std::mt19937& gen) {
    std::vector<int> v = aleatorio(n, gen);
    std::sort(v.begin(), v.end(), std::greater<int>());
    return v;
}

std::vector<int> casi_ordenado(std::size_t n, std::mt19937& gen) {
    std::vector<int> v = ascendente(n, gen);
    std::size_t k = static_cast<std::size_t>(
        std::ceil(FRACCION_PERMUTADA * static_cast<double>(n)));
    if (k > n) {
        k = n;
    }
    std::vector<std::size_t> posiciones(n);
    std::iota(posiciones.begin(), posiciones.end(), static_cast<std::size_t>(0));
    mezclar(posiciones, gen);
    posiciones.resize(k);
    std::vector<int> valores;
    valores.reserve(k);
    for (std::size_t pos : posiciones) {
        valores.push_back(v[pos]);
    }
    mezclar(valores, gen);
    for (std::size_t i = 0; i < k; ++i) {
        v[posiciones[i]] = valores[i];
    }
    return v;
}

std::vector<int> con_duplicados(std::size_t n, std::mt19937& gen) {
    std::vector<int> v(n);
    for (int& x : v) {
        x = static_cast<int>(
            entero_menor_que(gen, static_cast<std::uint32_t>(VALORES_UNICOS)));
    }
    return v;
}

}

const std::vector<Patron>& todos_los_patrones() {
    static const std::vector<Patron> patrones = {
        Patron::Aleatorio, Patron::Ascendente, Patron::Descendente,
        Patron::CasiOrdenado, Patron::Duplicados};
    return patrones;
}

std::string nombre_patron(Patron patron) {
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

std::vector<int> generar(Patron patron, std::size_t n, unsigned int semilla) {
    std::mt19937 gen(semilla);
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
