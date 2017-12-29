#! /bin/bash

for d in 0 1 2 3 4 5 6; do
    (
        cd "$d" || exit
        rm mip.png || true
        convert -append $(ls | sort -n) mip.png
    )
done

find . -type f -name "*.png" -exec mogrify -strip {} \;
