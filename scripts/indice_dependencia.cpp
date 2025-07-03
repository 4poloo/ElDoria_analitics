#include <iostream>
#include <fstream>
#include <sstream>
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
        cerr << "❌ No se pudo abrir archivo.\n";
        return 1;
    }

    string header;
    getline(infile, header);
    vector<string> lines;
    string line;
    while (getline(infile, line)) lines.push_back(line);
    infile.close();

    uint64_t menores15 = 0, mayores64 = 0, trabajadores = 0;

    // 🔵 Paralelización: suma directa por edades
    #pragma omp parallel for reduction(+:menores15,mayores64,trabajadores) schedule(static)
    for (size_t i = 0; i < lines.size(); ++i) {
        stringstream ss(lines[i]);
        string campo;
        int edad = -1;
        int col = 0;

        while (getline(ss, campo, ';')) {
            if (col == 2) {
                try { edad = stoi(campo); } catch (...) {}
                break;
            }
            col++;
        }

        if (edad < 0) continue;
        if (edad < 15) menores15++;
        else if (edad > 64) mayores64++;
        else trabajadores++;
    }

    double indice = (double)(menores15 + mayores64) / trabajadores;

    ofstream out(output_file);
    out << "menores_15,mayores_64,trabajadores,indice_dependencia\n";
    out << menores15 << "," << mayores64 << "," << trabajadores << "," << indice << "\n";
    out.close();

    cout << "✅ Índice de dependencia generado: " << output_file << "\n";
    return 0;
}
