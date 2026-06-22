#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>

#include "LerArquivo.hpp"
#include "AnalisadorDeNoticias.hpp"
#include "ProcessadorTexto.hpp"

using namespace std;

// Lê o arquivo uma vez só para contar as linhas válidas.
// Isso permite calcular o tamanho de cada janela dinamicamente.
static long long contarLinhas(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return 0;

    string linha;
    getline(arquivo, linha); // pula cabeçalho

    long long total = 0;
    while (getline(arquivo, linha)) {
        if (linha.find(',') != string::npos) total++;
    }
    return total;
}

void LerArquivo::lerTexto(const string& nomeArquivo, AnalisadorDeNoticias& analisador) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo!\n";
        return;
    }

    // Buffer de 8MB: reduz chamadas ao sistema operacional na leitura
    vector<char> bufferLeitura(8 * 1024 * 1024);
    arquivo.rdbuf()->pubsetbuf(bufferLeitura.data(), bufferLeitura.size());

    // Passagem 1: conta linhas para calcular o tamanho de cada janela
    cout << "Contando linhas...\n";
    long long totalLinhas = contarLinhas(nomeArquivo);
    if (totalLinhas == 0) {
        cerr << "Arquivo vazio ou sem dados validos.\n";
        return;
    }

    long long tamanhoJanela = totalLinhas / 5;
    cout << "Total: " << totalLinhas << " linhas | Janela: " << tamanhoJanela << "\n";

    // Reserve: aloca memória para o vetor de manchetes de uma só vez
    analisador.manchetes.reserve(totalLinhas);

    // Passagem 2: processa as manchetes
    cout << "Lendo arquivo...\n";
    string linha;
    long long idAtual = 0;

    getline(arquivo, linha); // pula cabeçalho

    while (getline(arquivo, linha)) {
        size_t posicaoVirgula = linha.find(',');
        if (posicaoVirgula == string::npos) continue;

        vector<string> palavras;
        palavras.reserve(20); // manchetes raramente têm mais de 20 palavras úteis

        ProcessadorTexto::limparTexto(linha, posicaoVirgula + 1, palavras);

        unordered_set<string> palavrasUnicas(palavras.begin(), palavras.end());

        // move(): transfere o vetor em vez de copiar
        analisador.manchetes.push_back({(int)idAtual, std::move(palavras)});

        // Clamp no 4: o resto da divisão inteira cai na última janela
        int janelaAtual = (int)(idAtual / tamanhoJanela);
        if (janelaAtual > 4) janelaAtual = 4;

        for (const string& palavra : palavrasUnicas) {
            analisador.frequenciaGlobal[palavra]++;
            analisador.frequenciaJanelas[janelaAtual][palavra]++;
            analisador.indiceInvertido[palavra].push_back((int)idAtual);
        }

        idAtual++;

        if (idAtual % 100000 == 0) {
            cout << "Processadas " << idAtual << " linhas...\n";
        }
    }

    arquivo.close();
    cout << "Leitura concluida! Total: " << idAtual << " linhas.\n";
}