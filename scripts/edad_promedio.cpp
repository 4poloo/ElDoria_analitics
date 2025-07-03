#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <filesystem>
#include <utility>   // pair
#include <functional> // hash

using namespace std;

// ✅ Hash custom para pair<string, string>
struct pair_hash {
    template <class T1, class T2>
    size_t operator() (const pair<T1, T2>& p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

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

    using Key = pair<string, string>;
    unordered_map<Key, pair<uint64_t, uint64_t>, pair_hash> global_data; // suma, conteo

    omp_lock_t lock;
    omp_init_lock(&lock);

    size_t total = lines.size();

    // 🔵 OpenMP: cada hilo acumula suma y conteo por especie+género
    #pragma omp parallel
    {
        unordered_map<Key, pair<uint64_t, uint64_t>, pair_hash> local_data;

        #pragma omp for schedule(static)
        for (size_t i = 0; i < total; ++i) {
            stringstream ss(lines[i]);
            string campo;
            string especie, genero;
            int edad = -1;
            int col = 0;

            while (getline(ss, campo, ';')) {
                if (col == 0) especie = campo;
                else if (col == 1) genero = campo;
                else if (col == 2) {
                    try { edad = stoi(campo); } catch (...) {}
                    break;
                }
                col++;
            }

            if (edad >= 0) {
                local_data[{especie, genero}].first += edad;
                local_data[{especie, genero}].second += 1;
            }
        }

        omp_set_lock(&lock);
        for (const auto& [key, sum_count] : local_data) {
            global_data[key].first += sum_count.first;
            global_data[key].second += sum_count.second;
        }
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);

    ofstream out(output_file);
    out << "especie,genero,edad_promedio\n";
    for (const auto& [key, sum_count] : global_data) {
        double promedio = (double) sum_count.first / sum_count.second;
        out << key.first << "," << key.second << "," << promedio << "\n";
    }
    out.close();

    cout << "✅ Edad promedio generado: " << output_file << "\n";
    return 0;
}
