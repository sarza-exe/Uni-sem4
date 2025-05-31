#include <bits/stdc++.h>
using namespace std;

// MST GENERATOR

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
    // Adjecency list
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
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<>> pq; // smallest weight is always at the top. (weight, node)

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

int n;  // number of nodes in tree
vector<vector<int>> children; // children[u] = list of u's children
vector<int> parent; // parent[u] = parent of u (or -1 for root = 0)
vector<int> order; // temporary array for sorting children
vector<long long> ttime; // ttime[u] = transfer time - min rounds to inform entire subtree rooted at u
vector<vector<int>> schedule; // schedule[u] = optimal order of children of u

// DFS to build parent/children relations for the rooted tree at root = 0
void buildTree(const vector<vector<int>>& adj, int u, int p) {
    parent[u] = p;
    for (int v : adj[u]) {
        if (v == p) continue;
        children[u].push_back(v);
        buildTree(adj, v, u);
    }
}

// Post-order DFS to compute ttime[u] and record schedule[u]
void dfsCompute(int u) {
    // Base: if u has no children, ttime[u] = 0, schedule[u] remains empty.
    if (children[u].empty()) {
        ttime[u] = 0;
        return;
    }
    // Compute for each child first
    for (int c : children[u]) {
        dfsCompute(c);
    }
    // Now we have ttime[c] for each child c.
    // Sort children of u by descending ttime[c]
    order = children[u]; 
    sort(order.begin(), order.end(), [&](int a, int b) {
        return ttime[a] > ttime[b];
    });
    // Record this sorted order as the optimal schedule for u
    schedule[u] = order;
    // Compute ttime[u] = max_{i=1..k}(i + ttime[c_i])
    long long best = 0;
    for (int i = 0; i < (int)order.size(); ++i) {
        long long finish_time = (long long)(i+1) + ttime[order[i]];
        best = max(best, finish_time);
    }
    ttime[u] = best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // INPUT AND MST CONSTRUCTION
    cout << "Enter number of nodes (n): ";
    cin >> n;
    if (n <= 0) {
        cerr << "Number of nodes must be positive.\n";
        return 1;
    }

    // Generate a random complete graph on n nodes and build its MST via Prim
    vector<Edge> complete_edges = generate_complete_graph(n);
    auto [mst_weight, mst_edges] = prim(n, complete_edges);

    // Build adjacency list of the MST (undirected)
    vector<vector<int>> adj(n);
    for (auto &e : mst_edges) {
        double w;
        int u, v;
        tie(w, u, v) = e;
        // MST edges are undirected in the tree
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // ROOT THE TREE AT NODE 0 and PREPARE STRUCTURES

    parent.assign(n, -1);
    children.assign(n, vector<int>());
    buildTree(adj, 0, -1);

    ttime.assign(n, 0LL);
    schedule.assign(n, vector<int>());

    // COMPUTE BROADCAST SCHEDULE AND MINIMUM ROUNDS

    // Run DFS from the root to compute ttime and schedule
    dfsCompute(0);

    // OUTPUT RESULTS

    // Print total MST weight (just for reference)
    cout << fixed << setprecision(5);
    cout << "\nTotal weight of generated MST: " << mst_weight << "\n\n";

    // Print the minimum number of rounds needed for entire tree:
    cout << "Minimum number of rounds to inform all nodes: " << ttime[0] << "\n\n";

    // Print, for each node u, the optimal order of informing its children.
    // Format: u: child1 child2 ... child_k
    cout << "Broadcast schedule (for each node u, the order to inform its children):\n";
    for (int u = 0; u < n; ++u) {
        if (schedule[u].empty()) {
            cout << "Node " << u << ": (no children)\n";
        } else {
            cout << "Node " << u << ":";
            for (int c : schedule[u]) {
                cout << " " << c;
            }
            cout << "\n";
        }
    }

    return 0;
}
