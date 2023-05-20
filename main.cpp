#include <iostream>
#include <string.h>
#include <vector>
#include <random>
#include <bitset>
#include <chrono>
#include <thread>
#include <algorithm>
#include <mutex>
#include <omp.h>

std::mutex mutex;

void map_bin_to_hex(char *c) {
    char hexMap[] =
            {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int index = int(c[0]) * 8 + int(c[1]) * 4 + int(c[2]) * 2 + int(c[3]) * 1;
    //std::cout << "bit: " << int(c[0]) << int(c[1]) << int(c[2]) << int(c[3]) << std::endl;
    std::cout << hexMap[index];
}

void toHex(std::vector<int> S, int &L) {
    int i, j, k;
    for (i = -2; i < L; i += 4) {
        char hexB[4];
        for (k = 0, j = i; j < (i + 4); j++, k++) {
            if (j < 0) {
                hexB[k] = 0;
            } else {
                if (S[j] == -1) {
                    hexB[k] = 0;
                } else {
                    hexB[k] = 1;
                }
            }
            //printf("h[%d] = %d\n", k, hexB[k]);
        }
        map_bin_to_hex(hexB);
    }
}

void generate(std::vector<int> &S, int &L, std::mt19937 &mt) {
    std::uniform_real_distribution<double> dist(0, 2);
    for (int i = 0; i < L; i++) {
        S[i] = dist(mt);
    }
    std::replace(S.begin(), S.end(), 0, -1);
}

void calculateC(std::vector<int> S, int &L, int &k, std::vector<int> &C) {
    for (int i = 1; i < k; i++) {
        int sum = 0;
        for (int j = 0; j < L - i; j++) {
            sum += S[j] * S[j + i];
        }
        C[i] = sum;
        //std::cout << "i: " << i << " C[i]: " << C[i] << std::endl;
    }
}

void calculatePSL(std::vector<int> S, std::vector<int> &bestS, std::vector<int> C, int PSL, int &bestPSL,
                  int L, int k, std::mt19937 &mt) {
    generate(S, L, mt);
    calculateC(S, L, k, C);
    for (int i = 1; i < L; i++) {
        if (PSL < abs(C[i])) {
            PSL = abs(C[i]);
        }
        //std::cout << "curr: " << PSL << " I: " << i << " abs(C[i]): " << abs(C[i]) << std::endl;
    }
    std::lock_guard<std::mutex> guard(mutex);
    if (PSL < bestPSL) {
        bestPSL = PSL;
        bestS = S;
        std::cout << "bestPSL: " << bestPSL << std::endl;
    }
}

void calculateMF(std::vector<int> S, std::vector<int> &bestS, std::vector<int> C, double MF, double &bestMF,
                 int L, int k, std::mt19937 &mt) {
    generate(S, L, mt);
    calculateC(S, L, k, C);
    int sum = 0;
    for (int i = 1; i < k; i++) {
        sum += C[i] * C[i];
        //std::cout << "i= " << i << "C[i]: " << C[i] << " C[i]^2: " << C[i] * C[i] << " Sum: " << sum << std::endl;
    }
    MF = double((L * L)) / (2 * sum);
    std::lock_guard<std::mutex> guard(mutex);
    if (MF > bestMF) {
        bestMF = MF;
        bestS = S;
        std::cout << "bestMF: " << bestMF << std::endl;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
int main(int argc, char **argv) {
    int L, nfesLmt, seed, bestPSL = 99, PSL = 0, numOfThreads = 1;
    double bestMF = 0, MF = 0, speed, runtime;
    std::string type;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "L") == 0) {
            L = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-type") == 0) {
            type = argv[i + 1];
        }
        if (strcmp(argv[i], "-seed") == 0) {
            seed = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-nfesLmt") == 0) {
            nfesLmt = atoi(argv[i + 1]);
        }
    }

    std::cout << "Imput number of threads: ";
    std::cin >> numOfThreads;

    int k = L - 1;
    std::vector<int> S(L);
    std::vector<int> bestS(L);
    std::vector<int> C(k);
    std::mt19937 mt(seed);

    L = 250;
    type = "PSL";
    seed = 9443832;
    nfesLmt = 100000;
    int count = 0;
    auto start = std::chrono::high_resolution_clock::now();

    omp_set_num_threads(numOfThreads);
    #pragma omp parallel for
    for (int i = 0; i < nfesLmt; i++) {
        if (type == "PSL") {
            calculatePSL(S, bestS, C, PSL, bestPSL, L, k, mt);
        }
        if (type == "MF") {
            calculateMF(S, bestS, C, MF, bestMF, L, k, mt);
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();

    runtime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    speed = nfesLmt / (runtime / 1000);

    std::cout << "\nL: " << L << "\nnfesLmt: " << nfesLmt << "\nseed: "
              << seed << "\nsequence: 0x";
    toHex(bestS, L
    );
    std::cout << "\nMF: ";
    if (bestMF == 0) {
        std::cout << "Not calculated.";
    } else {
        std::cout <<
                  bestMF;
    }
    std::cout << "\nPSL: ";
    if (bestPSL == 99) {
        std::cout << "Not calculated.";
    } else {
        std::cout <<
                  bestPSL;
    }
    std::cout << "\nruntime: " << runtime
              << "\nspeed: " << speed << " evaluations/s"
              << "\nnumber of threads: " <<
              numOfThreads;
    return 0;
}
#pragma clang diagnostic pop