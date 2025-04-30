import matplotlib.pyplot as plt
import numpy as np

def load_data(file_path):
    """
    Expects each line like:
      comp time
    """
    comps = []
    times = []
    with open(file_path, "r") as f:
        for line in f:
            tokens = line.split(" ")
            if not tokens:
                continue

            # Last token is the average time for that line
            avg_time = float(tokens.pop())
            avg_cmp = float(tokens.pop())

            # Now tokens are [comp, swap, comp, swap, …]
            comps.append(avg_cmp)
            times.append(avg_time)

    return comps, times

file_paths = ["data/bsBeginning.txt", "data/bsEnd.txt", "data/bsMiddle.txt", "data/bsNone.txt", "data/bsRandom.txt"]
titles = [
    "bin_search_cmp",
    "bin_search_time",
]
labels = ["v near beginning", "v near end", "v near middle", "v nonexistent", "v random"]
colors = ["black", "gold", "blue", "purple", "brown"]

comps = []
times = []
for file_path in file_paths:
    c, t = load_data(file_path)
    comps.append(c)
    times.append(t)


n_values = list(range(1000, 100001, 1000))




for i in range(0, len(file_paths)):
    plt.plot(n_values, comps[i], color=colors[i], linewidth=1.2, label=labels[i])

log_curve = np.log2(n_values)
# scale it to line up better with data
scale = comps[0][20] / log_curve[20]
log_curve *= scale
plt.plot(n_values,log_curve,color='red', linestyle='--',linewidth=1.5,label=f"{scale:.2f}*log₂(N)")

log_curve = np.log2(n_values)
# scale it to line up better with data
scale = comps[3][17] / log_curve[17]
log_curve *= scale
plt.plot(n_values,log_curve,color='black', linestyle='--',linewidth=1.5,label=f"{scale:.2f}*log₂(N)")


plt.xlabel("Array size (N)")
plt.ylabel("Average comparisons")
plt.title(f"Average Comparisons of Binary Search")
plt.legend()  # Show legend with algorithm names
name = "plots/" + titles[0] + ".png"
plt.savefig(name)
plt.clf()


for i in range(0, len(file_paths)):
    plt.plot(n_values, times[i] , color=colors[i], linewidth=1.2, label=labels[i])

line = np.array(n_values) / 400
plt.plot(n_values,line,color='red', linestyle='--',linewidth=1.5,label=f"N/400")


plt.xlabel("Array size (N)")
plt.ylabel("Average times")
plt.title(f"Average Times of Binary Search")
plt.legend()  # <-- Show legend with algorithm names
name = "plots/" + titles[1] + ".png"
plt.savefig(name)
plt.clf()