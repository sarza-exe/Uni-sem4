import pandas as pd
import matplotlib.pyplot as plt
import sys

if len(sys.argv) >= 2:
        dir = sys.argv[1]
else:
    print("Pass directory name!")
    exit(-1)

df = pd.read_csv(f'{dir}/stats.csv')

# Oczekiwane kolumny: 'n', 'avg_time_kruskal', 'avg_time_prim'
if not {'n', 'avg_time_kruskal', 'avg_time_prim'}.issubset(df.columns):
    raise ValueError("Plik CSV musi zawierać kolumny 'n', 'avg_time_kruskal', 'avg_time_prim'.")

# Wyciągnięcie wartości
n = df['n']
avg_time_kruskal = df['avg_time_kruskal']
avg_time_prim = df['avg_time_prim']

# Utworzenie wykresu
plt.figure(figsize=(10, 6))
plt.plot(n, avg_time_kruskal, marker='o', label="Kruskal's Algorithm")
plt.plot(n, avg_time_prim, marker='s', label="Prim's Algorithm")
plt.xlabel('Number of vertices (n)')
plt.ylabel('Average Time (seconds)')
plt.title('Average Running Time of Kruskal vs Prim')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f'{dir}/plot')

# Wyświetlenie wykresu
plt.show()
