import networkx as nx
import numpy as np
import random
import matplotlib.pyplot as plt

# Ustawienia początkowe
NUM_NODES = 20
NUM_EDGES = 25  # musi być < 30, ale musi zapewnić spójność (drzewo ma 19 krawędzi)
DENSITY_N = 0.2  # określa odsetek par węzłów, dla których będzie ruch
MIN_N_VALUE = 1
MAX_N_VALUE = 10
PACKET_SIZE = 8000  # średnia wielkość pakietu m w bitach
T_MAX = 0.5  # przykładowa wartość graniczna opóźnienia
P_EDGE = 0.95  # prawdopodobieństwo nieuszkodzenia krawędzi
NUM_TRIALS = 500  # liczba prób symulacyjnych


# Funkcja generująca spójny graf o zadanej liczbie wierzchołków i krawędzi
def generate_graph(num_nodes, num_edges):
    # Tworzymy drzewo rozpinające (spójny graf z num_nodes-1 krawędziami)
    G = nx.generators.trees.random_tree(n=num_nodes)
    G = nx.Graph(G)  # Konwersja na zwykły, niedukowany graf
    # Dodajemy dodatkowe krawędzie losowo, zachowując warunek liczby krawędzi < num_edges
    additional_edges = num_edges - (num_nodes - 1)
    potential_edges = [(u, v) for u in G.nodes() for v in G.nodes() if u < v and not G.has_edge(u, v)]
    random.shuffle(potential_edges)
    for (u, v) in potential_edges[:additional_edges]:
        G.add_edge(u, v)
    return G


# Funkcja generująca macierz natężeń N jako słownik {(i,j): n_ij}
def generate_N(num_nodes, density, min_val, max_val):
    N = {}
    for i in range(num_nodes):
        for j in range(num_nodes):
            if i != j and random.random() < density:
                N[(i, j)] = random.randint(min_val, max_val)
            else:
                N[(i, j)] = 0
    return N


# Funkcja wyznaczająca przepływy na krawędziach zgodnie z macierzą N
def compute_edge_flow(G, N):
    # Używamy klucza jako uporządkowaną krotkę (min(u,v), max(u,v))
    edge_flow = {tuple(sorted(e)): 0 for e in G.edges()}
    # Dla każdej pary (i, j) z ruchem, obliczamy najkrótszą ścieżkę w grafie G
    for (i, j), flow in N.items():
        if flow > 0:
            try:
                path = nx.shortest_path(G, source=i, target=j)
            except nx.NetworkXNoPath:
                # Jeśli nie ma ścieżki, ruch nie jest realizowany
                continue
            # Dodajemy ruch do każdej krawędzi na wyznaczonej trasie
            for k in range(len(path) - 1):
                e = tuple(sorted((path[k], path[k + 1])))
                edge_flow[e] += flow
    return edge_flow


# Funkcja przypisująca przepustowości do krawędzi - upewniamy się, że c(e) > a(e)
def assign_capacity(edge_flow, slack_min=1000, slack_max=5000):
    capacity = {}
    for e, flow in edge_flow.items():
        slack = random.randint(slack_min, slack_max)
        capacity[e] = flow + slack  # zagwarantowane c(e) > a(e)
    return capacity


# Funkcja obliczająca opóźnienie T dla danej sieci
def compute_delay(edge_flow, capacity, total_flow, packet_size):
    total_delay = 0
    for e in edge_flow:
        a_e = edge_flow[e]
        cap = capacity[e]
        # Unikamy dzielenia przez zero – zakładamy, że cap > 0 i cap/m > a(e)
        denominator = (cap / packet_size) - a_e
        if denominator <= 0:
            # W praktyce taka krawędź byłaby przeciążona – ustawiamy duże opóźnienie
            delay = 1e6
        else:
            delay = a_e / denominator
        total_delay += delay
    T = total_delay / total_flow if total_flow > 0 else 1e6
    return T


