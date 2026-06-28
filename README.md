# Monitoramento de Tendências em Manchetes de Notícias

Sistema de análise de grandes volumes de dados textuais desenvolvido para a disciplina de **Algoritmos e Estruturas de Dados I — CEFET-MG**. O sistema processa a base *A Million News Headlines* para identificar palavras frequentes, detectar termos emergentes ao longo do tempo e calcular a similaridade entre manchetes.

&nbsp;

## 📋 Sumário

- Visão Geral
- Estrutura do Projeto
- Estruturas de Dados
- Etapas do Sistema
- Análise de Complexidade
- Formato de Entrada e Saída
- Compilação e Execução
- Autores

&nbsp;

## 🔭 Visão Geral

O sistema é organizado em quatro etapas sequenciais executadas a partir de dois arquivos de entrada:

```
input.csv     → corpus de 1 milhão de manchetes
titulos.txt   → títulos a serem consultados
```

```
Leitura → Top 100 Frequentes → Top 100 Emergentes → Similaridade → output.txt
```

Na primeira etapa, o corpus é lido e todas as estruturas de dados são construídas simultaneamente. Nas etapas seguintes, os rankings de palavras frequentes e emergentes são gerados. Por fim, para cada título consultado, o sistema localiza as 10 manchetes mais similares e cruza os resultados com as palavras emergentes identificadas.

&nbsp;

## 📂 Estrutura do Projeto

```
.
├── build/
│   ├── objects/     # Arquivos objeto (.o) gerados na compilação
│   └── app          # Executável final
├── dados/
│   ├── input.csv    # Corpus de manchetes (A Million News Headlines)
│   └── titulos.txt  # Arquivo de consultas (um título por linha)
├── include/
│   ├── AnalisadorDeNoticias.hpp
│   ├── LerArquivo.hpp
│   ├── MinHeap.hpp
│   └── ProcessadorTexto.hpp
├── src/
│   ├── AnalisadorDeNoticias.cpp
│   ├── LerArquivo.cpp
│   ├── main.cpp
│   └── ProcessadorTexto.cpp
├── output.txt       # Arquivo de saída gerado a cada execução
└── Makefile
└── README.md
```

&nbsp;

## 💿 Estruturas de Dados

### Dicionário — `unordered_map<string, Estatisticas>`

Tabela hash que mapeia cada palavra a uma struct `Estatisticas`, contendo:

```cpp
struct Estatisticas {
    int freqGlobal;       // frequência total no corpus
    int freqJanela[5];    // frequência em cada uma das 5 janelas temporais
};
```

A unificação dos dois contadores numa única struct garante que cada palavra seja acessada apenas uma vez por inserção — sem lookups duplicados na tabela.

### Índice Invertido — `unordered_map<string, vector<int>>`

Mapeia cada palavra a uma lista de IDs das manchetes em que ela aparece. Construído na mesma passagem do dicionário, é a estrutura central da etapa de similaridade: em vez de comparar todas as manchetes entre si (O(n²)), o índice reduz a busca apenas às manchetes que compartilham palavras com a consulta.

### Vetor de Manchetes — `vector<Manchete>`

```cpp
struct Manchete {
    int id;
    vector<string> palavras;  // apenas tokens filtrados, sem o texto bruto
};
```

Armazena as manchetes pré-processadas em memória. O texto bruto original é descartado após a extração dos tokens, em conformidade com a restrição do projeto.

### MinHeap — implementação própria

Heap mínimo de capacidade fixa `K`, utilizado em três contextos:

| Uso | K |
|-----|---|
| Top-100 palavras frequentes | 100 |
| Top-100 palavras emergentes | 100 |
| Top-10 manchetes similares por consulta | 10 |

A raiz sempre guarda o menor dos K melhores elementos vistos, funcionando como limite de comparação. Candidatos piores são descartados em O(1) sem nenhuma operação adicional.

&nbsp;

## ⚙️ Etapas do Sistema

### 1. Leitura e Pré-processamento (`LerArquivo` + `ProcessadorTexto`)

O arquivo `input.csv` é percorrido duas vezes. A primeira passagem conta o total de linhas válidas para calcular dinamicamente o tamanho de cada janela temporal (`⌊N/5⌋`). A segunda passagem processa cada manchete:

