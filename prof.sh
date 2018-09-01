#! /bin/bash

set -e

config=profile
filter=${1:-*}

python genmake.py
echo make utest config=${config}
make utest config=${config}

echo ./build/${config}/utest --gtest_filter="$filter"
./build/${config}/utest --gtest_filter="$filter"

gprof build/profile/utest gmon.out > prof_result

echo check the file prof_result