# Funkcja symulująca niezawodność sieci
def simulate_reliability(G, N, capacity, p_edge, T_max, packet_size, num_trials):
    successes = 0
    total_flow = sum(N.values())
    # Obliczamy oryginalny rozkład przepływu dla porównania – na pełnym grafie
    base_edge_flow = compute_edge_flow(G, N)

    for _ in range(num_trials):
        # Losowo wybieramy, które krawędzie przetrwają
        surviving_edges = [e for e in G.edges() if random.random() < p_edge]
        H = nx.Graph()
        H.add_nodes_from(G.nodes())
        H.add_edges_from(surviving_edges)
        # Sprawdź, czy graf przetrwał (jest spójny)
        if not nx.is_connected(H):
            continue  # symulacja niezaliczona
        # Obliczamy przepływ w nowym (przetrwałym) grafie
        trial_edge_flow = compute_edge_flow(H, N)
        # Na podstawie przepustowości z oryginalnej topologii (przyjmujemy, że się nie zmieniają)
        T_trial = compute_delay(trial_edge_flow, capacity, total_flow, packet_size)
        if T_trial < T_max:
            successes += 1
    reliability = successes / num_trials
    return reliability


# Funkcja eksperymentalna: skalowanie macierzy natężeń (wzrost ruchu)
def experiment_scaling_N(G, N, capacity, p_edge, T_max, packet_size, num_trials, factors):
    reliabilities = []
    for factor in factors:
        # Skaluje macierz N
        N_scaled = {k: int(v * factor) for k, v in N.items()}
        reliability = simulate_reliability(G, N_scaled, capacity, p_edge, T_max, packet_size, num_trials)
        reliabilities.append(reliability)
    return reliabilities


# Funkcja eksperymentalna: skalowanie przepustowości (zwiększenie marginesu)
def experiment_scaling_capacity(G, N, capacity, p_edge, T_max, packet_size, num_trials, factors):
    reliabilities = []
    # Wyznaczamy przepływ na krawędziach dla oryginalnego N
    base_edge_flow = compute_edge_flow(G, N)
    for factor in factors:
        # Skalujemy przepustowości – zwiększamy margines o czynnik factor
        cap_scaled = {e: base_edge_flow[e] + factor * (capacity[e] - base_edge_flow[e]) for e in capacity}
        reliability = simulate_reliability(G, N, cap_scaled, p_edge, T_max, packet_size, num_trials)
        reliabilities.append(reliability)
    return reliabilities


# Funkcja eksperymentalna: stopniowe dodawanie nowych krawędzi do topologii
def experiment_scaling_topology(G, N, capacity, p_edge, T_max, packet_size, num_trials, extra_edges_list):
    reliabilities = []
    # Lista krawędzi, których nie ma w G, które potencjalnie można dodać
    potential_edges = [(u, v) for u in G.nodes() for v in G.nodes() if u < v and not G.has_edge(u, v)]
    random.shuffle(potential_edges)
    for extra_count in extra_edges_list:
        # Tworzymy kopię oryginalnego grafu i dodajemy extra_count krawędzi
        H = G.copy()
        for e in potential_edges[:extra_count]:
            H.add_edge(*e)
        # Przeliczamy przepływ dla nowego grafu H
        new_edge_flow = compute_edge_flow(H, N)
        # Przyjmujemy, że dla nowych krawędzi przepustowości są obliczane analogicznie (dodajemy margines)
        new_capacity = {}
        for e in H.edges():
            e_key = tuple(sorted(e))
            # Jeśli krawędź była w oryginalnym grafie, użyjemy jej przepustowości; inaczej przypiszemy c = a(e) + slack
            if e_key in capacity:
                new_capacity[e_key] = capacity[e_key]
            else:
                slack = random.randint(1000, 5000)
                new_capacity[e_key] = new_edge_flow[e_key] + slack
        reliability = simulate_reliability(H, N, new_capacity, p_edge, T_max, packet_size, num_trials)
        reliabilities.append(reliability)
    return reliabilities


