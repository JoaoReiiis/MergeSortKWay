# Multiway Merge Sort em C++

Este repositório contém uma implementação de um algoritmo de Merge Sort Multiway externo em C++, que realiza a ordenação de registros de um arquivo CSV com base em um campo específico (`Period`). O algoritmo divide o arquivo de entrada em k partições, gera "runs" ordenados e intercala essas partições de forma eficiente, utilizando arquivos temporários.

## Funcionalidades

* Leitura de um arquivo CSV (`dados.csv`).
* Divisão em k arquivos binários temporários.
* Geração de runs ordenados de tamanho crescente (multiplicativo por k).
* Intercalação multiway dos runs até a obtenção de um único arquivo ordenado.
* Conversão do resultado final para CSV (`ordenado.csv`).

## Pré-requisitos

* Compilador C++ compatível com C++17 (ex.: GCC 9+ ou Clang).
* Sistema operacional compatível (Linux, macOS ou Windows).

## Instruções de Uso

1. **Preparar os dados**:

   * Descompacte o arquivo `dados.zip` na raiz do projeto. Isso irá gerar o arquivo `dados.csv`.

2. **Criar diretório temporário**:

   * Execute o comando abaixo para criar a pasta onde os arquivos intermédios serão gerados:

     ```bash
     mkdir temp
     ```

3. **Compilar o código**:

   * Utilize o seguinte comando para compilar o programa:

     ```bash
     g++ -std=c++17 merge_multiway.cpp -o merge_multiway
     ```

4. **Executar o algoritmo**:

   * Após a compilação, execute:

     ```bash
     ./merge_multiway
     ```
   * O programa irá gerar dois arquivos na raiz do projeto:

     * `ordenado.bin`: versão binária dos registros ordenados.
     * `ordenado.csv`: versão CSV com o mesmo cabeçalho de `dados.csv`, contendo os registros ordenados.

## Estrutura de Arquivos

```
├── dados.zip         # Arquivo compactado contendo dados.csv
├── dados.csv         # Arquivo CSV de entrada (após descompactar)
├── merge_multiway.cpp# Código-fonte em C++
├── ordenado.bin      # Saída binária ordenada
├── ordenado.csv      # Saída CSV ordenada
└── temp/             # Diretório de arquivos temporários (deve ser criado)
```

## Explicação do Algoritmo

1. **Leitura do CSV**: o arquivo `dados.csv` é lido linha a linha, pulando o cabeçalho.
2. **Particionamento Inicial**: cada registro é distribuído em k arquivos binários (`temp/F0.bin`, ..., `temp/F{k-1}.bin`) de forma round-robin.
3. **Geração e Intercalação de Runs**:

   * Define-se um tamanho de run inicial igual a 1.
   * Em cada rodada, cada partição gera runs de tamanho `runSize` e intercala esses runs entre k arquivos de saída (`S0.bin` ...).
   * Ao final da rodada, o tamanho de run é multiplicado por k e os papéis de entrada/saída são invertidos.
4. **Consolidação Final**: após as rodadas necessárias (enquanto `runSize < totalRegistros`), o primeiro arquivo binário contém todos os registros ordenados.
5. **Conversão para CSV**: o arquivo binário ordenado é lido e convertido de volta para o formato CSV, incluindo o cabeçalho original.

## Personalização

* O valor de `k` (número de caminhos) está definido como `16` no `main()`. Você pode alterá-lo conforme necessidade de memória e desempenho.

---

