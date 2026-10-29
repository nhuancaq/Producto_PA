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

using namespace std;

namespace {

using Rutina = void (*)(vector<int>&, Metricas&);

struct Algoritmo {
    string nombre;
    Rutina sin_contar;
    Rutina contando;
    bool recursivo;
};

struct Configuracion {
    unsigned int semilla = 12345;
    int repeticiones = 30;
    string salida = "results/resultados.csv";
    string directorio_datos = "data";
    vector<size_t> tamanos = {1000, 10000, 100000};
    bool muestra = false;
};

vector<Algoritmo> crear_algoritmos() {
    return {{"insercion", insercion<false>, insercion<true>, false},
            {"quicksort", quicksort<false>, quicksort<true>, true}};
}

void mostrar_ayuda() {
    cout
        << "Uso: benchmark [opciones]\n"
        << "  --semilla N         semilla base (por defecto 12345)\n"
        << "  --repeticiones N    repeticiones por condicion (por defecto 30)\n"
        << "  --tamanos A,B,C     tamanos de entrada (por defecto 1000,10000,100000)\n"
        << "  --salida RUTA       archivo CSV de salida (por defecto results/resultados.csv)\n"
        << "  --datos RUTA        carpeta para las muestras (por defecto data)\n"
        << "  --muestra           genera entradas y salidas de ejemplo con n = 40 y termina\n";
}

vector<size_t> leer_tamanos(const string& texto) {
    vector<size_t> tamanos;
    stringstream flujo(texto);
    string parte;
    while (getline(flujo, parte, ',')) {
        tamanos.push_back(static_cast<size_t>(stoull(parte)));
    }
    return tamanos;
}

bool leer_argumentos(int argc, char** argv, Configuracion& cfg) {
    for (int i = 1; i < argc; ++i) {
        const string arg = argv[i];
        if (arg == "--muestra") {
            cfg.muestra = true;
            continue;
        }
        if (arg == "--ayuda" || arg == "-h") {
            return false;
        }
        if (i + 1 >= argc) {
            cerr << "Falta el valor de " << arg << "\n";
            return false;
        }
        const string valor = argv[++i];
        try {
            if (arg == "--semilla") {
                cfg.semilla = static_cast<unsigned int>(stoul(valor));
            } else if (arg == "--repeticiones") {
                cfg.repeticiones = stoi(valor);
            } else if (arg == "--salida") {
                cfg.salida = valor;
            } else if (arg == "--datos") {
                cfg.directorio_datos = valor;
            } else if (arg == "--tamanos") {
                cfg.tamanos = leer_tamanos(valor);
            } else {
                cerr << "Opcion desconocida: " << arg << "\n";
                return false;
            }
        } catch (const exception&) {
            cerr << "Valor invalido para " << arg << "\n";
            return false;
        }
    }
    return cfg.repeticiones > 0 && !cfg.tamanos.empty();
}

void escribir_vector(const filesystem::path& ruta,
                     const vector<int>& v) {
    ofstream archivo(ruta);
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) {
            archivo << ' ';
        }
        archivo << v[i];
    }
    archivo << '\n';
}

int generar_muestra(const Configuracion& cfg) {
    const filesystem::path carpeta(cfg.directorio_datos);
    filesystem::create_directories(carpeta);
    const vector<Algoritmo> algoritmos = crear_algoritmos();
    for (Patron patron : todos_los_patrones()) {
        const string nombre = nombre_patron(patron);
        const vector<int> entrada = generar(patron, 40, cfg.semilla);
        escribir_vector(carpeta / ("entrada_" + nombre + ".txt"), entrada);
        for (const Algoritmo& alg : algoritmos) {
            vector<int> datos = entrada;
            Metricas descarte;
            alg.sin_contar(datos, descarte);
            escribir_vector(carpeta / ("salida_" + alg.nombre + "_" + nombre + ".txt"),
                            datos);
        }
    }
    cout << "Muestras escritas en " << carpeta.string() << "\n";
    return 0;
}

int ejecutar_benchmark(const Configuracion& cfg) {
    const filesystem::path ruta(cfg.salida);
    if (ruta.has_parent_path()) {
        filesystem::create_directories(ruta.parent_path());
    }
    ofstream csv(ruta);
    if (!csv) {
        cerr << "No se pudo abrir " << cfg.salida << "\n";
        return 1;
    }
    csv << "algoritmo,n,patron,repeticion,tiempo_ms,comparaciones,intercambios,"
           "profundidad_recursion,semilla,commit_hash\n";

    const string commit = obtener_commit_hash();
    const vector<Algoritmo> algoritmos = crear_algoritmos();
    const vector<Patron>& patrones = todos_los_patrones();

    for (size_t n : cfg.tamanos) {
        for (size_t ip = 0; ip < patrones.size(); ++ip) {
            const Patron patron = patrones[ip];
            const string nombre = nombre_patron(patron);
            cout << "n=" << n << " patron=" << nombre << flush;

            for (int rep = 1; rep <= cfg.repeticiones; ++rep) {
                const unsigned int semilla_entrada =
                    semilla_para(cfg.semilla, n, ip, rep);
                const vector<int> base = generar(patron, n, semilla_entrada);

                for (const Algoritmo& alg : algoritmos) {
                    vector<int> datos = base;
                    Metricas descarte;
                    const auto inicio = chrono::steady_clock::now();
                    alg.sin_contar(datos, descarte);
                    const auto fin = chrono::steady_clock::now();
                    const double tiempo_ms =
                        chrono::duration<double, milli>(fin - inicio)
                            .count();
                    if (!esta_ordenado(datos)) {
                        cerr << "\nResultado no ordenado en " << alg.nombre
                                  << "\n";
                        return 1;
                    }

                    datos = base;
                    Metricas m;
                    alg.contando(datos, m);
                    if (!esta_ordenado(datos)) {
                        cerr << "\nResultado no ordenado en " << alg.nombre
                                  << " (corrida con contadores)\n";
                        return 1;
                    }

                    csv << alg.nombre << ',' << n << ',' << nombre << ',' << rep
                        << ',' << fixed << setprecision(6) << tiempo_ms
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
            cout << " listo\n";
        }
    }
    cout << "Resultados en " << cfg.salida << "\n";
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
