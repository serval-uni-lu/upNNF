#!/bin/bash

docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/capkc --cnf "$1"

docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/wrap 16000 3600 \
    /capkc/d4 -dDNNF "$1.up" -out="$1.unnf"

