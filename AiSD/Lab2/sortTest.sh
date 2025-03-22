#!/bin/bash
data_generators=("./data_random" "./data_desc" "./data_asc")
sorting_programs=("./InsertSort" "./QuickSort" "./HybridSort")

for program in "${sorting_programs[@]}"; do
    for generator in "${data_generators[@]}"; do
        for i in {8..32}; do
            echo "$generator $i | $program"
            $generator $i | $program
            echo
        done;
    done;
done;