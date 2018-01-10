#!/bin/bash

mkdir -p log
./rrd_create_insert_hist.pl
./graph.pl > log/graph.log

