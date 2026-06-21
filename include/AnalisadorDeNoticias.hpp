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

    private:

        // vetor para armazenar as manchetes, como um banco de dados em memória
        std::vector<Manchete> manchetes;

        // Dicionários

        
        // armazena a frequência global de cada palavra
        std::unordered_map<std::string, int> frequenciaGlobal;
        // array de 5 dicionarios, cada posição representa uma janela temporal do texto
        std::unordered_map<std::string, int> frequenciaJanelas[5]; 

        // Índice Invertido
        // associa uma palavra a uma lista de IDs de manchetes onde ela aparece, para facilitar a busca e contagem de palavras em cada manchete
        std::unordered_map<std::string, std::vector<int>> indiceInvertido;


        // função para limpar o texto, removendo pontuação e convertendo para minúsculas, além de filtrar palavras irrelevantes, para preparar as manchetes para análise
        void limparTexto(std::string &linha, size_t posicaoInicial, std::vector<std::string> &palavraDeSaida);


    public:

        // método para ler as manchetes do arquivo, processar e armazenar as informações necessárias para análise
        void processarArquivo(const std::string nomeArquivo);


        // aplica a fórmua matemática de crescimento comparando a janela 4 com a 0, para identificar as palavras que estão crescendo mais rapidamente, e armazena as 100 palavras com maior crescimento em um heap mínimo, para facilitar a recuperação das palavras mais emergentes
        void gerarTop100Emergentes();

        // calcula o indice de Jaccard para encontrar as nóticias mais similares com a do ID informado pelo usuário
        void encontrarTop10Similares(int idAlvo);


};