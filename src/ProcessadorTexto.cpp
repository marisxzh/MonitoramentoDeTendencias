#include <cctype>
#include "ProcessadorTexto.hpp"

using namespace std;

void ProcessadorTexto::limparTexto(const string& linha, size_t posicaoInicial, vector<string>& palavrasDeSaida){

    string palavraAtual;
    palavraAtual.reserve(32);

    for (size_t i = posicaoInicial; i < linha.size(); ++i) {
        char c = linha[i];

        if (isalpha(c)) {
            palavraAtual += (char)tolower(c);

        } else {
            if (palavraAtual.length() > 3) {
                palavrasDeSaida.push_back(palavraAtual);
            }
            palavraAtual.clear();
        }
    }

    if (palavraAtual.length() > 3) {
        palavrasDeSaida.push_back(palavraAtual);
    }
}