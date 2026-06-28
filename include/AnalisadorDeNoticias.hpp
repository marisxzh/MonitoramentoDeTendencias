#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "MinHeap.hpp"

// estrutura para armazenar as manchetes, com um id e um vetor de palavras limpas e filtradas
struct Manchete {
 
    int id;
    std::vector<std::string> palavras;

};

class AnalisadorDeNoticias {

    public:

        std::vector<Manchete> manchetes;

        struct Estatisticas {
            int freqGlobal = 0;
            int freqJanela[5] = {0};
        };

        std::unordered_map<std::string, Estatisticas> dicionario;
        std::unordered_map<std::string, std::vector<int>> indiceInvertido;
        std::vector<HeapNode> top100Emergentes;
        std::unordered_set<std::string> setEmergentes;

        void gerarTop100Emergentes();
        void gerarTop100Frequentes();
        void encontrarTop10Similares(int idAlvo);
        void encontrarSimilaresPorTitulos(const std::string& arquivoTxt, std::ostream& saida);
};