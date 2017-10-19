#! /bin/bash

for d in 0 1 2 3 4; do
    (
        cd "$d" || exit
        rm mip.ppm || true
        convert -append $(ls | sort -n) mip.ppm
    )
done
