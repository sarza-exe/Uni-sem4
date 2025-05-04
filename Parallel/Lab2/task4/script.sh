#!/usr/bin/env bash
# loop indefinitely
while true; do
  # capture the program’s output
  output=$(go run ex4.go)
  # check for the '*' symbol
  if echo "$output" | grep -q '\*'; then
    # save the run that contained '*' to out2
    printf "%s\n" "$output" > out3
    echo "Found '*' — saved output to out3"
    break
  fi
  # optional: small pause to avoid busy-looping
  sleep 0.1
done
