#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

// Ordena pelo campo Period //
string getPeriod(const string &line) {
  int pos1 = line.find(',');
  if (pos1 == -1)
    return "";
  int pos2 = line.find(',', pos1 + 1);
  if (pos2 == -1)
    return "";
  return line.substr(pos1 + 1, pos2 - pos1 - 1);
}

int main() {
  const int k = 16;
  const string INPUT_FILE_PREFIX = "temp/F";
  const string OUTPUT_FILE_PREFIX = "temp/S";

  ifstream entradaCSV("dados.csv", ios::in | ios::binary);
  string cabecalho;
  getline(entradaCSV, cabecalho);

  for (int i = 0; i < k; i++) {
    ofstream file(INPUT_FILE_PREFIX + to_string(i) + ".bin",
                  ios::out | ios::binary | ios::trunc);
    file.close();
  }

  int totalRegistros = 0;
  string linha;
  while (getline(entradaCSV, linha)) {
    int len = int(linha.size());
    int idx = totalRegistros % k;
    ofstream fout(INPUT_FILE_PREFIX + to_string(idx) + ".bin",
                  ios::out | ios::binary | ios::app);
    fout.write(reinterpret_cast<const char *>(&len), sizeof(int));
    fout.write(linha.data(), len);
    fout.close();
    totalRegistros++;
  }
  entradaCSV.close();

  int blocoTamanho = 1;
  bool flip = true;
  auto inicio = chrono::high_resolution_clock::now();

  while (blocoTamanho < totalRegistros) {
    string prefixIn = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
    string prefixOut = (flip ? OUTPUT_FILE_PREFIX : INPUT_FILE_PREFIX);

    for (int i = 0; i < k; i++) {
      ofstream of(prefixOut + to_string(i) + ".bin",
                  ios::out | ios::binary | ios::trunc);
      of.close();
    }

    ifstream inputs[k];
    for (int i = 0; i < k; i++)
      inputs[i].open(prefixIn + to_string(i) + ".bin", ios::in | ios::binary);

    string registro[k];
    int lidos[k];
    bool presente[k];

    int runIndex = 0;
    bool aindaTemRuns = true;

    while (aindaTemRuns) {
      int somaPresentes = 0;
      for (int i = 0; i < k; i++) {
        lidos[i] = 0;
        presente[i] = false;
        int len;
        if (inputs[i].read(reinterpret_cast<char *>(&len), sizeof(int))) {
          registro[i].resize(len);
          inputs[i].read(&registro[i][0], len);
          lidos[i] = 1;
          presente[i] = true;
          somaPresentes++;
        }
      }
      if (somaPresentes == 0) {
        aindaTemRuns = false;
        break;
      }

      int countRunsNaVez = somaPresentes;
      while (countRunsNaVez > 0) {
        int menorIdx = -1;
        string menorPeriod;
        for (int i = 0; i < k; i++) {
          if (presente[i]) {
            string per = getPeriod(registro[i]);
            if (menorIdx < 0 || per < menorPeriod) {
              menorIdx = i;
              menorPeriod = per;
            }
          }
        }
        int arqDestino = runIndex % k;
        int lenOut = int(registro[menorIdx].size());

        ofstream output(prefixOut + to_string(arqDestino) + ".bin",
                        ios::out | ios::binary | ios::app);
        output.write(reinterpret_cast<const char *>(&lenOut), sizeof(int));
        output.write(registro[menorIdx].data(), lenOut);
        output.close();

        if (lidos[menorIdx] < blocoTamanho) {
          int lenNext;
          if (inputs[menorIdx].read(reinterpret_cast<char *>(&lenNext),
                                    sizeof(int))) {
            registro[menorIdx].resize(lenNext);
            inputs[menorIdx].read(&registro[menorIdx][0], lenNext);
            lidos[menorIdx]++;
          } else {
            presente[menorIdx] = false;
            countRunsNaVez--;
          }
        } else {
          presente[menorIdx] = false;
          countRunsNaVez--;
        }
      }
      runIndex++;
    }

    for (int i = 0; i < k; i++)
      inputs[i].close();

    blocoTamanho *= k;
    flip = !flip;
  }

  string finalPrefix = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
  string sortedBin = "ordenado.bin";
  string sortedCSV = "ordenado.csv";

  ifstream inputFinal(finalPrefix + "0.bin", ios::in | ios::binary);
  ofstream outputFinal(sortedBin, ios::out | ios::binary | ios::trunc);
  char buffer[4096];
  while (inputFinal.good()) {
    inputFinal.read(buffer, sizeof(buffer));
    streamsize l = inputFinal.gcount();
    outputFinal.write(buffer, l);
    outputFinal.flush();
  }

  {
    ifstream inputBin(sortedBin, ios::in | ios::binary);
    ofstream outputCSV(sortedCSV, ios::out | ios::trunc);
    outputCSV << cabecalho << '\n';
    int len;
    while (inputBin.read(reinterpret_cast<char *>(&len), sizeof(int))) {
      char *linha = new char[len];
      inputBin.read(linha, len);
      outputCSV.write(linha, len);
      outputCSV << '\n';
      delete[] linha;
      outputCSV.flush();
    }
  }

  cout << "Ordenação concluída. Total de registros: " << totalRegistros << "\n";
  auto fim = chrono::high_resolution_clock::now();
  auto duracao =
      chrono::duration_cast<chrono::milliseconds>(fim - inicio).count();
  cout << "Tempo total de execução: " << duracao << " ms\n";
  cout << "Quantidade de arquivos: " << k << "\n";

  return 0;
}
