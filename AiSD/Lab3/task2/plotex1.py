import matplotlib.pyplot as plt

def load_data(file_path):
    with open(file_path, "r") as file:
        comps = []
        swaps = []
        for line in file:
            split = line.split(" ")
            split = split[0:100]
            comps.append(split[0::2])
            swaps.append(split[1::2])
    return comps, swaps

file_paths = ["data/randomSelectData1.txt", "data/selectData1.txt", "data/randomSelectDatanhalfn.txt", "data/selectDatanhalfn.txt", "data/randomSelectDataquatern.txt", "data/selectDataquatern.txt"]
titles = [
    "selection_cmp_k_1",
    "selection_swaps_k_1",
    "selection_cmp_k_half_n",
    "selection_swaps_k_half_n",
    "selection_cmp_k_quarter_n",
    "selection_swaps_k_quarter_n",
]
labels = ["Random Select", "Select"]
colors = ["black", "red"]
subfolder = "task1/"

def plot_metric(n_values, metrics, ylabel, title, out_path):
    """
    Plots the given metrics against n_values and saves the figure.

    Parameters:
    - n_values: list or array of x-axis values (array sizes)
    - metrics: list of metric-series (e.g., [av_comps, av_swaps])
    - ylabel: string label for y-axis
    - title: string title for the plot
    - out_path: file path where the plot will be saved
    """
    plt.figure()
    for idx, series in enumerate(metrics):
        plt.plot(n_values, series, color=colors[idx % len(colors)], linewidth=1.5, label=labels[idx % len(labels)])

    plt.xlabel("Array size")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.savefig(out_path)
    plt.clf()


comps = []
swaps = []
for file_path in file_paths:
    c, s = load_data(file_path)
    comps.append(c)
    swaps.append(s)

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

    stat = ["1", "half of n", "quarter of n"]
    for i in range(0, 3):
        plot_metric(
            n_values=n_values,
            metrics=[av_comps[2*i], av_comps[2*i+1]],
            ylabel="Average comparisons",
            title=f"Average Comparisons for k = {stat[i]}",
            out_path=f"plots/{subfolder}{titles[2*i]}.png"
        )
        plot_metric(
            n_values=n_values,
            metrics=[av_swaps[2 * i], av_swaps[2 * i + 1]],
            ylabel="Average Swaps",
            title=f"Average Swaps for k = {stat[i]}",
            out_path=f"plots/{subfolder}{titles[2*i+1]}.png"
        )