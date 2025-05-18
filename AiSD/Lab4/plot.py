import pandas as pd
import matplotlib.pyplot as plt
import sys

if len(sys.argv) >= 2:
        dir = sys.argv[1]
else:
    print("Pass directory name!")
    exit(-1)

df = pd.read_csv(f'{dir}/bst_stats.csv')

metrics = [
    ('avg_comp', 'Average comparisons'),
    ('max_comp', 'Max comparisons'),
    ('avg_reads', 'Average pointer reads'),
    ('max_reads', 'Max pointer reads'),
    ('avg_writes', 'Average pointer writes'),
    ('max_writes', 'Max pointer writes'),
    ('avg_height', 'Average height'),
    ('max_height', 'Max height'),
]

line_styles = {
    'insert': '-',
    'remove': '--'
}

for col, label in metrics:
    plt.figure()
    if 'height' in col:
        plt.yscale('log')
    for scen in df['scenario'].unique():
        for op in df['op'].unique():
            sub = df[(df['scenario'] == scen) & (df['op'] == op)]
            if sub.empty:
                continue
            plt.plot(sub['n'], sub[col],
                     marker='o',
                     linestyle=line_styles.get(op, '-'),
                     label=f"{scen} ({op})")
    plt.title(f'{label}')
    plt.xlabel('n')
    plt.ylabel(label)
    plt.legend(title="Scenario (op)")
    plt.grid(True, which='both')
    plt.savefig(f'{dir}/plots/{col}.png')
