#!/bin/bash
set -e
config=${2:-debug}
filter=${1:-*}

python genmake.py
echo make utest config=${config}
make utest config=${config}

echo ./build/${config}/utest --gtest_filter="$filter" --gtest_color=yes
./build/${config}/utest --gtest_filter="$filter" --gtest_color=yes
