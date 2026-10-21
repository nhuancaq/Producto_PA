#ifndef GENERADORES_HPP
#define GENERADORES_HPP

#include <cstddef>
#include <string>
#include <vector>

enum class Patron {
    Aleatorio,
    Ascendente,
    Descendente,
    CasiOrdenado,
    Duplicados
};

const std::vector<Patron>& todos_los_patrones();

std::string nombre_patron(Patron patron);

std::vector<int> generar(Patron patron, std::size_t n, unsigned int semilla);

#endif
