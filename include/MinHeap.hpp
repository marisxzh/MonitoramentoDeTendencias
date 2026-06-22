// está aqui para evitar a leitura multipla do mesmo arquivo, o que pode causar erros de compilação
#pragma once
#include <vector>
#include <string>
#include <algorithm>

// estrutura de nó que guarda a palavra e sua pontuação
struct HeapNode {
    std::string palavra;
    float pontuacao;

};

// classe MinHeap para armazenar e organizar os nós de acordo com suas pontuações
class MinHeap {
private:
    // vetor que armazena os nós do heap
    std::vector<HeapNode> heap; 
    int capacidade; // capacidade máxima do heap

    // método para subir o elemento se ele for menor que seu pai, para descobrir o pior dos melhores.
    void siftUp(int i) {

        while (i > 0) {
            int pai = (i - 1) / 2; // índice do pai
            if (heap[i].pontuacao < heap[pai].pontuacao) {
                
                // troca o elemento com seu pai
                std::swap(heap[i], heap[pai]); 
                // move para o índice do pai
                i = pai;

            } else {
                
                // se o elemento não for menor que seu pai, para de subir
                break; 
            
            }
        }
    };

    // método para descer o elemento se ele for maior que seus filhos, para descobrir o pior dos melhores.
    void heapify(int i){

        int tamanho = heap.size();

        while (true){

            int esquerda = 2 * i + 1; // índice do filho esquerdo
            int direita = 2 * i + 2; // índice do filho direito
            int menor = i; // assume que o menor é o elemento atual

            if (esquerda < tamanho && heap[esquerda].pontuacao < heap[menor].pontuacao) {
                menor = esquerda; // se o filho esquerdo for menor, atualiza o índice do menor
            }
            if (direita < tamanho && heap[direita].pontuacao < heap[menor].pontuacao) {
                menor = direita; // se o filho direito for menor, atualiza o índice do menor
            }

            // se o menor não for o elemento atual, troca e continua descendo
            if (menor != i) {
                // troca o elemento com o menor dos filhos
                std::swap(heap[i], heap[menor]); 
                // move para o índice do menor
                i = menor; 
            } else {
                // se o elemento não for maior que seus filhos, para de descer
                break; 
            }

        }

    };


public:
    
    // construtor que inicializa a capacidade do heap
    MinHeap(int cap){
        capacidade = cap;
    }

    // método para inserir um novo nó no heap
    void insert(const std::string& palavra, float pontuacao) {
        
        //se tem espaço no heap, insere o novo nó e sobe ele para a posição correta
        if (heap.size() < capacidade) {

            // adiciona o novo nó ao final do vetor
            heap.push_back({palavra, pontuacao});
            // sobe o novo nó para a posição correta
            siftUp(heap.size() - 1); 

        } 
        // se o heap já está cheio, verifica se a pontuação do novo nó é maior que a pontuação do nó com a menor pontuação (o topo do heap)
        else if (pontuacao > heap[0].pontuacao) {
            // substitui o nó com a menor pontuação pelo novo nó
            heap[0] = {palavra, pontuacao};
            // desce o novo nó para a posição correta
            heapify(0); 

        }
    };

    // retorna um vetor com os nós do heap ordenados por pontuação, do maior para o menor
    std::vector<HeapNode> getSorted() {

        // cria uma cópia do vetor do heap para ordenar sem modificar o heap original
        std::vector<HeapNode> resultado = heap;
        std::sort(resultado.begin(), resultado.end(), [](const HeapNode& a, const HeapNode& b) {

            // ordena do maior para o menor
            return a.pontuacao > b.pontuacao; 

        });

        return resultado; // retorna o vetor ordenado

    };
};