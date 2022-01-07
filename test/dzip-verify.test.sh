#!/bin/busybox sh

set -e

tmp="$(mktemp)"
dd if=/dev/random of="$tmp" bs=1M count=100 2>/dev/null
tar -c --to-stdout src >> "$tmp"

if ! cat "$tmp" | bin/dzip | bin/dzip -d | cmp "$tmp"
then
    echo "dzip output does not match input"
    rm "$tmp"
    false
else
    echo "Output and input match"
    rm "$tmp"
    true
fi
