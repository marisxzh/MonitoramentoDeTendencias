#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>

#include "LerArquivo.hpp"
#include "AnalisadorDeNoticias.hpp"
#include "ProcessadorTexto.hpp" 

using namespace std;

void LerArquivo::lerTexto(const string& nomeArquivo, AnalisadorDeNoticias& analisador) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo!\n";
        return;
    }

    string linha;
    int idAtual = 0;
    int tamanhoJanela = 200000; 

    cout << "Lendo arquivo. Isso pode demorar um pouco...\n";

    getline(arquivo, linha); // Pula cabeçalho

    while (getline(arquivo, linha)) {
        size_t posicaoVirgula = linha.find(',');
        if (posicaoVirgula == string::npos) continue;
        
        vector<string> palavras;
        
        // chamando a função através da classe ProcessadorTexto
        ProcessadorTexto::limparTexto(linha, posicaoVirgula + 1, palavras);
        
        unordered_set<string> palavrasUnicas(palavras.begin(), palavras.end());

        // !!! precisa de melhoria, isso que esta "atrasando" o processamento
        // acessando o vetor 'manchetes' de dentro do objeto analisador
        analisador.manchetes.push_back({idAtual, palavras});

        int janelaAtual = idAtual / tamanhoJanela;
        if (janelaAtual > 4) janelaAtual = 4;

        // preenchendo os dicionários que estão dentro do objeto analisador
        for (const string& palavra : palavrasUnicas) {
            analisador.frequenciaGlobal[palavra]++;
            analisador.frequenciaJanelas[janelaAtual][palavra]++;
            analisador.indiceInvertido[palavra].push_back(idAtual);
        }

        idAtual++;
        
        if(idAtual % 100000 == 0) {
            cout << "Processadas " << idAtual << " linhas...\n";
        }
    }
    arquivo.close();
    cout << "Leitura concluida! Total de linhas: " << idAtual << "\n";
}