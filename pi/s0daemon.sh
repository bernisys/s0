#!/bin/bash

if [ ! -f s0.ini ] ; then
  echo "please specify a location for s0.ini"
  echo "example:"
  echo "LAST=/var/run/s0.last"
  echo "SAVE=/var/cache/s0.last"
  exit 1
fi

. s0.ini

if [ "$LAST" = "" ] ; then
  echo "LAST not set in s0.ini, set this to a RAM filesystem."
  ERR=1
fi
if [ "$SAVE" = "" ] ; then
  echo "SAVE not set in s0.ini, set this to a resident filesystem."
  ERR=1
fi

if [ "$ERR" != "" ] ; then
  exit 1;
fi



if [ "$1" = "" ] ; then
  echo "start / stop / restart"
  exit 1
fi

case $1 in
  start)
    if [ "$SAVE" -nt "$LAST" ] ; then
      cp -f "$SAVE" "$LAST"
    fi
    nohup ./s0 >/dev/null 2>&1 &
    ;;

  stop)
    killall s0
    if [ "$LAST" -nt "$SAVE" ] ; then
      cp -f "$LAST" "$SAVE"
    fi
    ;;

  restart)
    $0 stop
    $0 start
    ;;
esac