- Localiza a vírgula separadora e extrai o título
- Aplica `limparTexto()`: verifica `isalpha()`, converte com `tolower()`, descarta tokens com ≤ 3 caracteres
- Remove duplicatas com `std::sort` + `std::unique` + `erase()` — sem alocar estruturas auxiliares
- Atualiza o dicionário e o índice invertido simultaneamente
- Transfere o vetor de tokens com `std::move()`, evitando cópias

Um buffer de 8 MB é alocado no fluxo de entrada para reduzir chamadas ao sistema operacional.

### 2. Top-100 Palavras Frequentes (`gerarTop100Frequentes`)

Percorre o dicionário e insere cada palavra na MinHeap com K = 100, usando `freqGlobal` como pontuação. Custo: **O(N log 100)**, onde N é o tamanho do vocabulário.

### 3. Top-100 Palavras Emergentes (`gerarTop100Emergentes`)

Aplica a taxa de crescimento sobre os dados de frequência por janela já armazenados no dicionário:

```
C(p) = (FJ5 - FJ1) / (FJ1 + 1)
```

O resultado é preservado em memória no vetor `top100Emergentes` e num `unordered_set` (`setEmergentes`) para cruzamento em O(1) na etapa seguinte.

### 4. Análise de Similaridade (`encontrarSimilaresPorTitulos`)

Para cada título do arquivo de consultas:

1. Aplica o mesmo pré-processamento do corpus
2. Usa o índice invertido para localizar manchetes candidatas (apenas as que compartilham ao menos uma palavra)
3. Calcula o Índice de Jaccard para cada candidato:

```
J(A, B) = |A ∩ B| / |A ∪ B|
```

4. Mantém as 10 manchetes com Jaccard mais alto (abaixo do threshold de 0.70) numa MinHeap com K = 10
5. Para cada resultado, verifica quais palavras pertencem ao `setEmergentes` e as apresenta como keywords

Os resultados são gravados em `output.txt`.

&nbsp;

## 📊 Análise de Complexidade

| Etapa | Complexidade | Justificativa |
|-------|:------------:|---------------|
| Leitura e indexação | O(N · M log M) | N manchetes, M tokens por manchete (sort para dedup) |
| Top-100 frequentes | O(V log 100) | V palavras únicas no vocabulário |
| Top-100 emergentes | O(V log 100) | Mesma iteração sobre o dicionário |
| Similaridade (por consulta) | O(C log 10) | C candidatos encontrados via índice invertido |

&nbsp;

## 📥 Formato de Entrada e Saída

### Entrada — `dados/input.csv`

```
publish_date,headline_text
20030219,aba decides against community broadcasting licence
20030219,act fire witnesses must be aware of defamation
...
```

### Entrada — `dados/titulos.txt`

```
Australia election results
climate change policy
...
```

Um título por linha. O sistema aplica o mesmo pré-processamento do corpus antes de realizar a busca.

### Saída — `output.txt`

```
[ANALISE DE SIMILARIDADE POR ARQUIVO DE TITULOS]
======================================================================

Titulo 1: Australia election results
Tokens : [ australia election results ]
----------------------------------------------------------------------
RANK    JACCARD   ID      MANCHETE
----------------------------------------------------------------------
1       0.6700    304521  australia federal election results
        Keywords emergentes: election australia
2       0.5800    112043  results australian election campaign
        Keywords emergentes: (nenhuma)
...
======================================================================
```

&nbsp;

## 🔧 Compilação e Execução

> ⚠️ **Requisito:** ambiente Linux. Necessário ter `g++` e `make` instalados.

| Comando | Função |
|---------|--------|
| `make clean` | Remove os arquivos da compilação anterior |
| `make` | Compila o projeto com otimizações (`-O3 -march=native`) |
| `make run` | Executa o programa com os arquivos padrão em `dados/` |

Para executar com arquivos personalizados:

```bash
./build/app caminho/para/input.csv caminho/para/titulos.txt
```

&nbsp;

## 👥 Autores

Projeto desenvolvido por:

**Bernardo Henrique Lebron Machado** — Engenharia de Computação / CEFET-MG  
**Mariana Kaori Yano** — Engenharia de Computação / CEFET-MG  
**Pedro Henrique de Araújo Carvalho** — Engenharia de Computação / CEFET-MG
