#pragma once
#include <string>

// para poder acessar os métodos e atributos do AnalisadorDeNoticias
class AnalisadorDeNoticias;

class LerArquivo {
public:

    // método para ler as manchetes do arquivo, processar e armazenar as informações necessárias para análise
    static void lerTexto(const std::string& nomeArquivo, AnalisadorDeNoticias& analisador);
};