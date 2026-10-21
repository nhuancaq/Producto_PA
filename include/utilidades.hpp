#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <cstddef>
#include <string>
#include <vector>

bool esta_ordenado(const std::vector<int>& v);

std::string obtener_commit_hash();

unsigned int semilla_para(unsigned int base, std::size_t n,
                          std::size_t indice_patron, int repeticion);

#endif
