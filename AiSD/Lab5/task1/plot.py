import pandas as pd
import matplotlib.pyplot as plt
import sys

df = pd.read_csv(f'stats.csv')

# expecting 'n', 'avg_time_kruskal', 'avg_time_prim'
if not {'n', 'avg_rounds', 'min_rounds', 'max_rounds'}.issubset(df.columns):
    raise ValueError("Wrong columns.")

# extract
n = df['n']
avg_time_kruskal = df['avg_time_kruskal']
avg_time_prim = df['avg_time_prim']

# plot
plt.figure(figsize=(10, 6))
plt.plot(n, avg_time_kruskal, marker='o', label="Kruskal's Algorithm")
plt.plot(n, avg_time_prim, marker='s', label="Prim's Algorithm")
plt.xlabel('Number of vertices (n)')
plt.ylabel('Average Time (seconds)')
plt.title('Average Running Time of Kruskal vs Prim')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f'plot')
plt.show()
