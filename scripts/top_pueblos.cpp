// ==========================================
// 📂 top_pueblos.cpp
// ==========================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "❌ Uso: " << argv[0] << " <datos_completos.txt> <salida_top_pueblos.csv>\n";
        return 1;
    }

    const string input_file  = argv[1];
    const string output_file = argv[2];

    ifstream in(input_file);
    if (!in.is_open()) {
        cerr << "❌ No se pudo abrir: " << input_file << "\n";
        return 1;
    }

    // Saltar encabezado
    string header;
    getline(in, header);

    const size_t CHUNK = 200000;
    vector<string> lines;
    lines.reserve(CHUNK);

    unordered_map<string, uint64_t> global_pueblos;
    omp_lock_t lock;
    omp_init_lock(&lock);

    while (in) {
        lines.clear();
        string line;
        for (size_t i = 0; i < CHUNK && getline(in, line); ++i) {
            lines.emplace_back(move(line));
        }
        if (lines.empty()) break;

        #pragma omp parallel
        {
            unordered_map<string, uint64_t> local_pueblos;

            #pragma omp for schedule(static)
            for (size_t i = 0; i < lines.size(); ++i) {
                const string& L = lines[i];
                size_t start = 0, pos;
                string campos[6];

                for (int f = 0; f < 5; ++f) {
                    pos = L.find(';', start);
                    if (pos == string::npos) { start = L.size(); break; }
                    campos[f] = L.substr(start, pos - start);
                    start = pos + 1;
                }
                if (start < L.size())
                    campos[5] = L.substr(start);

                if (!campos[4].empty())
                    local_pueblos[campos[4]]++;
                if (!campos[5].empty())
                    local_pueblos[campos[5]]++;
            }

            // Mezclar resultados locales ➜ global
            omp_set_lock(&lock);
            for (auto& kv : local_pueblos)
                global_pueblos[kv.first] += kv.second;
            omp_unset_lock(&lock);
        }
    }

    omp_destroy_lock(&lock);
    in.close();

    // Ordenar nodos por frecuencia
    vector<pair<string, uint64_t>> vec(global_pueblos.begin(), global_pueblos.end());
    sort(vec.begin(), vec.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });

    if (vec.size() > 10000)
        vec.resize(10000);

    ofstream out(output_file);
    out << "poblado,frecuencia\n";
    for (const auto& kv : vec) {
        out << kv.first << "," << kv.second << "\n";
    }
    out.close();

    cout << "✅ Top pueblos generado: " << output_file << "\n";
    return 0;
}
