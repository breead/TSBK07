#!/usr/bin/env bash

old="$1"
new="$2"

for f in "$old".*; do
    ext=${f#"$old".}
    cp -- "$f" "$new.$ext"
done
