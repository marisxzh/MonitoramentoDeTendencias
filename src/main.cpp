#include "AnalisadorDeNoticias.hpp"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    string nomeArquivo = (argc >= 2) ? argv[1] : "dados/input.csv";
    int linhaConsulta = (argc >= 3) ? stoi(argv[2]) : 12;

    AnalisadorDeNoticias analisador;
    
    analisador.processarArquivo(nomeArquivo);
    analisador.gerarTop100Emergentes();
    analisador.encontrarTop10Similares(linhaConsulta);

    return 0;
}