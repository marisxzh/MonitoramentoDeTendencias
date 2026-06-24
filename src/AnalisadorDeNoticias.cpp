#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <sstream>

#include "AnalisadorDeNoticias.hpp"

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
        // Acessa a freqGlobal dentro da nossa nova struct
        top100Heap.insert(item.first, (float)item.second.freqGlobal);
    }

    //?fim pedro
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
        // Pega direto da struct, sem usar map.count() ou map.at()
        float freq1 = item.second.freqJanela[0];
        float freq5 = item.second.freqJanela[4];
        float crescimento = (freq5 - freq1) / (freq1 + 1.0f);
        top100Heap.insert(palavra, crescimento);
    }

    top100Emergentes = top100Heap.getSorted();

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

    // Não criamos mais "unordered_set" aqui! Usamos o vetor ordenado diretamente.
    const vector<string>& palavrasAlvo = manchetes[idAlvo].palavras;

    // Apenas para exibir a manchete alvo no relatório imprimindo os tokens puros
    cout << "  Manchete alvo (indice " << idAlvo << "): [ ";
    for (const string& tok : palavrasAlvo) cout << tok << " ";
    cout << "]\n\n";

    unordered_map<int, int> contagemIntersecao;

    // Localiza os candidatos no Índice Invertido
    for (const string& palavra : palavrasAlvo) {
        auto it = indiceInvertido.find(palavra);
        if (it == indiceInvertido.end()) continue;

        for (int outroId : it->second) {
            if (outroId != idAlvo) {
                contagemIntersecao[outroId]++;
            }
        }
    }

    // Exigência do PDF do professor: Threshold de 70% (0.70)
    const float THRESHOLD = 0.70f;
    MinHeap top10Heap(10);

    for (const auto& item : contagemIntersecao) {
        int outroId    = item.first;
        int intersecao = item.second; // Quantidade de palavras em comum

        const vector<string>& palavrasCandidato = manchetes[outroId].palavras;
        
        // Jaccard: Interseção / União.  União = |A| + |B| - Interseção
        int tamanhoUniao = (int)palavrasAlvo.size() + (int)palavrasCandidato.size() - intersecao;

        // O verdadeiro Índice de Jaccard
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
            
            // Imprime a pontuação e o ID
            cout << fixed << setprecision(4) << "Jaccard: " << no.pontuacao 
                 << " | ID: " << id << "\n";
            
            // Imprime os tokens puros (atendendo à restrição de memória do PDF)
            cout << "Tokens: [ ";
            for (const string& tok : manchetes[id].palavras) {
                cout << tok << " ";
            }
            cout << "]\n";

            // Encontra e imprime as Keywords Emergentes NESTA manchete específica (Ação Integrada do PDF)
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

//?fim pedro