# Główna część – generacja danych i przeprowadzenie eksperymentów
if __name__ == '__main__':
    # Generowanie grafu
    G = generate_graph(NUM_NODES, NUM_EDGES)
    print("Wygenerowany graf:", G.number_of_nodes(), "wierzchołków,", G.number_of_edges(), "krawędzi")

    # Rysujemy wygenerowany graf
    plt.figure(figsize=(6, 6))
    pos = nx.spring_layout(G, seed=42)
    nx.draw(G, pos, with_labels=True, node_color='lightblue', edge_color='gray')
    plt.title("Topologia sieci G")
    plt.show()

    # Generujemy macierz natężeń N
    N = generate_N(NUM_NODES, DENSITY_N, MIN_N_VALUE, MAX_N_VALUE)
    total_N = sum(N.values())
    print("Suma elementów N (całkowity ruch):", total_N)

    # Wyznaczamy przepływy na krawędziach
    base_edge_flow = compute_edge_flow(G, N)
    # Przypisujemy przepustowości – gwarantujemy c(e) > a(e)
    capacity = assign_capacity(base_edge_flow)

    # Obliczamy opóźnienie dla oryginalnego, pełnego grafu
    T_original = compute_delay(base_edge_flow, capacity, total_N, PACKET_SIZE)
    print("Opóźnienie T (pełny graf):", T_original)

    # Symulacja niezawodności przy ustalonych parametrach
    reliability_base = simulate_reliability(G, N, capacity, P_EDGE, T_MAX, PACKET_SIZE, NUM_TRIALS)
    print("Szacowana niezawodność (Pr[T < T_max]) przy ustawieniach bazowych:", reliability_base)

    # Eksperyment 1: Stopniowe zwiększanie natężenia ruchu
    factors = np.linspace(0.5, 2.0, 10)  # skaluje macierz N od 0.5x do 2x
    reliability_N = experiment_scaling_N(G, N, capacity, P_EDGE, T_MAX, PACKET_SIZE, NUM_TRIALS, factors)
    plt.plot(factors, reliability_N, marker='o')
    plt.xlabel("Czynnik skalujący macierz N")
    plt.ylabel("Niezawodność (Pr[T < T_max])")
    plt.title("Wpływ zwiększenia ruchu na niezawodność")
    plt.grid(True)
    plt.show()

    # Eksperyment 2: Stopniowe zwiększanie przepustowości (zmiana marginesu)
    factors_cap = np.linspace(0.5, 2.0, 10)  # skaluje przepustowości
    reliability_cap = experiment_scaling_capacity(G, N, capacity, P_EDGE, T_MAX, PACKET_SIZE, NUM_TRIALS, factors_cap)
    plt.plot(factors_cap, reliability_cap, marker='o', color='green')
    plt.xlabel("Czynnik skalujący przepustowość")
    plt.ylabel("Niezawodność (Pr[T < T_max])")
    plt.title("Wpływ zwiększenia przepustowości na niezawodność")
    plt.grid(True)
    plt.show()

    # Eksperyment 3: Stopniowe dodawanie nowych krawędzi (zmiana topologii)
    extra_edges_list = range(0, 11, 2)  # dodajemy od 0 do 10 krawędzi
    reliability_topo = experiment_scaling_topology(G, N, capacity, P_EDGE, T_MAX, PACKET_SIZE, NUM_TRIALS,
                                                   extra_edges_list)
    plt.plot(list(extra_edges_list), reliability_topo, marker='o', color='red')
    plt.xlabel("Liczba dodanych krawędzi")
    plt.ylabel("Niezawodność (Pr[T < T_max])")
    plt.title("Wpływ zmiany topologii na niezawodność")
    plt.grid(True)
    plt.show()
