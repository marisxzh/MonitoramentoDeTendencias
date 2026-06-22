#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "AnalisadorDeNoticias.hpp"
#include "LerArquivo.hpp"

using namespace std;
using namespace chrono;

template<typename F>
static void executarParaArquivo(ofstream& saida, F funcao) {
    streambuf* bufOriginal = cout.rdbuf(saida.rdbuf());
    funcao();
    cout.rdbuf(bufOriginal);
}

static void imprimirTempo(const string& etapa, long long ms) {
    cout << "[OK] " << etapa << " em " << ms << "ms\n";
}

int main(int argc, char* argv[]) {
    string nomeArquivo = (argc >= 2) ? argv[1] : "dados/input.csv";
    int linhaConsulta  = (argc >= 3) ? stoi(argv[2]) : 2;

    ofstream saida("resultados.txt");
    if (!saida.is_open()) {
        cerr << "Erro ao criar resultados.txt\n";
        return 1;
    }

    // Cabeçalho do relatório
    saida << string(60, '=') << "\n";
    saida << "  NEWS TRENDS MONITOR - RELATORIO DE ANALISE\n";
    saida << "  Dataset: A Million News Headlines (ABC Australia)\n";
    saida << "  CEFET-MG - AEDS\n";
    saida << string(60, '=') << "\n";

    cout << "=== Monitoramento de Tendencias ===\n";
    cout << "Arquivo : " << nomeArquivo << "\n";
    cout << "Consulta: linha " << linhaConsulta << "\n\n";

    AnalisadorDeNoticias analisador;

    // Etapa 1: leitura
    auto t0 = high_resolution_clock::now();
    LerArquivo::lerTexto(nomeArquivo, analisador);
    auto t1 = high_resolution_clock::now();
    long long msLeitura = duration_cast<milliseconds>(t1 - t0).count();
    imprimirTempo("Leitura e indexacao", msLeitura);

    // Sumário
    saida << "\n[SUMARIO]\n";
    saida << string(60, '-') << "\n";
    saida << "  Total de manchetes : " << analisador.manchetes.size() << "\n";
    saida << "  Janelas temporais  : 5 (aprox. "
          << analisador.manchetes.size() / 5 << " manchetes cada)\n";
    saida << "  Manchete alvo      : indice " << linhaConsulta << "\n";

    // Etapa 2: Top 100 frequentes
    cout << "Gerando Top 100 frequentes...\n";
    auto t2 = high_resolution_clock::now();
    executarParaArquivo(saida, [&]() { analisador.gerarTop100Frequentes(); });
    auto t3 = high_resolution_clock::now();
    imprimirTempo("Top 100 frequentes", duration_cast<milliseconds>(t3 - t2).count());

    // Etapa 3: Top 100 emergentes
    cout << "Gerando Top 100 emergentes...\n";
    auto t4 = high_resolution_clock::now();
    executarParaArquivo(saida, [&]() { analisador.gerarTop100Emergentes(); });
    auto t5 = high_resolution_clock::now();
    imprimirTempo("Top 100 emergentes", duration_cast<milliseconds>(t5 - t4).count());

    // Etapa 4: Similaridade
    cout << "Calculando similaridade para linha " << linhaConsulta << "...\n";
    auto t6 = high_resolution_clock::now();
    executarParaArquivo(saida, [&]() { analisador.encontrarTop10Similares(linhaConsulta); });
    auto t7 = high_resolution_clock::now();
    imprimirTempo("Similaridade Jaccard", duration_cast<milliseconds>(t7 - t6).count());

    // Rodapé
    saida << "\n" << string(60, '=') << "\n";
    saida << "  Fim do relatorio\n";
    saida << string(60, '=') << "\n";

    long long total = duration_cast<milliseconds>(t7 - t0).count();
    cout << "\nTempo total : " << total << "ms\n";
    cout << "Resultado   : resultados.txt\n";

    saida.close();
    return 0;
}