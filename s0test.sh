#!/bin/bash

while true ; do
  PIN=$(( $RANDOM % 8 ))
  MODE=up
  if [ $(( $RANDOM % 2 )) = 0 ] ; then MODE=down ; fi
  gpio mode $PIN $MODE
done
