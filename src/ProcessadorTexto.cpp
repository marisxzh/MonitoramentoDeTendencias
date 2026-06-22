#include <cctype>
#include "ProcessadorTexto.hpp"

using namespace std;

void ProcessadorTexto::limparTexto(const string& linha, size_t posicaoInicial, vector<string>& palavrasDeSaida){

    string palavraAtual = "";

    for (size_t i = posicaoInicial; i < linha.size(); ++i) {
        char c = linha[i];

        // se o caractere for alfabético, adiciona à palavra atual
        if (isalpha(c)) {
            // converte para minúscula
            palavraAtual += tolower(c);
             
        } else {
            if(palavraAtual.length() > 3){
                // adiciona a palavra ao vetor de saída
                palavrasDeSaida.push_back(palavraAtual);
            }
            palavraAtual = ""; // limpa a palavra atual para a próxima
        }
    }

    if (palavraAtual.length() > 3) {
        // adiciona a última palavra ao vetor de saída, se houver
        palavrasDeSaida.push_back(palavraAtual);
    }
}