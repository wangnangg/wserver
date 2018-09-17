#!/bin/bash
set -e
config=${1}
shift
args=$@

python genmake.py
echo make exe config=${config}
make exe config=${config}

echo ./build/${config}/wserver $args
./build/${config}/wserver $args
