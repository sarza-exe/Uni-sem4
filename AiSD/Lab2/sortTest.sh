#!/bin/bash

# Check if sort name was provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <SortName> (e.g., InsertSort)"
    exit 1
fi

# Construct the sorting program path
sort_name=$1
sorting_program="./$sort_name"

# Define data generators
data_generators=("./data_random" "./data_desc" "./data_asc")

# Loop through generators and input sizes
for generator in "${data_generators[@]}"; do
    for i in {8..32}; do
        echo "$generator $i | $sorting_program"
        $generator $i | $sorting_program
        echo
    done
done