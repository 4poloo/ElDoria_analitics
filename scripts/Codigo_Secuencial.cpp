#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <algorithm>

const int TOTAL_ESTRATOS = 10;

using namespace std;
using namespace chrono;

string clean(const string& str) {
    string limpio = str;
    if (limpio.size() >= 2 && limpio.front() == '"' && limpio.back() == '"')
        limpio = limpio.substr(1, limpio.size() - 2);

    // Convertir a minúsculas
    transform(limpio.begin(), limpio.end(), limpio.begin(), ::tolower);
    return limpio;
}

int obtenerEstrato(const string& cp) {
    if (cp.empty()) return -1;
    return cp[0] - '0';
}

int calcularEdad(const string& fecha) {
    if (fecha.size() < 4) return -1;
    int anio = stoi(fecha.substr(0, 4));
    return 2025 - anio;
}

int main() {
    ifstream archivo("/workspaces/ElDoria_analitics/data/eldoria.csv");
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        return 1;
    }

    unordered_map<string, vector<int>> datosPorClave;

    string linea;
    getline(archivo, linea); // Saltar encabezado

    int lineasValidas = 0;

    auto inicio = high_resolution_clock::now();

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        vector<string> campos;

        while (getline(ss, campo, ';')) {
            campos.push_back(clean(campo));
        }

        if (campos.size() < 8) continue;

        string especie = campos[1];
        string genero = campos[2];
        string fecha = campos[5];
        string cp = campos[6];
        string destino = campos[7];

        int edad= calcularEdad(fecha);

        string clave = especie + "_" + genero;
        datosPorClave[clave].push_back(edad);

        lineasValidas++;

        if (lineasValidas % 1000000 == 0 || lineasValidas <= 100) {
                        auto ahora = high_resolution_clock::now();
                        auto duracion = duration_cast<milliseconds>(ahora - inicio);

                        {
                            cout << "[PROCESADO] #" << lineasValidas
                                 << " | Especie: " << especie
                                 << ", Género: " << genero
                                 << ", edad: " << edad
                                 << ", CP: " << cp
                                 << ",fecha: " << fecha
                                 << ", destino: "<< destino
                                 << " | Tiempo: " << duracion.count() << " ms" << endl;
                        }
                    }
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nTotal líneas válidas: " << lineasValidas << endl;
    cout << "Tiempo total (secuencial): " << duracion.count()/1000 << " seg " << endl;

    // --- Imprimir contenido para corroborar ---
    for (const auto& par : datosPorClave) {
        const string& clave = par.first;
        const vector<int>& edades = par.second;

        cout << "Clave: " << clave << " | Total edades: " << edades.size() << endl;

        // Mostrar las primeras 5 edades como muestra
        for (size_t i = 0; i < edades.size() && i < 5; ++i) {
            cout << "  Edad " << i + 1 << ": " << edades[i] << endl;
        }

        cout << "-----------------------------" << endl;
    }

    return 0;
}