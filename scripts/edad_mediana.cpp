#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

const int NUM_THREADS = 4;

int clamp(int edad) {
    if (edad < 0) return 0;
    if (edad > 120) return 120;
    return edad;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "❌ Uso: " << argv[0] << " <entrada> <salida>\n";
        return 1;
    }

    string entrada = argv[1];
    string salida = argv[2];

    ifstream in(entrada);
    if (!in) {
        cerr << "❌ Error al abrir entrada\n";
        return 1;
    }

    string header;
    getline(in, header);

    vector<string> lines;
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }
    in.close();

    cout << "🔄 Total líneas: " << lines.size() << "\n";

    unordered_map<string, vector<int>> global_hist;

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        unordered_map<string, vector<int>> local_hist;

        #pragma omp for schedule(dynamic)
        for (size_t i = 0; i < lines.size(); ++i) {
            stringstream ss(lines[i]);
            vector<string> campos;
            string campo;
            while (getline(ss, campo, ';')) campos.push_back(campo);
            if (campos.size() < 3) continue;

            string especie = campos[0];
            string genero = campos[1];
            int edad = clamp(stoi(campos[2]));
            string key = especie + "_" + genero;

            if (local_hist[key].empty()) local_hist[key].resize(121, 0);
            local_hist[key][edad]++;
        }

        #pragma omp critical
        {
            for (auto& [key, hist] : local_hist) {
                if (global_hist[key].empty()) global_hist[key].resize(121, 0);
                for (int i = 0; i <= 120; ++i) {
                    global_hist[key][i] += hist[i];
                }
            }
        }
    }

    ofstream out(salida);
    out << "especie,genero,edad_mediana_aprox\n";

    for (const auto& [key, hist] : global_hist) {
        int total = 0;
        for (int f : hist) total += f;

        int cum = 0;
        int mediana = 0;
        for (int i = 0; i <= 120; ++i) {
            cum += hist[i];
            if (cum >= total / 2) {
                mediana = i;
                break;
            }
        }

        size_t sep = key.find("_");
        string especie = key.substr(0, sep);
        string genero = key.substr(sep + 1);

        out << especie << "," << genero << "," << mediana << "\n";
    }

    out.close();
    cout << "✅ Edad mediana aproximada generada en: " << salida << "\n";
    return 0;
}
