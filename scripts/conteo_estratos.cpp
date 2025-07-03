#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <archivo_entrada.txt> <archivo_salida.csv>\n";
        return 1;
    }

    string input_file = argv[1];
    string output_file = argv[2];

    if (!filesystem::exists(input_file)) {
        cerr << "❌ Archivo no encontrado: " << input_file << "\n";
        return 1;
    }

    ifstream infile(input_file);
    if (!infile.is_open()) {
        cerr << "❌ No se pudo abrir el archivo.\n";
        return 1;
    }

    // Saltar encabezado
    string header;
    getline(infile, header);

    vector<string> lines;
    string line;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    unordered_map<int, uint64_t> conteo_global;
    omp_lock_t writelock;
    omp_init_lock(&writelock);

    size_t total = lines.size();

    // 🔵 OpenMP: cada hilo hace conteo local
    #pragma omp parallel
    {
        unordered_map<int, uint64_t> conteo_local;

        #pragma omp for schedule(static)
        for (size_t i = 0; i < total; ++i) {
            stringstream ss(lines[i]);
            string campo;
            int col = 0;
            int estrato = -1;

            while (getline(ss, campo, ';')) {
                if (col == 3) { // columna estrato
                    try {
                        estrato = stoi(campo);
                    } catch (...) {
                        estrato = -1;
                    }
                    break;
                }
                col++;
            }

            if (estrato >= 0) {
                conteo_local[estrato]++;
            }
        }

        // 🔵 Combinar en global con lock
        omp_set_lock(&writelock);
        for (const auto& [estrato, count] : conteo_local) {
            conteo_global[estrato] += count;
        }
        omp_unset_lock(&writelock);
    }

    omp_destroy_lock(&writelock);

    // Guardar resultado
    filesystem::create_directories(filesystem::path(output_file).parent_path());
    ofstream out(output_file);
    out << "estrato,cantidad\n";
    for (const auto& [estrato, count] : conteo_global) {
        out << estrato << "," << count << "\n";
    }
    out.close();

    cout << "✅ Archivo generado: " << output_file << "\n";
    return 0;
}
