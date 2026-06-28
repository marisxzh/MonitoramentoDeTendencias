#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <sstream>

#include "AnalisadorDeNoticias.hpp"
#include "ProcessadorTexto.hpp"

using namespace std;

// Reconstrói o texto original da manchete a partir dos tokens 
static string reconstruirTexto(const vector<string>& palavras) {
    string resultado;
    for (int i = 0; i < (int)palavras.size(); i++) {
        if (i > 0) resultado += " ";
        resultado += palavras[i];
    }
    return resultado;
}

// Função que exibe as 100 palavras mais frequentes do corpus em ordem decrescente.
void AnalisadorDeNoticias::gerarTop100Frequentes() {
    MinHeap top100Heap(100);

    for (const auto& item : dicionario) {
        top100Heap.insert(item.first, (float)item.second.freqGlobal);
    }

    vector<HeapNode> top100 = top100Heap.getSorted();

    cout << "\n[TOP-100 PALAVRAS MAIS FREQUENTES]\n";
    cout << string(60, '-') << "\n";
    cout << left << setw(6)  << "RANK"
         << setw(30) << "PALAVRA"
         << setw(10) << "FREQUENCIA" << "\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)top100.size(); i++) {
        cout << left << setw(6)  << i + 1
             << setw(30) << top100[i].palavra
             << setw(10) << (int)top100[i].pontuacao << "\n";
    }
}

// Função que exibe as 100 palavras com maior taxa de crescimento entre a janela 1 e a janela 5.
void AnalisadorDeNoticias::gerarTop100Emergentes() {
    MinHeap top100Heap(100);

   for (const auto& item : dicionario) {
        const string& palavra = item.first;
        float freq1 = item.second.freqJanela[0];
        float freq5 = item.second.freqJanela[4];
        float crescimento = (freq5 - freq1) / (freq1 + 1.0f);
        top100Heap.insert(palavra, crescimento);
    }

    top100Emergentes = top100Heap.getSorted();

    setEmergentes.clear();
    for (const auto& e : top100Emergentes) {
        setEmergentes.insert(e.palavra);
    }

    cout << "\n[TOP-100 PALAVRAS EMERGENTES - C(p) = (FJ5 - FJ1) / (FJ1 + 1)]\n";
    cout << string(60, '-') << "\n";
    cout << left << setw(6)  << "RANK"
         << setw(25) << "PALAVRA"
         << setw(12) << "C(p)"
         << setw(10) << "FREQ_J1"
         << setw(10) << "FREQ_J5" << "\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)top100Emergentes.size(); i++) {
        const string& palavra = top100Emergentes[i].palavra;
        int freq1 = dicionario[palavra].freqJanela[0];
        int freq5 = dicionario[palavra].freqJanela[4];

        cout << left  << setw(6)  << i + 1
             << setw(25) << palavra
             << fixed << setprecision(4) << setw(12) << top100Emergentes[i].pontuacao
             << setw(10) << freq1
             << setw(10) << freq5 << "\n";
    } 
}

void AnalisadorDeNoticias::encontrarTop10Similares(int idAlvo) {

    cout << "\n[ANALISE DE SIMILARIDADE - JACCARD]\n";
    cout << string(60, '-') << "\n";

    if (idAlvo < 0 || idAlvo >= (int)manchetes.size()) {
        cout << "  Manchete invalida!\n";
        return;
    }

    const vector<string>& palavrasAlvo = manchetes[idAlvo].palavras;

    cout << "  Manchete alvo (indice " << idAlvo << "): [ ";
    for (const string& tok : palavrasAlvo) cout << tok << " ";
    cout << "]\n\n";

    unordered_map<int, int> contagemIntersecao;

    for (const string& palavra : palavrasAlvo) {
        auto it = indiceInvertido.find(palavra);
        if (it == indiceInvertido.end()) continue;

        for (int outroId : it->second) {
            if (outroId != idAlvo) {
                contagemIntersecao[outroId]++;
            }
        }
    }

    const float THRESHOLD = 0.70f;
    MinHeap top10Heap(10);

    for (const auto& item : contagemIntersecao) {
        int outroId    = item.first;
        int intersecao = item.second; 
        const vector<string>& palavrasCandidato = manchetes[outroId].palavras;
        int tamanhoUniao = (int)palavrasAlvo.size() + (int)palavrasCandidato.size() - intersecao;
        float jaccard = (float)intersecao / (float)tamanhoUniao;

        if (jaccard >= THRESHOLD) {
            top10Heap.insert(to_string(outroId), jaccard);
        }
    }

    vector<HeapNode> top10 = top10Heap.getSorted();

    cout << "[RESULTADOS DA BUSCA - TOP 10 SIMILARES]\n";
    cout << string(60, '-') << "\n";

    if (top10.empty()) {
        cout << "  Nenhuma manchete similar encontrada com Jaccard >= " << THRESHOLD << ".\n";
    } else {
        // Converte top100Emergentes para um set para busca super rápida das keywords
        unordered_set<string> setEmergentes;
        for (const auto& e : top100Emergentes) {
            setEmergentes.insert(e.palavra);
        }

        for (const auto& no : top10) {
            int id = stoi(no.palavra);
            
            cout << fixed << setprecision(4) << "Jaccard: " << no.pontuacao 
                 << " | ID: " << id << "\n";
            
            cout << "Tokens: [ ";
            for (const string& tok : manchetes[id].palavras) {
                cout << tok << " ";
            }
            cout << "]\n";

            cout << "Keywords (Emergentes): ";
            bool achouEmergente = false;
            for (const string& tok : manchetes[id].palavras) {
                if (setEmergentes.count(tok)) {
                    cout << tok << " ";
                    achouEmergente = true;
                }
            }
            if (!achouEmergente) {
                cout << "(nenhuma)";
            }
            cout << "\n" << string(60, '-') << "\n";
        }
    }
}

