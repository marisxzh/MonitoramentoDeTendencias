#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>

#include "AnalisadorDeNoticias.hpp"
#include "LerArquivo.hpp"

using namespace std;
using namespace chrono;

template<typename S, typename F>
static void executarRedirecionado(S& destino, F funcao) {
    streambuf* bufOriginal = cout.rdbuf(destino.rdbuf());
    funcao();
    cout.rdbuf(bufOriginal);
}

static void imprimirTempo(const string& etapa, long long ms) {
    cout << "[OK] " << etapa << " em " << fixed << setprecision(2) << ms / 1000.0 << "s\n";
}

int main(int argc, char* argv[]) {
    string nomeArquivo    = (argc >= 2) ? argv[1] : "dados/input.csv";
    string arquivoTitulos = (argc >= 3) ? argv[2] : "dados/titulos.txt";

    ofstream saida("output.txt");
    if (!saida.is_open()) {
        cerr << "Erro ao criar output.txt\n";
        return 1;
    }

    cout << "=== Monitoramento de Tendencias ===\n";
    cout << "Corpus  : " << nomeArquivo << "\n";
    cout << "Titulos : " << arquivoTitulos << "\n\n";

    AnalisadorDeNoticias analisador;

    // Etapa 1: leitura e indexacao
    auto t0 = high_resolution_clock::now();
    LerArquivo::lerTexto(nomeArquivo, analisador);
    auto t1 = high_resolution_clock::now();
    imprimirTempo("Leitura e indexacao", duration_cast<milliseconds>(t1 - t0).count());

    ostringstream lixeira;

    // Etapa 2: Top 100 frequentes (necessario internamente, descartado do terminal)
    cout << "Gerando Top 100 frequentes...\n";
    auto t2 = high_resolution_clock::now();
    executarRedirecionado(lixeira, [&]() { analisador.gerarTop100Frequentes(); });
    auto t3 = high_resolution_clock::now();
    imprimirTempo("Top 100 frequentes", duration_cast<milliseconds>(t3 - t2).count());

    // Etapa 3: Top 100 emergentes (necessario para cruzamento, descartado do terminal)
    cout << "Gerando Top 100 emergentes...\n";
    auto t4 = high_resolution_clock::now();
    executarRedirecionado(lixeira, [&]() { analisador.gerarTop100Emergentes(); });
    auto t5 = high_resolution_clock::now();
    imprimirTempo("Top 100 emergentes", duration_cast<milliseconds>(t5 - t4).count());

    // Etapa 4: Similaridade por arquivo de titulos — resultado vai pro output.txt
    cout << "Calculando similaridade por titulos...\n";
    auto t6 = high_resolution_clock::now();
    analisador.encontrarSimilaresPorTitulos(arquivoTitulos, saida);
    auto t7 = high_resolution_clock::now();
    imprimirTempo("Similaridade por titulos", duration_cast<milliseconds>(t7 - t6).count());

    long long total = duration_cast<milliseconds>(t7 - t0).count();
    cout << "\nTempo total : " << fixed << setprecision(2) << total / 1000.0 << "s\n";
    cout << "Resultado   : output.txt\n";

    saida.close();
    return 0;
}