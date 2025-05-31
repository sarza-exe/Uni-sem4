#include <bits/stdc++.h>
using namespace std;

// Disjoint Set Union
// track a set of elements partitioned into disjoint (non-overlapping) subsets
struct DSU {
    vector<int> parent; //The root of a set is the element whose parent is itself 
    vector<int> rankv; // rankv - estimate of the height of the tree whose root is x
    DSU(int n): parent(n), rankv(n,0) {
        iota(parent.begin(), parent.end(), 0); // it's node is its own parent and therefor tree
    }
    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]); // it changes depth into breadth
    }
    bool unite(int a, int b) { // unite attaches smaller tree to bigger
        a = find(a); b = find(b);
        if (a == b) return false;
        if (rankv[a] < rankv[b]) swap(a,b);
        parent[b] = a;
        if (rankv[a] == rankv[b]) rankv[a]++;
        return true;
    }
};


using Edge = tuple<double,int,int>;  // (waga, u, v)

// Generate complete graph with weights in (0,1)
vector<Edge> generate_complete_graph(int n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);
    vector<Edge> edges;
    edges.reserve(n*(n-1)/2);
    for (int u = 0; u < n; ++u) {
        for (int v = u+1; v < n; ++v) {
            double w = dist(gen);
            edges.emplace_back(w, u, v);
        }
    }
    return edges;
}


// Kruskal algorithm
pair<double, vector<Edge>> kruskal(int n, vector<Edge> edges) {
    sort(edges.begin(), edges.end(),
         [](auto &a, auto &b){ return get<0>(a) < get<0>(b); }); // O(E log E) sort edges by increasing weight
    DSU dsu(n);
    double total_weight = 0;
    vector<Edge> mst;
    mst.reserve(n-1);
    for (auto &e : edges) {
        double w; 
        int u, v;
        tie(w,u,v) = e;
        if (dsu.unite(u, v)) { //  O(1) if we united u and v trees
            mst.push_back(e); // Add the smallest edge that doesn’t create a cycle.
            total_weight += w;
            if ((int)mst.size() == n-1) break;
        }
    }
    return { total_weight, mst };
}


// Prim algorithm
pair<double, vector<Edge>> prim(int n, const vector<Edge> &edges) {
    // Budujemy listę sąsiedztwa
    vector<vector<pair<int,double>>> adj(n); // adj(u) = vector of pairs such as (index of neighbor v, weight(u,v))
    for (auto &e : edges) {
        double w; int u, v;
        tie(w,u,v) = e; // unpack tuple into variables w u v
        adj[u].emplace_back(v,w);
        adj[v].emplace_back(u,w);
    }
    vector<bool> used(n,false); // nodes already included in MST
    vector<double> min_e(n, 1e18); // min_e[v] is the minimum weight of any edge connecting node v to the MST so far
    vector<int> sel_e(n, -1); // sel_e[v] = u means: the best edge found so far to connect node v to the MST comes from node u
    priority_queue<pair<double,int>,
                   vector<pair<double,int>>,
                   greater<>> pq; // smallest weight is always at the top. (weight, node)

    min_e[0] = 0; // starting node in MST
    pq.emplace(0.0, 0);

    double total_weight = 0; // of MSt so far
    vector<Edge> mst;
    mst.reserve(n-1);

    while (!pq.empty()) {
        auto [w,u] = pq.top(); pq.pop();
        if (used[u]) continue; // node already in MST
        used[u] = true;
        total_weight += w;
        if (sel_e[u] != -1) { // skip at first iteration (start node is paretnelss)
            int v = sel_e[u];
            mst.emplace_back(w, u, v);
        }
        for (auto &[v, w2] : adj[u]) { // for each neighbor of node u
            if (!used[v] && w2 < min_e[v]) {
                min_e[v] = w2;
                sel_e[v] = u;
                pq.emplace(w2, v);
            }
        }
    }
    return { total_weight, mst };
}

// Helper to print MST
void print_mst(const vector<Edge> &mst) {
    for (auto &e : mst) {
        double w; int u, v;
        tie(w,u,v) = e;
        cout << "(" << u << " - " << v << ") waga: " << w << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false); // doesn't sync cout/cin with stdio equivalents. Faster
    cin.tie(nullptr); // cin doesn't flush cout

    const int n = 5;

    auto edges = generate_complete_graph(n);
    cout << "\nGraf pełny wygenerowany, liczba krawędzi: " 
         << edges.size() << "\n";

    // Kruskal
    auto [w_kruskal, mst_kruskal] = kruskal(n, edges);
    cout << "\n=== MST (Kruskal) ===\n";
    cout << "Waga całkowita: " << w_kruskal << "\n";
    print_mst(mst_kruskal);

    // Prim
    auto [w_prim, mst_prim] = prim(n, edges);
    cout << "\n=== MST (Prim) ===\n";
    cout << "Waga całkowita: " << w_prim << "\n";
    print_mst(mst_prim);

    return 0;
}
