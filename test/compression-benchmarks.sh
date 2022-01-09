#!/bin/sh

set -e

size="$2"

input="$1"

shift 2

size_base="$(cat "$input" | wc -c)"

timer_output="$(mktemp)"

time_to_seconds() {
    read time
    minutes="$(echo "$time" | cut -d: -f1)"
    seconds="$(echo "$time" | cut -d: -f2)"

    echo "$minutes + $seconds" | bc -l
}

input_file="$(mktemp)"
output_file="$(mktemp)"

dd bs="$size" count=1 if="$input" of="$input_file"

echo "input size:"
du -h "$input_file"

for util in "$@"
do
    echo
    echo "$util:"
    dd bs=1G if="$input_file" 2>/dev/null | "$util" | dd bs=1G of="$output_file"
    echo "ratio: $(echo "$(du -s "$output_file" | cut -f1) / $(du -s "$input_file" | cut -f1)" | bc -l)"
    #echo "$util:"
    #size="$(cat "$input" | time "$util" 2> "$timer_output" | wc -c)"
    #echo "output/input ratio: $size/$size_base = `echo "$size/$size_base" | bc -l`"
    #elapsed="$(cat "$timer_output" | cut -d' ' -f3 | sed 's/elapsed//' | time_to_seconds)"
    #echo "elapsed: $elapsed"
    #echo -n "Input MBps: "
    #echo "$size_base / ($elapsed * 1000000)" | bc -l
    #echo
done

rm "$timer_output"
rm "$input_file"
rm "$output_file"
