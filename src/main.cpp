#include <chrono>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "algoritmos.hpp"
#include "generadores.hpp"
#include "metricas.hpp"
#include "utilidades.hpp"

namespace {

using Rutina = void (*)(std::vector<int>&, Metricas&);

struct Algoritmo {
    std::string nombre;
    Rutina sin_contar;
    Rutina contando;
    bool recursivo;
};

struct Configuracion {
    unsigned int semilla = 12345;
    int repeticiones = 30;
    std::string salida = "results/resultados.csv";
    std::string directorio_datos = "data";
    std::vector<std::size_t> tamanos = {1000, 10000, 100000};
    bool muestra = false;
};

std::vector<Algoritmo> crear_algoritmos() {
    return {{"insercion", insercion<false>, insercion<true>, false},
            {"quicksort", quicksort<false>, quicksort<true>, true}};
}

void mostrar_ayuda() {
    std::cout
        << "Uso: benchmark [opciones]\n"
        << "  --semilla N         semilla base (por defecto 12345)\n"
        << "  --repeticiones N    repeticiones por condicion (por defecto 30)\n"
        << "  --tamanos A,B,C     tamanos de entrada (por defecto 1000,10000,100000)\n"
        << "  --salida RUTA       archivo CSV de salida (por defecto results/resultados.csv)\n"
        << "  --datos RUTA        carpeta para las muestras (por defecto data)\n"
        << "  --muestra           genera entradas y salidas de ejemplo con n = 40 y termina\n";
}

std::vector<std::size_t> leer_tamanos(const std::string& texto) {
    std::vector<std::size_t> tamanos;
    std::stringstream flujo(texto);
    std::string parte;
    while (std::getline(flujo, parte, ',')) {
        tamanos.push_back(static_cast<std::size_t>(std::stoull(parte)));
    }
    return tamanos;
}

bool leer_argumentos(int argc, char** argv, Configuracion& cfg) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--muestra") {
            cfg.muestra = true;
            continue;
        }
        if (arg == "--ayuda" || arg == "-h") {
            return false;
        }
        if (i + 1 >= argc) {
            std::cerr << "Falta el valor de " << arg << "\n";
            return false;
        }
        const std::string valor = argv[++i];
        try {
            if (arg == "--semilla") {
                cfg.semilla = static_cast<unsigned int>(std::stoul(valor));
            } else if (arg == "--repeticiones") {
                cfg.repeticiones = std::stoi(valor);
            } else if (arg == "--salida") {
                cfg.salida = valor;
            } else if (arg == "--datos") {
                cfg.directorio_datos = valor;
            } else if (arg == "--tamanos") {
                cfg.tamanos = leer_tamanos(valor);
            } else {
                std::cerr << "Opcion desconocida: " << arg << "\n";
                return false;
            }
        } catch (const std::exception&) {
            std::cerr << "Valor invalido para " << arg << "\n";
            return false;
        }
    }
    return cfg.repeticiones > 0 && !cfg.tamanos.empty();
}

void escribir_vector(const std::filesystem::path& ruta,
                     const std::vector<int>& v) {
    std::ofstream archivo(ruta);
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i > 0) {
            archivo << ' ';
        }
        archivo << v[i];
    }
    archivo << '\n';
}

int generar_muestra(const Configuracion& cfg) {
    const std::filesystem::path carpeta(cfg.directorio_datos);
    std::filesystem::create_directories(carpeta);
    const std::vector<Algoritmo> algoritmos = crear_algoritmos();
    for (Patron patron : todos_los_patrones()) {
        const std::string nombre = nombre_patron(patron);
        const std::vector<int> entrada = generar(patron, 40, cfg.semilla);
        escribir_vector(carpeta / ("entrada_" + nombre + ".txt"), entrada);
        for (const Algoritmo& alg : algoritmos) {
            std::vector<int> datos = entrada;
            Metricas descarte;
            alg.sin_contar(datos, descarte);
            escribir_vector(carpeta / ("salida_" + alg.nombre + "_" + nombre + ".txt"),
                            datos);
        }
    }
    std::cout << "Muestras escritas en " << carpeta.string() << "\n";
    return 0;
}

int ejecutar_benchmark(const Configuracion& cfg) {
    const std::filesystem::path ruta(cfg.salida);
    if (ruta.has_parent_path()) {
        std::filesystem::create_directories(ruta.parent_path());
    }
    std::ofstream csv(ruta);
    if (!csv) {
        std::cerr << "No se pudo abrir " << cfg.salida << "\n";
        return 1;
    }
    csv << "algoritmo,n,patron,repeticion,tiempo_ms,comparaciones,intercambios,"
           "profundidad_recursion,semilla,commit_hash\n";

    const std::string commit = obtener_commit_hash();
    const std::vector<Algoritmo> algoritmos = crear_algoritmos();
    const std::vector<Patron>& patrones = todos_los_patrones();

    for (std::size_t n : cfg.tamanos) {
        for (std::size_t ip = 0; ip < patrones.size(); ++ip) {
            const Patron patron = patrones[ip];
            const std::string nombre = nombre_patron(patron);
            std::cout << "n=" << n << " patron=" << nombre << std::flush;

            for (int rep = 1; rep <= cfg.repeticiones; ++rep) {
                const unsigned int semilla_entrada =
                    semilla_para(cfg.semilla, n, ip, rep);
                const std::vector<int> base = generar(patron, n, semilla_entrada);

                for (const Algoritmo& alg : algoritmos) {
                    std::vector<int> datos = base;
                    Metricas descarte;
                    const auto inicio = std::chrono::steady_clock::now();
                    alg.sin_contar(datos, descarte);
                    const auto fin = std::chrono::steady_clock::now();
                    const double tiempo_ms =
                        std::chrono::duration<double, std::milli>(fin - inicio)
                            .count();
                    if (!esta_ordenado(datos)) {
                        std::cerr << "\nResultado no ordenado en " << alg.nombre
                                  << "\n";
                        return 1;
                    }

                    datos = base;
                    Metricas m;
                    alg.contando(datos, m);
                    if (!esta_ordenado(datos)) {
                        std::cerr << "\nResultado no ordenado en " << alg.nombre
                                  << " (corrida con contadores)\n";
                        return 1;
                    }

                    csv << alg.nombre << ',' << n << ',' << nombre << ',' << rep
                        << ',' << std::fixed << std::setprecision(6) << tiempo_ms
                        << ',' << m.comparaciones << ',' << m.intercambios << ',';
                    if (alg.recursivo) {
                        csv << m.profundidad_maxima;
                    } else {
                        csv << "NA";
                    }
                    csv << ',' << cfg.semilla << ',' << commit << '\n';
                }
            }
            csv.flush();
            std::cout << " listo\n";
        }
    }
    std::cout << "Resultados en " << cfg.salida << "\n";
    return 0;
}

}

int main(int argc, char** argv) {
    Configuracion cfg;
    if (!leer_argumentos(argc, argv, cfg)) {
        mostrar_ayuda();
        return 1;
    }
    if (cfg.muestra) {
        return generar_muestra(cfg);
    }
    return ejecutar_benchmark(cfg);
}
