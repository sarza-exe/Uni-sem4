#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <windows.h>

using namespace std;
using Edge = tuple<double,int,int>;  // (weight, u, v)

// DSU (Disjoint Set Union) for Kruskal
struct DSU {
    vector<int> parent, rankv;
    DSU(int n): parent(n), rankv(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rankv[a] < rankv[b]) swap(a, b);
        parent[b] = a;
        if (rankv[a] == rankv[b]) rankv[a]++;
        return true;
    }
};

// Generate a complete graph with random weights in (0,1)
static vector<Edge> generate_complete_graph(int n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);

    vector<Edge> edges;
    edges.reserve(n * (n - 1) / 2);
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            double w = dist(gen);
            edges.emplace_back(w, u, v);
        }
    }
    return edges;
}

// Kruskal's algorithm (returns total MST weight)
static double run_kruskal(int n, vector<Edge> edges) {
    sort(edges.begin(), edges.end(), [](auto &a, auto &b) {
        return get<0>(a) < get<0>(b);
    });
    DSU dsu(n);
    double total_weight = 0;
    int edges_used = 0;

    for (auto &e : edges) {
        double w;
        int u, v;
        tie(w, u, v) = e;
        if (dsu.unite(u, v)) {
            total_weight += w;
            edges_used++;
            if (edges_used == n - 1) break;
        }
    }
    return total_weight;
}

// Prim's algorithm (returns total MST weight)
static double run_prim(int n, const vector<Edge> &edges) {
    // Build adjacency list
    vector<vector<pair<int,double>>> adj(n);
    adj.assign(n, {});
    for (auto &e : edges) {
        double w;
        int u, v;
        tie(w, u, v) = e;
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
    }

    vector<bool> used(n, false);
    vector<double> min_edge(n, numeric_limits<double>::infinity());
    min_edge[0] = 0.0;
    priority_queue<pair<double,int>,
                   vector<pair<double,int>>,
                   greater<>> pq;
    pq.emplace(0.0, 0);

    double total_weight = 0;
    int taken = 0;

    while (!pq.empty() && taken < n) {
        auto [w, u] = pq.top();
        pq.pop();
        if (used[u]) continue;
        used[u] = true;
        total_weight += w;
        taken++;

        for (auto &p : adj[u]) {
            int v = p.first;
            double w2 = p.second;
            if (!used[v] && w2 < min_edge[v]) {
                min_edge[v] = w2;
                pq.emplace(w2, v);
            }
        }
    }
    return total_weight;
}

// Measure average running time of Kruskal over `rep` repetitions
static double average_kruskal_time(int n, int rep) {
    using namespace std::chrono;
    double sum_time = 0.0;

    for (int i = 0; i < rep; ++i) {
        // Generate a fresh complete graph
        auto edges = generate_complete_graph(n);

        auto t_start = high_resolution_clock::now();
        run_kruskal(n, edges);
        auto t_end = high_resolution_clock::now();

        duration<double> diff = t_end - t_start;
        sum_time += diff.count();
    }
    return sum_time / rep;
}

// Measure average running time of Prim over `rep` repetitions
static double average_prim_time(int n, int rep) {
    using namespace std::chrono;
    double sum_time = 0.0;

    for (int i = 0; i < rep; ++i) {
        // Generate a fresh complete graph
        auto edges = generate_complete_graph(n);

        auto t_start = high_resolution_clock::now();
        run_prim(n, edges);
        auto t_end = high_resolution_clock::now();

        duration<double> diff = t_end - t_start;
        sum_time += diff.count();
    }
    return sum_time / rep;
}

// Main: compute average times for n ∈ [nMin, nMax] step `step`, repeat `rep` times
int main(int argc, const char * argv[]) {
     SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
    ios::sync_with_stdio(false);

    if (argc != 5) {
        cerr << "Usage ./program [nMin nMax step rep]" << argc-1 << endl;
        return -1;
    }

    int nMin = atoi(argv[1]);
    int nMax = atoi(argv[2]);
    int step = atoi(argv[3]);
    int rep = atoi(argv[4]);

    cout << "nMin = " << nMin << " nMax = " << nMax << " step = " << step << " rep = " << rep << "\n";

    ofstream csvFile("stats.csv");
    if (!csvFile.is_open()) {
        cerr << "Couldn't open file.\n";
        return 1;
    }

    // Write CSV header
    csvFile << "n,avg_time_kruskal,avg_time_prim\n";

    // Loop over graph sizes
    for (int n = nMin; n <= nMax; n += step) {
        double avgK = 0.0, avgP = 0.0;

        // Launch two threads: one for Kruskal, one for Prim
        thread tK([&]() {
            avgK = average_kruskal_time(n, rep);
        });
        thread tP([&]() {
            avgP = average_prim_time(n, rep);
        });

        // Wait for both to finish
        tK.join();
        tP.join();

        // Write results to CSV
        csvFile << n << "," 
                << fixed << setprecision(6) << avgK << "," 
                << fixed << setprecision(6) << avgP << "\n";

        // Optional: print progress to console
        cout << "n = " << n 
             << " : Kruskal avg: " << avgK 
             << " s, Prim avg: " << avgP << " s\n";
    }

    csvFile.close();
    cout << "Results saved to stats.csv\n";
    SetThreadExecutionState(ES_CONTINUOUS);
    return 0;
}
