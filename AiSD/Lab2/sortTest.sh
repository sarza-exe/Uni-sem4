#!/bin/bash

# Check if exactly two arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <executable1> <executable2>"
    exit 1
fi

# Assign arguments to variables
exec1="./$1"
exec2="./$2"

for i in {8..32}; do
    $exec1 $i | $exec2
    echo
done;