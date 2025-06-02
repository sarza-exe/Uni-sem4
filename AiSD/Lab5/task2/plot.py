import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('results.csv')

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(df['n'], df['avg_rounds'], marker='o', label='Average Rounds')
plt.plot(df['n'], df['min_rounds'], marker='^', label='Min Rounds')
plt.plot(df['n'], df['max_rounds'], marker='s', label='Max Rounds')

# Labels, title, legend
plt.xlabel('Number of Nodes (n)')
plt.ylabel('Number of Rounds')
plt.title('Broadcast Rounds in Relation to the Number of Nodes')
plt.legend()
plt.grid(True)

# Show the plot
plt.tight_layout()
plt.savefig(f'plot')
plt.show()