// Lê um .txt com títulos (um por linha), tokeniza cada um com ProcessadorTexto
// e imprime no stream 'saida' as manchetes mais similares do corpus via Jaccard.
void AnalisadorDeNoticias::encontrarSimilaresPorTitulos(const string& arquivoTxt, ostream& saida) {

    ifstream arquivo(arquivoTxt);
    if (!arquivo.is_open()) {
        saida << "Erro: nao foi possivel abrir o arquivo \"" << arquivoTxt << "\".\n";
        return;
    }


    saida << "\n[ANALISE DE SIMILARIDADE POR ARQUIVO DE TITULOS]\n";
    saida << string(70, '=') << "\n";

    string linha;
    int numTitulo = 0;

    while (getline(arquivo, linha)) {
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty()) continue;

        numTitulo++;

        vector<string> palavras;
        palavras.reserve(20);
        ProcessadorTexto::limparTexto(linha, 0, palavras);

        sort(palavras.begin(), palavras.end());
        palavras.erase(unique(palavras.begin(), palavras.end()), palavras.end());

        saida << "\nTitulo " << numTitulo << ": " << linha << "\n";
        saida << "Tokens : [ ";
        for (const string& tok : palavras) saida << tok << " ";
        saida << "]\n";
        saida << string(70, '-') << "\n";

        if (palavras.empty()) {
            saida << "  Nenhum token util encontrado no titulo.\n";
            saida << string(70, '=') << "\n";
            continue;
        }

        unordered_map<int, int> contagemIntersecao;
        for (const string& palavra : palavras) {
            auto it = indiceInvertido.find(palavra);
            if (it == indiceInvertido.end()) continue;
            for (int outroId : it->second) {
                contagemIntersecao[outroId]++;
            }
        }

        const float THRESHOLD = 0.70f;
        MinHeap top10Heap(10);

        for (const auto& item : contagemIntersecao) {
            int outroId    = item.first;
            int intersecao = item.second;

            const vector<string>& palavrasCandidato = manchetes[outroId].palavras;
            int tamanhoUniao = (int)palavras.size()
                             + (int)palavrasCandidato.size()
                             - intersecao;

            float jaccard = (float)intersecao / (float)tamanhoUniao;

            if (jaccard <= THRESHOLD) {
                top10Heap.insert(to_string(outroId), jaccard);
            }
        }

        vector<HeapNode> top10 = top10Heap.getSorted();

        if (top10.empty()) {
            saida << "  Nenhuma manchete similar encontrada com Jaccard <= "
                  << fixed << setprecision(2) << THRESHOLD << ".\n";
        } else {
            saida << left << setw(8) << "RANK"
                  << setw(10) << "JACCARD"
                  << setw(8)  << "ID"
                  << "MANCHETE\n";
            saida << string(70, '-') << "\n";

            for (int i = 0; i < (int)top10.size(); i++) {
                int id = stoi(top10[i].palavra);

                // Reconstroi texto legivel a partir dos tokens
                string texto;
                for (const string& tok : manchetes[id].palavras) {
                    if (!texto.empty()) texto += " ";
                    texto += tok;
                }

                saida << left << setw(8) << (i + 1)
                      << fixed << setprecision(4) << setw(10) << top10[i].pontuacao
                      << setw(8) << id
                      << texto << "\n";

                // Keywords emergentes presentes nesta manchete
                saida << "        Keywords emergentes: ";
                bool achou = false;
                for (const string& tok : manchetes[id].palavras) {
                    if (setEmergentes.count(tok)) {
                        saida << tok << " ";
                        achou = true;
                    }
                }
                if (!achou) saida << "(nenhuma)";
                saida << "\n";
            }
        }

        saida << string(70, '=') << "\n";
    }

    arquivo.close();

    if (numTitulo == 0) {
        saida << "  Arquivo vazio ou sem titulos validos.\n";
    } else {
        saida << "\nTotal de titulos processados: " << numTitulo << "\n";
    }
}
