#pragma once
#include <string>
#include <vector>

class ProcessadorTexto {
public:
    
    // função para limpar o texto, removendo pontuação e convertendo para minúsculas, além de filtrar palavras irrelevantes, para preparar as manchetes para análise
    static void limparTexto(const std::string& linha, size_t posicaoInicial, std::vector<std::string>& palavrasDeSaida);
};