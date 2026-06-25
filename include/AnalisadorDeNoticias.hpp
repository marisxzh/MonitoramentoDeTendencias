#pragma once
#include <iostream>
#include <vector>
#include <string>
// tabela hash para busca e contagem de palavras
#include <unordered_map>
//armazena apenas chaves únicas, para evitar contagem duplicada de palavras
#include <unordered_set>

#include "MinHeap.hpp"

// estrutura para armazenar as manchetes, com um id e um vetor de palavras limpas e filtradas
struct Manchete {
 
    int id;
    std::vector<std::string> palavras;

};


class AnalisadorDeNoticias {


    public:

        // vetor para armazenar as manchetes, como um banco de dados em memória
        std::vector<Manchete> manchetes;

        // Struct única para evitar múltiplos lookups na tabela Hash
        struct Estatisticas {
            int freqGlobal = 0;
            int freqJanela[5] = {0};
        };

        // Um único dicionário substitui os 6 antigos!
        std::unordered_map<std::string, Estatisticas> dicionario;

        //?fim pedro
        
        // Índice Invertido
        // associa uma palavra a uma lista de IDs de manchetes onde ela aparece, para facilitar a busca e contagem de palavras em cada manchete
        std::unordered_map<std::string, std::vector<int>> indiceInvertido;

        // guarda o resultado de gerarTop100Emergentes para ser reutilizado
        // pelo cruzamento de palavras emergentes na análise de similaridade
        std::vector<HeapNode> top100Emergentes;

        // set de emergentes pré-montado para cruzamento em O(1)
        // evita reconstruir o set a cada título consultado
        std::unordered_set<std::string> setEmergentes;

        // aplica a fórmua matemática de crescimento comparando a janela 4 com a 0, para identificar as palavras que estão crescendo mais rapidamente, e armazena as 100 palavras com maior crescimento em um heap mínimo, para facilitar a recuperação das palavras mais emergentes
        void gerarTop100Emergentes();


        //Pedro(?)
        void gerarTop100Frequentes();

        // calcula o indice de Jaccard para encontrar as nóticias mais similares com a do ID informado pelo usuário
        void encontrarTop10Similares(int idAlvo);

        // lê um arquivo .txt com títulos de manchetes (um por linha, sem tokenizar),
        // tokeniza cada título e lista as manchetes mais similares do corpus para cada um
        void encontrarSimilaresPorTitulos(const std::string& arquivoTxt, std::ostream& saida);


};