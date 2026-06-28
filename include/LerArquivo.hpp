#pragma once
#include <string>

// para poder acessar os métodos e atributos do AnalisadorDeNoticias
class AnalisadorDeNoticias;

class LerArquivo {

    public:
    
        static void lerTexto(const std::string& nomeArquivo, AnalisadorDeNoticias& analisador);
};