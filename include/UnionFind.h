#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>
#include <numeric>

class UnionFind {
    std::vector<int> pai;
    std::vector<int> rank;

public:
    UnionFind(int n) {
        pai.resize(n);
        std::iota(pai.begin(), pai.end(), 0); // Preenche com 0, 1, 2...
        rank.assign(n, 0);
    }

    int find(int i) {
        if (pai[i] == i)
            return i;
        return pai[i] = find(pai[i]); // Path compression
    }

    void unite(int i, int j) {
        int raiz_i = find(i);
        int raiz_j = find(j);

        if (raiz_i != raiz_j) {
            if (rank[raiz_i] < rank[raiz_j])
                pai[raiz_i] = raiz_j;
            else {
                pai[raiz_j] = raiz_i;
                if (rank[raiz_i] == rank[raiz_j])
                    rank[raiz_i]++;
            }
        }
    }
};

#endif