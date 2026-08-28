#!/bin/bash

./capkc/build/capkc --cnf "$1"
d4 -dDNNF "$1.up" -out="$1.unnf"
