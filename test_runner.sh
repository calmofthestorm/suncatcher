#!/bin/sh

#TODO real solution
export GTEST_TOTAL_SHARDS=4
export SLOWTEST=0
export GTEST_SHARD_INDEX=0
bin/run_unit_tests --gtest_filter=-*SLOW* --gtest_color=yes --gtest_shuffle
export GTEST_SHARD_INDEX=1
bin/run_unit_tests --gtest_filter=-*SLOW* --gtest_color=yes --gtest_shuffle
export GTEST_SHARD_INDEX=2
bin/run_unit_tests --gtest_filter=-*SLOW* --gtest_color=yes --gtest_shuffle
export GTEST_SHARD_INDEX=3
bin/run_unit_tests --gtest_filter=-*SLOW* --gtest_color=yes --gtest_shuffle
A=$!
if [ $A ]; then echo "hi"; fi
