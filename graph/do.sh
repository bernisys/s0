#!/bin/bash

mkdir -p log
NUM=$(ps -aux | grep -c "\.\/rrd_create_insert_hist\.pl")
if [ $NUM = 0 ] ; then
  ./rrd_create_insert_hist.pl
fi
./graph.pl > log/graph.log
./power.sh >> log/graph.log
