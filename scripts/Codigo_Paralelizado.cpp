// ==========================================
// 📂 codigo_paralelizado.cpp (Optimizado)
// ==========================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <omp.h>
#include <filesystem>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using namespace std;
using namespace chrono;
namespace fs = filesystem;

const size_t BLOCK_SIZE = 50000;
const int NUM_THREADS = 8;
const int PROGRESS_INTERVAL = 50000;

struct RegistroCompleto {
    string especie, genero, cp, fecha, origen, destino;
    int edad, estrato;

    RegistroCompleto(string&& esp, string&& gen, string&& c, string&& f, string&& o, string&& d, int ed, int es)
        : especie(move(esp)), genero(move(gen)), cp(move(c)), fecha(move(f)), 
          origen(move(o)), destino(move(d)), edad(ed), estrato(es) {}
};

void printDebug(const string& m) {
    auto now = system_clock::to_time_t(system_clock::now());
    string ts = ctime(&now);
    ts.pop_back();
    cout << "\033[1;34m[DEBUG][" << ts << "] " << m << "\033[0m" << endl;
}

string clean(string s) {
    s.erase(remove(s.begin(), s.end(), '"'), s.end());
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

int calcularEdad(const string& f) noexcept {
    if (f.size() < 4) return -1;
    try { return 2025 - stoi(f.substr(0, 4)); } catch (...) { return -1; }
}

int calcularEstrato(const string& cp) noexcept {
    return cp.empty() ? -1 : cp[0] - '0';
}

vector<string> dividirLineas(const char* data, size_t size, size_t& pos) {
    vector<string> lns;
    lns.reserve(BLOCK_SIZE);
    while (pos < size && lns.size() < BLOCK_SIZE) {
        const char* start = data + pos;
        const char* end = static_cast<const char*>(memchr(start, '\n', size - pos));
        if (!end) end = data + size;
        lns.emplace_back(start, end - start);
        pos = end - data + 1;
    }
    return lns;
}

void procesarBloque(const vector<string>& lns, vector<RegistroCompleto>& res) {
    vector<vector<RegistroCompleto>> threadData(NUM_THREADS);

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < lns.size(); ++i) {
            if ((i+1) % PROGRESS_INTERVAL == 0) {
                double pct = (100.0 * (i+1)) / lns.size();
                #pragma omp critical
                printDebug("Progreso bloque: " + to_string(i+1) + "/" + to_string(lns.size()) + " (" + to_string(int(pct)) + "%)");
            }

            stringstream ss(lns[i]);
            vector<string> campos;
            string campo;
            while (getline(ss, campo, ';')) campos.push_back(clean(move(campo)));
            if (campos.size() < 8) continue;

            threadData[tid].emplace_back(
                move(campos[1]), move(campos[2]), "", move(campos[5]),
                move(campos[6]), move(campos[7]), 
                calcularEdad(campos[5]), calcularEstrato(campos[6])
            );
        }
    }

    for (auto& td : threadData) {
        res.insert(res.end(), make_move_iterator(td.begin()), make_move_iterator(td.end()));
    }
}

void exportarParcial(const vector<RegistroCompleto>& datos, const string& nombre) {
    ofstream out(nombre, ios::binary);
    out << "especie;genero;edad;estrato;origen;destino\n";
    for (const auto& r : datos) {
        out << r.especie << ";" << r.genero << ";" << r.edad << ";" 
            << r.estrato << ";" << r.origen << ";" << r.destino << "\n";
    }
    out.close();
}

void procesarArchivo(const string& input) {
    int fd = open(input.c_str(), O_RDONLY);
    if (fd == -1) { cerr << "❌ No se pudo abrir: " << input << endl; return; }

    size_t fileSize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printDebug("📦 Tamaño archivo: " + to_string(fileSize/1024/1024) + " MB");

    char* mapped = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0));
    if (mapped == MAP_FAILED) {
        close(fd); cerr << "❌ Error mmap\n"; return;
    }

    fs::create_directories("outputs/parciales");
    fs::create_directories("outputs");

    vector<string> archivosParciales;
    size_t pos = 0;
    const char* endHeader = static_cast<const char*>(memchr(mapped, '\n', fileSize));
    if (endHeader) pos = endHeader - mapped + 1;

    while (pos < fileSize) {
        auto lns = dividirLineas(mapped, fileSize, pos);
        if (lns.empty()) break;

        vector<RegistroCompleto> datos;
        procesarBloque(lns, datos);

        string parcial = "outputs/parciales/parcial_" + to_string(archivosParciales.size()) + ".csv";
        exportarParcial(datos, parcial);
        archivosParciales.push_back(parcial);
        printDebug("✅ Parcial: " + parcial + " (" + to_string(datos.size()) + ")");

        datos.clear();
    }

    munmap(mapped, fileSize);
    close(fd);

    printDebug("🔗 Combinando parciales...");

    ofstream final("outputs/datos_completos.txt", ios::binary);
    final << "especie;genero;edad;estrato;origen;destino\n";

    for (const auto& p : archivosParciales) {
        ifstream in(p);
        string tmp;
        getline(in, tmp);
        while (getline(in, tmp)) final << tmp << "\n";
        in.close();
    }
    final.close();

    printDebug("✅ Archivo final: outputs/datos_completos.txt");
}

int main() {
    omp_set_num_threads(NUM_THREADS);
    string input = "data/eldoria.csv";
    printDebug("🚀 Iniciando " + input);

    auto ini = high_resolution_clock::now();
    procesarArchivo(input);
    auto fin = high_resolution_clock::now();

    auto ms = duration_cast<milliseconds>(fin - ini).count();
    int h = ms / (1000*60*60); int m = (ms/60000)%60; int s = (ms/1000)%60;
    cout << "\n=====================================================\n";
    printDebug("✔ Finalizado en " + to_string(h) + "h " + to_string(m) + "m " + to_string(s) + "s (" + to_string(ms) + " ms)");
    cout << "=====================================================\n";

    return 0;
}
