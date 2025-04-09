def load_data(file_path):
    with open(file_path, "r") as file:
        comps = []
        swaps = []
        for line in file:
            split = line.split(" ")
            split = split[0:200]
            comps.append(split[0::2])
            swaps.append(split[1::2])
    return comps, swaps

file_paths = ["data/insertSortData.txt", "data/quickSortData.txt", "data/hybridSortData.txt", "data/DPQuickSortData.txt", "data/mergeSortData.txt", "data/newSortData.txt"]
titles = [
    "Avg_no_comps",
    "Avg_no_swaps",
    "Comps_divided_by_N",
    "Swaps_divided_by_N"
]
labels = ["Insertion Sort", "Quick Sort", "Hybrid Sort", "DP Quick Sort", "Merge Sort", "New Sort"]
colors = ["silver", "magenta", "green", "blue", "black", "red"]
subfolder = "N_from_10_to_50/"

comps = []
swaps = []
for file_path in file_paths:
    c, s = load_data(file_path)
    comps.append(c)
    swaps.append(s)

k_values = [10, 100]

import matplotlib.pyplot as plt

n_values = [10, 20, 30, 40, 50]
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
        plt.plot(n_values, av_comps[i], color=colors[i], linewidth=2, label=labels[i])

    plt.xlabel("Array size")
    plt.ylabel("Average comparisons")
    plt.title(f"Average Comparisons for {k} reps")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + subfolder + titles[0] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()

    for i in range(0, len(file_paths)):
        plt.plot(n_values, av_swaps[i], color=colors[i], linewidth=2, label=labels[i])

    plt.xlabel("Array size")
    plt.ylabel("Average Swaps")
    plt.title(f"Average Swaps for {k} reps")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + subfolder + titles[1] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()

    for i in range(0, len(file_paths)):
        plt.plot(n_values, [c / n for c, n in zip(av_comps[i], n_values)], color=colors[i], linewidth=2, label=labels[i])

    plt.xlabel("Array size (N)")
    plt.ylabel("Average Comparisons / N")
    plt.title(f"Average Comparisons / N for {k} reps")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + subfolder + titles[2] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()

    for i in range(0, len(file_paths)):
        plt.plot(n_values, [c / n for c, n in zip(av_swaps[i], n_values)], color=colors[i], linewidth=2, label=labels[i])

    plt.xlabel("Array size (N)")
    plt.ylabel("Average Swaps / N")
    plt.title(f"Average Swaps / N for {k} reps")
    plt.legend()  # <-- Show legend with algorithm names
    name = "plots/" + subfolder +  titles[3] + str(k) + ".png"
    plt.savefig(name)
    plt.clf()