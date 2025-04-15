from random import randint
import numpy as np
import random
import networkx as nx

m = 1000

def generate_topology():
    # Create an empty graph
    G = nx.Graph()

    # Add 20 nodes
    G.add_nodes_from(range(1, 21))  # Nodes labeled 1 through 20

    # Add fewer than 30 edges (here: 28 edges)
    edges = [
        (1, 2), (2, 3), (3, 4), (4, 5),
        (5, 6), (6, 7), (7, 8), (8, 9),
        (9, 10), (10, 1),  # Cycle of 10 nodes
        (1, 11), (2, 12), (3, 13), (4, 14), (5, 15),
        (6, 16), (7, 17), (8, 18), (9, 19), (10, 20),  # Spokes to outer nodes
        (11, 12), (12, 13), (13, 14), (14, 15),
        (15, 16), (16, 17), (17, 18), (18, 19)
        # 10-cycle outer with one missing edge to stay under 30
    ]

    G.add_edges_from(edges)
    return G

def get_N():
    # Generate the matrix of packet intensities N where N[i][j] is the number of packets from node i to node j.
    N = np.array([
        [1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6],
        [2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7],
        [3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1],
        [4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2],
        [5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3],
        [6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4],
        [7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5],
        [1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6],
        [2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7],
        [3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1],
        [4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2],
        [5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3],
        [6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4],
        [7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5],
        [1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6],
        [2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7],
        [3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1],
        [4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2],
        [5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3],
        [6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4]
    ])

    for i in range(20):
        N[i][i] = 0

    return N

def calculate_a(G, N):
    a = {e: 0.0 for e in G.edges}

    for i in range(len(N)):
        for j in range(len(N)):
            if i != j and N[i][j] > 0:
                try:
                    # Przyjmujemy najkrótszą ścieżkę jako trasę transmisji
                    path = nx.shortest_path(G, source=i, target=j)
                    # Dzielenie ruchu na poszczególne krawędzie ścieżki
                    path_edges = list(zip(path[:-1], path[1:]))
                    for e in path_edges:
                        # Ujednolicamy reprezentację krawędzi (bez względu na kolejność węzłów)
                        e_norm = tuple(sorted(e))
                        if e_norm in a:
                            a[e_norm] += N[i][j]
                except nx.NetworkXNoPath:
                    # Jeśli nie ma ścieżki, pomijamy tę parę
                    continue
    return a

def calculate_c(G):
    return {e: randint(900, 1100) * m for e in G.edges}

def compute_network_delay(G, N, c, a):
    # Calculate total packet intensity
    G_sum = np.sum(N)
    # Compute delay sum using the formula (ensure division is proper)
    delay_sum = sum(a.get(e, 0) / (c.get(e, 1) / m - a.get(e, 0)) for e in G.edges)
    T = (1 / G_sum) * delay_sum
    return T

def estimate_reliability(G, N, c, a, p, T_max, num_samples=1000):
    for e in G.edges:
        if (c.get(e) / m) < a.get(e):
            return -1

    count = 0
    for _ in range(num_samples):
        G_temp = G.copy()
        for e in list(G.edges):
            if random.random() > p:  # simulate edge failure
                G_temp.remove_edge(*e)
        if nx.is_connected(G_temp):
            T = compute_network_delay(G_temp, N, c, a)
            if T < T_max:
                count += 1
    return count / num_samples

def run_experiments():
    G = generate_topology()
    N = get_N()
    a = calculate_a(G, N)
    c = calculate_c(G)
    p, T_max = 0.95, 0.3

    print("Eksperymenty dla różnych wartości N")
    for scale in [1, 2, 3, 5]:
        print("Original network delay:", compute_network_delay(G, N, c, a))
        N_scaled = (N * scale).astype(int)
        a_scaled = calculate_a(G, N_scaled)
        print("Original network delay:", compute_network_delay(G, N, c, a))
        reliability = estimate_reliability(G, N_scaled, c, a_scaled, p, T_max)
        if reliability != -1:
            print(f'N x {scale}, Reliability: {reliability}')
        else:
            print("dupadupadupadupa")

    print("Eksperymenty dla różnych wartości c")
    for scale in [1, 5, 10, 30]:
        c_scaled = {e: int(c[e] * scale) for e in G.edges}
        reliability = estimate_reliability(G, N, c_scaled, a, p, T_max)
        if reliability != -1:
            print(f'N x {scale}, Reliability: {reliability}')
        else:
            print("dupadupadupadupa")

    print("Eksperymenty dla różnych topologii")
    for extra_edges in [1, 5, 10, 30]:
        new_edges = []
        for _ in range(extra_edges):
            u, v = random.randint(0, 19), random.randint(0, 19)
            if u != v:
                new_edges.append((u, v))
        G_extended = G.copy()
        G_extended.add_edges_from(new_edges)
        # Adjust or initialize capacities and flows for extra edges
        mean_capacity = int(np.mean(list(c.values())))
        c_extended = {e: c.get(e, mean_capacity) for e in G_extended.edges}
        # For new edges that do not have an assigned flow, we can set them randomly
        a_extended = {tuple(sorted(e)): a.get(e, random.randint(1, c_extended[tuple(sorted(e))] // 2)) 
                      for e in G_extended.edges}
        reliability = estimate_reliability(G_extended, N, c_extended, a_extended, p, T_max)
        if reliability != -1:
            print(f'N x {scale}, Reliability: {reliability}')
        else:
            print("dupadupadupadupa")

if __name__ == "__main__":
    run_experiments()