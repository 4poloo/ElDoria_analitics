#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <omp.h>
#include <filesystem>

using namespace std;

// ✅ Hash para tuple<string,string,string>
struct tuple_hash {
    template <class T1, class T2, class T3>
    size_t operator() (const tuple<T1,T2,T3>& t) const {
        auto h1 = hash<T1>{}(get<0>(t));
        auto h2 = hash<T2>{}(get<1>(t));
        auto h3 = hash<T3>{}(get<2>(t));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

string clasificarGrupoEtario(int edad) {
    if (edad < 18) return "<18";
    if (edad <= 35) return "18-35";
    if (edad <= 60) return "36-60";
    return "60+";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "❌ Uso: " << argv[0] << " <archivo_entrada.txt> <archivo_salida.csv>\n";
        return 1;
    }

    string input_file = argv[1];
    string output_file = argv[2];

    ifstream infile(input_file);
    if (!infile.is_open()) {
        cerr << "❌ No se pudo abrir archivo.\n";
        return 1;
    }

    string header;
    getline(infile, header);
    vector<string> lines;
    string line;
    while (getline(infile, line)) lines.push_back(line);
    infile.close();

    using Key = tuple<string,string,string>;
    unordered_map<Key, uint64_t, tuple_hash> global_map;

    omp_lock_t lock;
    omp_init_lock(&lock);

    // 🔵 Paralelización: cada hilo agrupa especie, genero, grupo_etario
    #pragma omp parallel
    {
        unordered_map<Key, uint64_t, tuple_hash> local_map;

        #pragma omp for schedule(static)
        for (size_t i = 0; i < lines.size(); ++i) {
            stringstream ss(lines[i]);
            string campo, especie, genero;
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
                string grupo = clasificarGrupoEtario(edad);
                local_map[{especie, genero, grupo}]++;
            }
        }

        omp_set_lock(&lock);
        for (const auto& [key, count] : local_map) {
            global_map[key] += count;
        }
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);

    ofstream out(output_file);
    out << "especie,genero,grupo_etario,conteo\n";
    for (const auto& [key, count] : global_map) {
        out << get<0>(key) << "," << get<1>(key) << "," << get<2>(key) << "," << count << "\n";
    }
    out.close();

    cout << "✅ Distribución etaria generada: " << output_file << "\n";
    return 0;
}
