#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm> //?pedro

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

        // 1. Ordena o vetor e remove as palavras duplicadas na própria memória!
        // (Isso mata o gargalo de criar "unordered_sets" atoa)
        sort(palavras.begin(), palavras.end());
        palavras.erase(unique(palavras.begin(), palavras.end()), palavras.end());

        // Clamp no 4: o resto da divisão inteira cai na última janela
        int janelaAtual = (int)(idAtual / tamanhoJanela);
        if (janelaAtual > 4) janelaAtual = 4;

        // 2. Itera sobre o próprio vetor de palavras (que agora não tem duplicatas)
        for (const string& palavra : palavras) {
            // guarda referência: acessa o dicionário UMA vez por palavra (era 2x antes)
            auto& estat = analisador.dicionario[palavra];
            estat.freqGlobal++;
            estat.freqJanela[janelaAtual]++;

            analisador.indiceInvertido[palavra].push_back((int)idAtual);
        }

        // 3. move(): transfere o vetor APÓS usá-lo, para evitar cópias desnecessárias
        analisador.manchetes.push_back({(int)idAtual, std::move(palavras)});

        //?fim pedro

        idAtual++;

        if (idAtual % 100000 == 0) {
            cout << "Processadas " << idAtual << " linhas...\r" << flush;
        }
    }

    arquivo.close();
    cout << "\r                              \r" << "Leitura concluida! Total: " << idAtual << " linhas.\n" << flush;
}