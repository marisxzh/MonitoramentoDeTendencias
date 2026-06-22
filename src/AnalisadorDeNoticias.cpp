#include <fstream>
#include <algorithm>
#include <cctype>

#include "AnalisadorDeNoticias.hpp"

using namespace std;


void AnalisadorDeNoticias::gerarTop100Emergentes() {
    // Inicialização do MinHeap
    MinHeap top100Heap(100);

    for (const auto& item : frequenciaGlobal) {
        string palavra = item.first;
        float freq1 = frequenciaJanelas[0][palavra]; 
        float freq5 = frequenciaJanelas[4][palavra]; 
        
        float crescimento = (freq5 - freq1) / (freq1 + 1.0);
        
        top100Heap.insert(palavra, crescimento);
    }

    // Busca do vetor já ordenado pelo Heap
    vector<HeapNode> top100 = top100Heap.getSorted();

    cout << "\n--- TOP 100 PALAVRAS EMERGENTES ---\n";
    for (int i = 0; i < top100.size(); i++) {
        cout << i + 1 << ". " << top100[i].palavra << " (Taxa: " << top100[i].pontuacao << ")\n";
    }
};


//?Pedro
void AnalisadorDeNoticias::gerarTop100Frequentes() {
    MinHeap top100Heap(100);

    for (const auto& item : frequenciaGlobal) {
        top100Heap.insert(item.first, (float)item.second);
    }

    vector<HeapNode> top100 = top100Heap.getSorted();

    cout << "\n--- TOP 100 PALAVRAS MAIS FREQUENTES ---\n";
    for (int i = 0; i < (int)top100.size(); i++) {
        cout << i + 1 << ". " << top100[i].palavra
             << " (" << (int)top100[i].pontuacao << " ocorrências)\n";
    }
}

void AnalisadorDeNoticias::encontrarTop10Similares(int idAlvo) {
    if (idAlvo >= manchetes.size()) {
        cout << "Manchete invalida!\n";
        return;
    }

    const vector<string>& palavrasAlvo = manchetes[idAlvo].palavras;
    unordered_set<string> conjuntoAlvo(palavrasAlvo.begin(), palavrasAlvo.end());

    unordered_map<int, int> contagemIntersecao;

    for (const string& palavra : conjuntoAlvo) {
        for (int outroId : indiceInvertido[palavra]) {
            if (outroId != idAlvo) {
                contagemIntersecao[outroId]++;
            }
        }
    }

    // MinHeap para garantir a ordenação parcial dos 10 melhores
    MinHeap top10Heap(10);

    for (const auto& item : contagemIntersecao) {
        int outroId = item.first;
        int intersecao = item.second;

        unordered_set<string> outroConjunto(manchetes[outroId].palavras.begin(), manchetes[outroId].palavras.end());
        
        int tamanhoUniao = conjuntoAlvo.size() + outroConjunto.size() - intersecao;
        float jaccard = (float)intersecao / tamanhoUniao;

        if (jaccard >= 0.70) {
            top10Heap.insert(to_string(outroId), jaccard);
        }
    }

    vector<HeapNode> top10 = top10Heap.getSorted();

    cout << "\n--- TOP SIMILARES (ALVO: MANCHETE " << idAlvo << ") ---\n";
    if (top10.empty()) {
        cout << "Nenhuma manchete com Jaccard >= 0.70 foi encontrada.\n";
    } else {
        for (int i = 0; i < top10.size(); i++) {
            cout << "Manchete Linha " << top10[i].palavra 
                 << " | Similaridade Jaccard: " << top10[i].pontuacao << "\n";
        }
    }
};