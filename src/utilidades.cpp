#include "utilidades.hpp"

#include <cstdio>

using namespace std;

bool esta_ordenado(const vector<int>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i - 1] > v[i]) {
            return false;
        }
    }
    return true;
}

string obtener_commit_hash() {
#ifdef _WIN32
    FILE* tuberia = _popen("git rev-parse --short HEAD 2>nul", "r");
#else
    FILE* tuberia = popen("git rev-parse --short HEAD 2>/dev/null", "r");
#endif
    if (tuberia == nullptr) {
        return "desconocido";
    }
    char buffer[64];
    string resultado;
    if (fgets(buffer, sizeof(buffer), tuberia) != nullptr) {
        resultado = buffer;
    }
#ifdef _WIN32
    _pclose(tuberia);
#else
    pclose(tuberia);
#endif
    while (!resultado.empty() &&
           (resultado.back() == '\n' || resultado.back() == '\r')) {
        resultado.pop_back();
    }
    return resultado.empty() ? "desconocido" : resultado;
}

unsigned int semilla_para(unsigned int base, size_t n,
                          size_t indice_patron, int repeticion) {
    return base + 1000003u * static_cast<unsigned int>(repeticion) +
           7919u * static_cast<unsigned int>(n) +
           104729u * static_cast<unsigned int>(indice_patron);
}
