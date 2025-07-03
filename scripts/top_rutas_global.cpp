// ==========================================
// 📂 top_rutas_global.cpp
// ==========================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <omp.h>

using namespace std;

// Estructura para top rutas
struct Ruta {
    string origen, destino;
    uint64_t cantidad;
};

struct Compare {
    bool operator()(const Ruta& a, const Ruta& b) const {
        return a.cantidad > b.cantidad; // min-heap: el menor arriba
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "❌ Uso: " << argv[0] << " <datos_completos.txt> <salida_top.csv>\n";
        return 1;
    }

    const string input_file  = argv[1];
    const string output_file = argv[2];

    ifstream in(input_file);
    if (!in.is_open()) {
        cerr << "❌ No se pudo abrir: " << input_file << "\n";
        return 1;
    }

    string header;
    getline(in, header); // Saltar encabezado

    const size_t CHUNK_SIZE   = 1000000;   // Líneas por bloque
    const size_t BUCKET_LIMIT = 2'000'000; // Claves antes de flush

    vector<string> lines;
    lines.reserve(CHUNK_SIZE);

    unordered_map<string, uint64_t> global_bucket;
    omp_lock_t lock;
    omp_init_lock(&lock);

    priority_queue<Ruta, vector<Ruta>, Compare> heap;

    auto flush_bucket = [&]() {
        for (auto& [clave, cant] : global_bucket) {
            auto pos = clave.find(':');
            Ruta r = { clave.substr(0, pos), clave.substr(pos + 1), cant };
            heap.push(r);
            if (heap.size() > 10000) heap.pop();
        }
        global_bucket.clear();
    };

    while (in) {
        lines.clear();
        string line;
        for (size_t i = 0; i < CHUNK_SIZE && getline(in, line); ++i) {
            lines.emplace_back(move(line));
        }
        if (lines.empty()) break;

        #pragma omp parallel
        {
            unordered_map<string, uint64_t> local_map;

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
                if (start < L.size()) campos[5] = L.substr(start);

                if (!campos[4].empty() && !campos[5].empty()) {
                    string key = campos[4] + ":" + campos[5];
                    local_map[key]++;
                }
            }

            omp_set_lock(&lock);
            for (auto& [k, v] : local_map) {
                global_bucket[k] += v;
            }
            omp_unset_lock(&lock);
        }

        if (global_bucket.size() >= BUCKET_LIMIT) {
            cout << "🔄 Bucket alcanzó " << global_bucket.size() << " claves, vaciando al heap...\n";
            flush_bucket();
        }
    }

    in.close();
    flush_bucket();
    omp_destroy_lock(&lock);

    // Ordenar resultado final
    vector<Ruta> result;
    while (!heap.empty()) {
        result.push_back(heap.top());
        heap.pop();
    }
    reverse(result.begin(), result.end());

    ofstream out(output_file);
    out << "origen,destino,cantidad_viajes\n";
    for (auto& r : result) {
        out << r.origen << "," << r.destino << "," << r.cantidad << "\n";
    }
    out.close();

    cout << "✅ Top 10 000 rutas guardado en: " << output_file << "\n";
    return 0;
}
