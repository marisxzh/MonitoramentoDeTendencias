#include <fstream>
#include <algorithm>
#include <cctype>

#include "AnalisadorDeNoticias.hpp"

using namespace std;

void AnalisadorDeNoticias::limparTexto(string& linha, size_t posicaoInicial, vector<string>& palavrasDeSaida){

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



void AnalisadorDeNoticias::processarArquivo(string nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    string linha;
    int idAtual = 0;
    int tamanhoJanela = 200000; 

    cout << "Lendo arquivo. Isso pode demorar um pouco...\n";

    getline(arquivo, linha); // Pula cabeçalho

    while (getline(arquivo, linha)) {
        size_t posicaoVirgula = linha.find(',');
        if (posicaoVirgula == string::npos) continue;
        
        vector<string> palavras;
        limparTexto(linha, posicaoVirgula + 1, palavras);
        
        unordered_set<string> palavrasUnicas(palavras.begin(), palavras.end());

        manchetes.push_back({idAtual, palavras});

        int janelaAtual = idAtual / tamanhoJanela;
        if (janelaAtual > 4) janelaAtual = 4;

        for (const string& palavra : palavrasUnicas) {
            frequenciaGlobal[palavra]++;
            frequenciaJanelas[janelaAtual][palavra]++;
            indiceInvertido[palavra].push_back(idAtual);
        }

        idAtual++;
        
        if(idAtual % 100000 == 0) {
            cout << "Processadas " << idAtual << " linhas...\n";
        }
    }
    cout << "Leitura concluida! Total de linhas: " << idAtual << "\n";
}

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
}