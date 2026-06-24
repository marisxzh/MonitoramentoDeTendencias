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

    //?PEDRO
    /*
    for (const auto& item : frequenciaGlobal) {
        top100Heap.insert(item.first, (float)item.second);
    }
    */
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

    //?pedro
    /*
    for (const auto& item : frequenciaGlobal) {
        const string& palavra = item.first;
        float freq1 = frequenciaJanelas[0].count(palavra) ? frequenciaJanelas[0].at(palavra) : 0;
        float freq5 = frequenciaJanelas[4].count(palavra) ? frequenciaJanelas[4].at(palavra) : 0;
        float crescimento = (freq5 - freq1) / (freq1 + 1.0f);
        top100Heap.insert(palavra, crescimento);
    }

    // salva o resultado no atributo da classe para reutilizar em encontrarTop10Similares
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
        int freq1 = frequenciaJanelas[0].count(palavra) ? frequenciaJanelas[0].at(palavra) : 0;
        int freq5 = frequenciaJanelas[4].count(palavra) ? frequenciaJanelas[4].at(palavra) : 0;

        cout << left  << setw(6)  << i + 1
             << setw(25) << palavra
             << fixed << setprecision(4) << setw(12) << top100Emergentes[i].pontuacao
             << setw(10) << freq1
             << setw(10) << freq5 << "\n";
    }
*/
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
//?fim pedro
}

//?PEDRO
    /*
void AnalisadorDeNoticias::encontrarTop10Similares(int idAlvo) {

    
    // Imprimir cabeçalho da seção de similaridade
    cout << "\n[ANALISE DE SIMILARIDADE - JACCARD]\n";
    cout << string(60, '-') << "\n";

    if (idAlvo < 0 || idAlvo >= (int)manchetes.size()) {
        cout << "  Manchete invalida!\n";
        return;
    }

    // manchetes[idAlvo].palavras é o vetor de tokens da manchete alvo, que será comparada com as demais.
    // usando o unordered_set para armazenar as palavras da manchete alvo, para busca O(1)
    const vector<string>& palavrasAlvo = manchetes[idAlvo].palavras;
    unordered_set<string> conjuntoAlvo(palavrasAlvo.begin(), palavrasAlvo.end());

    // apenas para exibir a manchete alvo no relatório
    cout << "  Manchete alvo (indice " << idAlvo << "): "
         << reconstruirTexto(palavrasAlvo) << "\n\n";

    // usa o índice invertido para encontrar candidatos.
    // contagemIntersecao[id] = quantas palavras em comum com a alvo
    // ---------------------------------------------------------------
    unordered_map<int, int> contagemIntersecao;

    // le palavra por palavra da manchete alvo, e para cada palavra, pega a lista de IDs de manchetes que contém essa palavra.
    for (const string& palavra : conjuntoAlvo) {
        // pega a lista de manchetes que contêm essa palavra
        // auto -> descobre o tipo necessário automaticamente (no caso, std::unordered_map<string, vector<int>>::iterator it)
        auto it = indiceInvertido.find(palavra);
        // se a palavra não estiver no índice invertido, ignora
        if (it == indiceInvertido.end()) continue;

        // se for encontrada, aponta para um par composto da palavra(alvo) e o vetor que contem todos os ids das manchetes que contém essa palavra
        for (int outroId : it->second) {
            if (outroId != idAlvo) {
                contagemIntersecao[outroId]++;
            }
        }
    }

    // Jaccard(A, B) = |A ∩ B| / |A ∪ B|
    // |A ∩ B| = contagemIntersecao[id]
    // |A ∪ B| = |A| + |B| - |A ∩ B|  (fórmula da inclusão-exclusão)
    const float THRESHOLD = 0.30f;
    // limita para os 10 mais similares
    MinHeap top10Heap(10);


    for (const auto& item : contagemIntersecao) {
        // item.first = id da manchete candidata
        int outroId    = item.first;
        // item.second = valor que está associado à chave
        int intersecao = item.second;

        // monta o conjunto da manchete candidata para calcular |B|
        unordered_set<string> conjuntoCandidato(
            // ver do inicio ao fim do vetor de palavras limpas da manchete candidata
            manchetes[outroId].palavras.begin(),
            manchetes[outroId].palavras.end()
        );

        // fórmula para calcular o tamanho da união dos conjuntos A e B
        int tamanhoUniao = (int)conjuntoAlvo.size() + (int)conjuntoCandidato.size() - intersecao;

        // calcula o índice de Jaccard
        float jaccard = (float)intersecao / (float)tamanhoUniao;

        // se o jaccard for maior que o threshold, insere no heap mínimo para manter os 10 maiores
        if (jaccard >= THRESHOLD) {
            // guardamos o id como string porque o HeapNode armazena string
            top10Heap.insert(to_string(outroId), jaccard);
        }
    }

    // organiza o vetor do heap em ordem decrescente de similaridade, para exibir do mais similar para o menos similar
    vector<HeapNode> top10 = top10Heap.getSorted();

    // imprimir as manchetes mais similares
    cout << left << setw(8)  << "SCORE"
         << setw(10) << "INDICE"
         << "TOKENS\n";
    cout << string(60, '-') << "\n";

    if (top10.empty()) {
        cout << "  Nenhuma manchete similar encontrada (threshold: "
             << THRESHOLD << ").\n";
    } else {
        for (const auto& no : top10) {
            int id = stoi(no.palavra);
            cout << fixed << setprecision(4) << setw(8)  << no.pontuacao
                 << setw(10) << id
                 << reconstruirTexto(manchetes[id].palavras) << "\n";
        }
    }


    // cria uma tabela hash vazia para armazenar as palavras da manchete alvo
    // depois faz um loop pelas 10 notícias mais parecidas e adiciona todas as palavras dessas notícias na tabela hash
    unordered_set<string> palavrasDoNicho;
    for (const string& p : palavrasAlvo) palavrasDoNicho.insert(p);
    for (const auto& no : top10) {
        // converte a palavra do heap (que é o id da manchete) para inteiro
        int id = stoi(no.palavra);
        for (const string& p : manchetes[id].palavras) palavrasDoNicho.insert(p);
    }

    // converte top100Emergentes para um set para busca O(1)
    unordered_set<string> setEmergentes;
    for (const auto& e : top100Emergentes) setEmergentes.insert(e.palavra);

    // filtra as palavras do nicho que são emergentes
    vector<string> keywords;
    for (const string& p : palavrasDoNicho) {
        if (setEmergentes.count(p)) keywords.push_back(p);
    }

    cout << "\n[PALAVRAS EMERGENTES NO NICHO]\n";
    cout << string(60, '-') << "\n";

    if (keywords.empty()) {
        cout << "  Nenhuma palavra emergente neste nicho.\n";
    } else {
        for (const string& kw : keywords) cout << "  - " << kw << "\n";
    }
   

}
 */


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