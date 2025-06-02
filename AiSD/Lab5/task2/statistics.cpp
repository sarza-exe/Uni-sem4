#include <bits/stdc++.h>
using namespace std;

// Edge representation: (weight, u, v)
using Edge = tuple<double,int,int>;

// Generate complete graph with random weights in (0,1)
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

// Prim algorithm
pair<double, vector<Edge>> prim(int n, const vector<Edge> &edges) {
    // Build adjacency list (u → [(v, weight)])
    vector<vector<pair<int,double>>> adj(n);
    for (auto &e : edges) {
        double w; int u, v;
        tie(w,u,v) = e;
        adj[u].emplace_back(v,w);
        adj[v].emplace_back(u,w);
    }
    vector<bool> used(n,false);
    vector<double> min_e(n, 1e18);
    vector<int> sel_e(n, -1);
    priority_queue<pair<double,int>,
                   vector<pair<double,int>>,
                   greater<>> pq;

    // Start from node 0 (arbitrary choice for MST root)
    min_e[0] = 0.0;
    pq.emplace(0.0, 0);

    double total_weight = 0.0;
    vector<Edge> mst;
    mst.reserve(n-1);

    while (!pq.empty()) {
        auto [w,u] = pq.top();
        pq.pop();
        if (used[u]) continue;
        used[u] = true;
        total_weight += w;
        if (sel_e[u] != -1) {
            int parent = sel_e[u];
            mst.emplace_back(w, u, parent);
        }
        for (auto &nei : adj[u]) {
            int v = nei.first;
            double w2 = nei.second;
            if (!used[v] && w2 < min_e[v]) {
                min_e[v] = w2;
                sel_e[v] = u;
                pq.emplace(w2, v);
            }
        }
    }
    return { total_weight, mst };
}


// Helper functions that, given a tree adjacency list and a chosen root,
// compute the minimum number of rounds needed to broadcast from that root to all nodes.
// Build the rooted tree: fill parent[] and children[] vectors.
void buildTree(const vector<vector<int>> &adj, int u, int p, vector<int> &parent, vector<vector<int>> &children) {
    parent[u] = p;
    for (int v : adj[u]) {
        if (v == p) continue;
        children[u].push_back(v);
        buildTree(adj, v, u, parent, children);
    }
}

// Post-order DFS to compute ttime[u] and schedule[u]:
//  ttime[u] = minimum rounds to broadcast to entire subtree rooted at u (u informed at time 0)
//  schedule[u] = optimal order of informing u's children (not needed in experiments, but we compute anyway)
void dfsCompute(int u, vector<vector<int>> &children, vector<long long> &ttime, vector<vector<int>> &schedule) {
    if (children[u].empty()) {
        ttime[u] = 0;
        return;
    }
    // First compute for all children
    for (int c : children[u]) {
        dfsCompute(c, children, ttime, schedule);
    }
    // Sort children by descending ttime[c]
    vector<int> order = children[u];
    sort(order.begin(), order.end(), [&](int a, int b) {
        return ttime[a] > ttime[b];
    });
    schedule[u] = order;
    // Compute ttime[u] = max_{i=1..k} (i + ttime[order[i-1]])
    long long best = 0;
    for (int i = 0; i < (int)order.size(); ++i) {
        long long finish_time = (long long)(i + 1) + ttime[ order[i] ];
        best = max(best, finish_time);
    }
    ttime[u] = best;
}

// Given a tree adjacency list 'adj' of size n, and a chosen 'root',
// returns the minimum number of rounds to broadcast from 'root' to all nodes.
long long computeBroadcastTime(int n, const vector<vector<int>> &adj, int root) {
    vector<int> parent(n, -1);
    vector<vector<int>> children(n);
    vector<long long> ttime(n, 0LL);
    vector<vector<int>> schedule(n);

    // Build the rooted tree at 'root'
    buildTree(adj, root, -1, parent, children);
    // Compute ttime[] via post-order DFS
    dfsCompute(root, children, ttime, schedule);
    return ttime[root];
}


int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 5) {
        cerr << "Usage ./program [nMin nMax step rep]" << endl;
        return -1;
    }

    int nMin = atoi(argv[1]);
    int nMax = atoi(argv[2]);
    int step = atoi(argv[3]);
    int rep = atoi(argv[4]);

    cout << "nMin = " << nMin << " nMax = " << nMax << " step = " << step << " rep = " << rep << "\n";

    // Prepare random generator for choosing root
    random_device rd;
    mt19937 gen(rd());

    // Open CSV file for output
    ofstream results("results.csv");
    if (!results.is_open()) {
        return -1;
    }
    // CSV header
    results << "n,avg_rounds,min_rounds,max_rounds\n";

    // Main experiment loops
    for (int n = nMin; n <= nMax; n += step) {
        long long sumRounds = 0;
        long long minRounds = LLONG_MAX;
        long long maxRounds = 0;

        // Prepare distribution for selecting random root
        uniform_int_distribution<int> rootDist(0, n - 1);

        for (int r = 0; r < rep; ++r) {
            // Generate a random complete graph on n nodes
            vector<Edge> complete_edges = generate_complete_graph(n);

            // Build MST via Prim
            auto [mst_weight, mst_edges] = prim(n, complete_edges);

            // Build adjacency list of MST (undirected)
            vector<vector<int>> adj(n);
            for (auto &e : mst_edges) {
                double w; int u, v;
                tie(w, u, v) = e;
                adj[u].push_back(v);
                adj[v].push_back(u);
            }

            //  Pick a random start vertex (root) in [0, n-1]
            int root = rootDist(gen);

            // Compute broadcast time from that root
            long long rounds = computeBroadcastTime(n, adj, root);

            // Accumulate statistics
            sumRounds += rounds;
            minRounds = min(minRounds, rounds);
            maxRounds = max(maxRounds, rounds);
        }

        double avgRounds = static_cast<double>(sumRounds) / rep;
        // Write results for this n
        results << n << ","
                << fixed << setprecision(3) << avgRounds << ","
                << minRounds << ","
                << maxRounds << "\n";

        // Optional: print progress to console
        cout << "n = " << n
             << " -> avg: " << fixed << setprecision(3) << avgRounds
             << ", min: " << minRounds
             << ", max: " << maxRounds << "\n";
    }

    results.close();
    cout << "\nSimulation complete\n";
    return 0;
}