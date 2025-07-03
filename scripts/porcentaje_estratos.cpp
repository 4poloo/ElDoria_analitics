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
        cerr << "❌ Uso: " << argv[0] << " <archivo_entrada.txt> <archivo_salida.csv>\n";
        return 1;
    }

    string input_file = argv[1];
    string output_file = argv[2];

    ifstream infile(input_file);
    if (!infile.is_open()) {
        cerr << "❌ No se pudo abrir el archivo.\n";
        return 1;
    }

    string header;
    getline(infile, header);

    vector<string> lines;
    string line;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    unordered_map<int, uint64_t> conteo_global;
    omp_lock_t lock;
    omp_init_lock(&lock);

    size_t total = lines.size();

    // 🔵 OpenMP: cada hilo hace conteo local
    #pragma omp parallel
    {
        unordered_map<int, uint64_t> local_count;

        #pragma omp for schedule(static)
        for (size_t i = 0; i < total; ++i) {
            stringstream ss(lines[i]);
            string campo;
            int col = 0;
            int estrato = -1;

            while (getline(ss, campo, ';')) {
                if (col == 3) {
                    try { estrato = stoi(campo); } catch (...) {}
                    break;
                }
                col++;
            }

            if (estrato >= 0) {
                local_count[estrato]++;
            }
        }

        omp_set_lock(&lock);
        for (const auto& [estrato, count] : local_count) {
            conteo_global[estrato] += count;
        }
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);

    uint64_t total_personas = 0;
    for (const auto& [_, count] : conteo_global) {
        total_personas += count;
    }

    ofstream out(output_file);
    out << "estrato,porcentaje\n";
    for (const auto& [estrato, count] : conteo_global) {
        double pct = (100.0 * count) / total_personas;
        out << estrato << "," << pct << "\n";
    }
    out.close();

    cout << "✅ Porcentaje de estratos generado: " << output_file << "\n";
    return 0;
}
