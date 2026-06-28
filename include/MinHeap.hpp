#pragma once
#include <vector>
#include <string>
#include <algorithm>

struct HeapNode {
    std::string palavra;
    float pontuacao;

};

class MinHeap {
private:

    std::vector<HeapNode> heap; 
    int capacidade; 

    void siftUp(int i) {

        while (i > 0) {
            int pai = (i - 1) / 2; 
            if (heap[i].pontuacao < heap[pai].pontuacao) {
        
                std::swap(heap[i], heap[pai]); 
                i = pai;

            } else {
                
                break; 
            
            }
        }
    };

    void heapify(int i){

        int tamanho = heap.size();

        while (true){

            int esquerda = 2 * i + 1; 
            int direita = 2 * i + 2; 
            int menor = i; 

            if (esquerda < tamanho && heap[esquerda].pontuacao < heap[menor].pontuacao) {
                menor = esquerda; 
            }
            if (direita < tamanho && heap[direita].pontuacao < heap[menor].pontuacao) {
                menor = direita; 
            }

            if (menor != i) {
                std::swap(heap[i], heap[menor]); 
                i = menor; 
            } else {
                break; 
            }

        }

    };


public:
    
    MinHeap(int cap){
        capacidade = cap;
    }

    void insert(const std::string& palavra, float pontuacao) {
        
        if (heap.size() < capacidade) {

            heap.push_back({palavra, pontuacao});
            siftUp(heap.size() - 1); 

        } 
        else if (pontuacao > heap[0].pontuacao) {
            heap[0] = {palavra, pontuacao};
            heapify(0); 

        }
    };

    std::vector<HeapNode> getSorted() {

        std::vector<HeapNode> resultado = heap;
        std::sort(resultado.begin(), resultado.end(), [](const HeapNode& a, const HeapNode& b) {

            return a.pontuacao > b.pontuacao; 

        });

        return resultado; 

    };
};