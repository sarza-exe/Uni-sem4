import matplotlib.pyplot as plt

# def load_data(file_path):
#     with open(file_path, "r") as file:
#         comps = []
#         swaps = []
#         for line in file:
#             split = line.split(" ")
#             split = split[0:100]
#             comps.append(split[0::2])
#             swaps.append(split[1::2])
#     return comps, swaps

def load_data(file_path):
    """
    Expects each line like:
      comp1 swap1 comp2 swap2 … compN swapN avg_time
    Returns three parallel lists-of-lists:
      comps : [[comp1, comp2, …], …]
      swaps : [[swap1, swap2, …], …]
      times : [avg_time_line1, avg_time_line2, …]
    """
    comps = []
    swaps = []
    times = []
    with open(file_path, "r") as f:
        for line in f:
            tokens = line.split(" ")
            if not tokens:
                continue

            # Last token is the average time for that line
            avg_time = float(tokens.pop())

            # Now tokens are [comp, swap, comp, swap, …]
            comps.append(tokens[0::2])
            swaps.append(tokens[1::2])
            times.append(avg_time)

    return comps, swaps, times

file_paths = ["data/selectDataG3.txt", "data/selectDataG5.txt", "data/selectDataG7.txt", "data/selectDataG9.txt"]
titles = [
    "select_cmp",
    "select_swaps",
    "select_time",
]
labels = ["k = 3", "k = 5", "k = 7", "k = 9"]
colors = ["black", "red", "blue", "purple"]
subfolder = "task3/"

comps = []
swaps = []
times = []
for file_path in file_paths:
    c, s, t = load_data(file_path)
    comps.append(c)
    swaps.append(s)
    times.append(t)

k_values = [50]

n_values = list(range(100, 50001, 100))
for k in k_values:
    av_comps = []
    av_swaps = []
    for i in range(0, len(file_paths)):
        i_av_comps = []
        i_av_swaps = []
        for j in range(0, len(n_values)):
            i_av_comps.append(sum(map(int, comps[i][j][:k])) / k)
            i_av_swaps.append(sum(map(int, swaps[i][j][:k])) / k)
        av_comps.append(i_av_comps)
        av_swaps.append(i_av_swaps)

    for i in range(0, len(file_paths)):
        plt.plot(n_values, av_comps[i], color=colors[i], linewidth=1.2, label=labels[i])

    plt.xlabel("Array size")
    plt.ylabel("Average comparisons")
    plt.title(f"Average Comparisons of Select")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + titles[0] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()

    for i in range(0, len(file_paths)):
        plt.plot(n_values, av_swaps[i], color=colors[i], linewidth=1.2, label=labels[i])

    plt.xlabel("Array size")
    plt.ylabel("Average swaps")
    plt.title(f"Average Swaps of Select")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + titles[1] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()

    for i in range(0, len(file_paths)):
        plt.plot(n_values, times[i] , color=colors[i], linewidth=1.2, label=labels[i])

    plt.xlabel("Array size (N)")
    plt.ylabel("Average times")
    plt.title(f"Average Times of Select")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + titles[2] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()