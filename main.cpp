#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>

bool can_package_all_beers(const std::vector<int> &p, int k) {
    for (int bottles: p) {
        if (bottles % k == 0) {
            continue;
        }
        int boxes = bottles / k + 1;
        if (bottles % k < k - boxes) {
            return false;
        }
    }
    return true;
}

int calculate_largest_k_forced(const std::vector<int> &p) {
    int k = *std::min_element(p.begin(), p.end()) + 1;
    while (true) {
        if (can_package_all_beers(p, k)) {
            return k;
        } else {
            k--;
        }
    }
}

int calculate_largest_k(const std::vector<int> &p) {
    int k = *std::min_element(p.begin(), p.end());
    std::vector<int> check;
    for (int i = 1; i <= k/k+1; i++) {
        if (k % i == 0) {
            check.push_back(i);
            check.push_back(i + 1);
            if (i != k / i) {
                check.push_back(k / i);
                check.push_back((k / i) + 1);
            }
        }
    }
    std::sort(check.begin(), check.end(), std::greater<>());
    for (auto j: check) {
        if (can_package_all_beers(p, j)) {
            return j;
        }
    }
    return 2;
}

int main() {
    int N;
    N = 4; // število različnih vrst piva
    std::vector<int> p; //vektor vseh piv
    p.reserve(N);
    p.push_back(8);
    p.push_back(20);
    p.push_back(9);
    p.push_back(10);

    int k = calculate_largest_k(p);
    std::cout << "Piva: "; 
    for(int ai : p){
	std::cout << ai << ", ";
    }
    
    std::cout << std::endl << "Najvecja velikost skatel, v katere jih lahko zapakiramo: " << k << ".";

    return 0;
}