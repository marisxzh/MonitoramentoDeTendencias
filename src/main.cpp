#include <iostream>
#include <string>

// incluindo os módulos necesssários
#include "AnalisadorDeNoticias.hpp"
#include "LerArquivo.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // captura os argumentos via linha de comando ou assume os valores padrão
    string nomeArquivo = (argc >= 2) ? argv[1] : "dados/input.csv";
    int linhaConsulta = (argc >= 3) ? stoi(argv[2]) : 2;

    // instancindo o objeto do analisador de notícias, que irá armazenar as manchetes e os dicionários para análise
    AnalisadorDeNoticias analisador;
    
    // lendo o arquivo e preenchendo os dados no objeto analisador, para preparar as informações para as próximas análises
    LerArquivo::lerTexto(nomeArquivo, analisador);
    
    //?Pedro
    analisador.gerarTop100Frequentes();
    analisador.gerarTop100Emergentes();
    analisador.encontrarTop10Similares(linhaConsulta);


    return 0;
}