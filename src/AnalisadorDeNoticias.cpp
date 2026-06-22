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

    for (const auto& item : frequenciaGlobal) {
        top100Heap.insert(item.first, (float)item.second);
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

    for (const auto& item : frequenciaGlobal) {
        const string& palavra = item.first;
        float freq1 = frequenciaJanelas[0].count(palavra) ? frequenciaJanelas[0].at(palavra) : 0;
        float freq5 = frequenciaJanelas[4].count(palavra) ? frequenciaJanelas[4].at(palavra) : 0;
        float crescimento = (freq5 - freq1) / (freq1 + 1.0f);
        top100Heap.insert(palavra, crescimento);
    }

    vector<HeapNode> top100 = top100Heap.getSorted();

    cout << "\n[TOP-100 PALAVRAS EMERGENTES - C(p) = (FJ5 - FJ1) / (FJ1 + 1)]\n";
    cout << string(60, '-') << "\n";
    cout << left << setw(6)  << "RANK"
         << setw(25) << "PALAVRA"
         << setw(12) << "C(p)"
         << setw(10) << "FREQ_J1"
         << setw(10) << "FREQ_J5" << "\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)top100.size(); i++) {
        const string& palavra = top100[i].palavra;
        int freq1 = frequenciaJanelas[0].count(palavra) ? frequenciaJanelas[0].at(palavra) : 0;
        int freq5 = frequenciaJanelas[4].count(palavra) ? frequenciaJanelas[4].at(palavra) : 0;

        cout << left  << setw(6)  << i + 1
             << setw(25) << palavra
             << fixed << setprecision(4) << setw(12) << top100[i].pontuacao
             << setw(10) << freq1
             << setw(10) << freq5 << "\n";
    }
}


/// Esta função precisa ser revisada.
// O cruzamento entre similaridade Jaccard e palavras emergentes não funciona
// corretamente quando a manchete alvo pertence a janelas mais antigas (ex: janela 1),
// pois as palavras emergentes são calculadas sempre entre janela 1 e janela 5.

void AnalisadorDeNoticias::encontrarTop10Similares(int idAlvo) {
    if (idAlvo >= (int)manchetes.size()) {
        cout << "\n[ANALISE DE SIMILARIDADE - JACCARD]\n";
        cout << string(60, '-') << "\n";
        cout << "  Manchete invalida!\n";
        return;
    }

    const vector<string>& palavrasAlvo = manchetes[idAlvo].palavras;
    unordered_set<string> conjuntoAlvo(palavrasAlvo.begin(), palavrasAlvo.end());

    // Conta intersecção via índice invertido
    unordered_map<int, int> contagemIntersecao;
    for (const string& palavra : conjuntoAlvo) {
        if (indiceInvertido.count(palavra)) {
            for (int outroId : indiceInvertido.at(palavra)) {
                if (outroId != idAlvo) contagemIntersecao[outroId]++;
            }
        }
    }

    // Calcula Jaccard e guarda os que passam do threshold
    MinHeap top10Heap(10);
    for (const auto& item : contagemIntersecao) {
        int outroId   = item.first;
        int intersecao = item.second;
        unordered_set<string> outroConjunto(manchetes[outroId].palavras.begin(),
                                            manchetes[outroId].palavras.end());
        int tamanhoUniao = (int)conjuntoAlvo.size() + (int)outroConjunto.size() - intersecao;
        float jaccard = (float)intersecao / tamanhoUniao;
        if (jaccard >= 0.25f) {  // threshold menor para mostrar resultados úteis
            top10Heap.insert(to_string(outroId), jaccard);
        }
    }

    vector<HeapNode> top10 = top10Heap.getSorted();

    // Coleta palavras emergentes do top100 para cruzamento
    MinHeap emergentesHeap(100);
    for (const auto& item : frequenciaGlobal) {
        const string& palavra = item.first;
        float freq1 = frequenciaJanelas[0].count(palavra) ? frequenciaJanelas[0].at(palavra) : 0;
        float freq5 = frequenciaJanelas[4].count(palavra) ? frequenciaJanelas[4].at(palavra) : 0;
        emergentesHeap.insert(palavra, (freq5 - freq1) / (freq1 + 1.0f));
    }
    vector<HeapNode> emergentes = emergentesHeap.getSorted();
    unordered_set<string> palavrasEmergentes;
    for (const auto& e : emergentes) palavrasEmergentes.insert(e.palavra);

    // --- Impressão ---
    cout << "\n[ANALISE DE SIMILARIDADE - JACCARD]\n";
    cout << string(60, '-') << "\n";
    cout << "  Manchete alvo (indice " << idAlvo << "): "
         << reconstruirTexto(palavrasAlvo) << "\n\n";

    cout << left << setw(8)  << "SCORE"
         << setw(10) << "INDICE"
         << "TOKENS\n";
    cout << string(60, '-') << "\n";

    if (top10.empty()) {
        cout << "  Nenhuma manchete similar encontrada.\n";
    } else {
        for (const auto& no : top10) {
            int id = stoi(no.palavra);
            cout << fixed << setprecision(4) << setw(8)  << no.pontuacao
                 << setw(10) << id
                 << reconstruirTexto(manchetes[id].palavras) << "\n";
        }
    }

    // Cruzamento: palavras emergentes presentes nas manchetes similares
    unordered_set<string> palavrasNicho;
    for (const auto& no : top10) {
        int id = stoi(no.palavra);
        for (const string& p : manchetes[id].palavras) palavrasNicho.insert(p);
    }
    // inclui também as palavras da manchete alvo
    for (const string& p : palavrasAlvo) palavrasNicho.insert(p);

    cout << "\n[PALAVRAS EMERGENTES NO NICHO]\n";
    cout << string(60, '-') << "\n";

    vector<string> keywords;
    for (const string& p : palavrasNicho) {
        if (palavrasEmergentes.count(p)) keywords.push_back(p);
    }

    if (keywords.empty()) {
        cout << "  Nenhuma palavra emergente neste nicho.\n";
    } else {
        for (const string& kw : keywords) cout << "  - " << kw << "\n";
    }
}