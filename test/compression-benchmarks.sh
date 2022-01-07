#!/bin/sh

set -e

input="$1"

shift

size_base="$(cat "$input" | wc -c)"

timer_output="$(mktemp)"

time_to_seconds() {
    read time
    minutes="$(echo "$time" | cut -d: -f1)"
    seconds="$(echo "$time" | cut -d: -f2)"

    echo "$minutes + $seconds" | bc -l
}

for util in "$@"
do
    #sleep 1
    echo "$util:"
    size="$(cat "$input" | time "$util" 2> "$timer_output" | wc -c)"
    echo "output/input ratio: $size/$size_base = `echo "$size/$size_base" | bc -l`"
    elapsed="$(cat "$timer_output" | cut -d' ' -f3 | sed 's/elapsed//' | time_to_seconds)"
    echo "elapsed: $elapsed"
    echo -n "Input MBps: "
    echo "$size_base / ($elapsed * 1000000)" | bc -l
#    echo "Full timer info:"
#    cat "$timer_output"
    echo
done

rm "$timer_output"
