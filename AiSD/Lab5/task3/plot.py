import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys

# PLOT 1

df = pd.read_csv(f'avg_comparisons_per_op.csv')

if not {'n', 'avg_comparisons'}.issubset(df.columns):
    raise ValueError("Wrong columns.")

# extract
n = df['n']
avg_comparisons = df['avg_comparisons']

# plot
plt.figure(figsize=(10, 6))
plt.plot(n, avg_comparisons, marker='o', label="Average comparisons")

n_values = list(range(100, 10001, 100))
log_curve = np.log2(n_values)
# scale it to line up better with data
scale = 2.65
log_curve *= scale
plt.plot(n_values,log_curve,color='red', linestyle='--',linewidth=1.5,label=f"{scale:.2f}*log₂(N)")

plt.xlabel('n')
plt.ylabel('Average Comparisons')
plt.title('Average Comparisons per operation')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f'plot')
plt.show()





# PLOT 2

df = pd.read_csv('experiments.csv', header=None)

n = 500
total_ops = 4 * n + 1  # 2001

# Indexes
# - inserting to H1:     0 .. n-1
# - inserting to H2:     n .. 2n-1
# - union:  2n   (just one index)
# - extract-min:       2n+1 .. 4n

fig, axes = plt.subplots(3, 1, figsize=(10, 12), sharex=False)

colors = plt.cm.tab10.colors

# inserting (H1 i H2 together) 
ax0 = axes[0]
for idx in range(len(df)):
    # joint inserts to H1 and H2 (0:500 and 500:1000)
    ax0.plot(
        list(range(1, n+1)),
        df.iloc[idx, 0:n], 
        color=colors[idx], linewidth=0.5, alpha=1
    )
    ax0.plot(
        list(range(1, n+1)),
        df.iloc[idx, n:2*n],
        color=colors[idx], linewidth=0.5, alpha=1, linestyle='--'
    )
ax0.set_title('Insert Operations (H1 i H2)')
ax0.set_ylabel('Comparisons per Insert')
ax0.legend([f'Exp {i+1}' for i in range(len(df))], loc='upper right', fontsize='small')
ax0.grid(alpha=0.3)

# union (1 point per experiment)
ax1 = axes[1]
union_idx = 2*n 
x_union = [1] * len(df)

for idx in range(len(df)):
    ax1.bar(
        idx + 1, 
        df.iloc[idx, union_idx],
        color=colors[idx],
        alpha=0.8
    )
ax1.set_title('Heap-Union (one bar per experiment)')
ax1.set_xlabel('Experiment #')
ax1.set_ylabel('Comparisons in Union')
ax1.set_xticks(range(1, len(df)+1))
ax1.grid(axis='y', alpha=0.3)

# extract-min last 1000 point
ax2 = axes[2]
start_ext = 2*n + 1 
end_ext = 4*n + 1

for idx in range(len(df)):
    x_vals = range(1, (2*n)+1)
    y_vals = df.iloc[idx, start_ext : start_ext + 2*n]
    ax2.plot(x_vals, y_vals, color=colors[idx], linewidth=0.5, alpha=1)

ax2.set_title('Extract-Min Operations')
ax2.set_xlabel('Extract-Min Index (1..1000)')
ax2.set_ylabel('Comparisons per Extract-Min')
ax2.legend([f'Exp {i+1}' for i in range(len(df))], loc='upper right', fontsize='small')
ax2.grid(alpha=0.3)

plt.tight_layout()
plt.savefig("experimentHistory")
plt.show()
