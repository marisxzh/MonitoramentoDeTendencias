#include <iostream>
#include <string>

// Incluímos todos os módulos que dividimos para que o main possa coordená-los
#include "AnalisadorDeNoticias.hpp"
#include "LerArquivo.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // Captura os argumentos via linha de comando ou assume os valores padrão
    string nomeArquivo = (argc >= 2) ? argv[1] : "dados/input.csv";
    int linhaConsulta = (argc >= 3) ? stoi(argv[2]) : 2;

    // 1. Instanciamos o cérebro de armazenamento (onde ficarão as tabelas hash e o vetor)
    AnalisadorDeNoticias analisador;
    
    // 2. Chamamos o módulo de leitura para abrir o arquivo e preencher o analisador
    // Note que passamos 'analisador' por referência para que as tabelas hash sejam povoadas
    LerArquivo::lerTexto(nomeArquivo, analisador);
    
    // 3. Executamos as regras de negócio e os cálculos matemáticos do projeto
    analisador.gerarTop100Emergentes();
    analisador.encontrarTop10Similares(linhaConsulta);

    return 0;
}