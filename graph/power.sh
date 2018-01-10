#!/bin/sh

for GPIO in 0 1 2 3 ; do

  rrdtool graph "S0_"$GPIO"_1h.png" \
    --start -1h \
    --width 800 \
    --height 300 \
    --lazy \
    --slope-mode \
    --alt-autoscale \
    --alt-y-grid \
    --font "TITLE:13" \
    --title "S0.$GPIO Power (Watt) last 1h" \
    DEF:kWh_avg=rrds/S0_"$GPIO".rrd:kWh:AVERAGE \
    CDEF:time=kWh_avg,POP,TIME \
    CDEF:w_avg="kWh_avg,PREV(kWh_avg),-,time,PREV(time),-,/,3600000,*" \
    VDEF:w_avgmin=w_avg,MINIMUM \
    VDEF:w_avgavg=w_avg,AVERAGE \
    VDEF:w_avgmax=w_avg,MAXIMUM \
    TEXTALIGN:left \
    COMMENT:"      Average\n" \
    LINE1:w_avg#ff0000:"W" \
    GPRINT:"w_avgmin:%6.2lf%S" \
    GPRINT:"w_avgavg:%6.2lf%S" \
    GPRINT:"w_avgmax:%6.2lf%S" \
    COMMENT:"\n"

  for TIME in 6h:6h 1d:day 1w:week 1m:month 1y:year ; do
    START=$(echo $TIME | sed -e "s/:.*//")
    TIMEFRAME=$(echo $TIME | sed -e "s/.*://")

    rrdtool graph S0_"$GPIO"_$TIMEFRAME.png \
      --start -$START \
      --width 800 \
      --height 300 \
      --lazy \
      --slope-mode \
      --alt-autoscale \
      --alt-y-grid \
      --font "TITLE:13" \
      --title "S0.$GPIO Power (Watt) last $TIMEFRAME" \
      DEF:kWh_min=rrds/S0_"$GPIO".rrd:kWh:MIN \
      DEF:kWh_avg=rrds/S0_"$GPIO".rrd:kWh:AVERAGE \
      DEF:kWh_max=rrds/S0_"$GPIO".rrd:kWh:MAX \
      CDEF:time=kWh_min,POP,TIME \
      CDEF:dt="time,PREV(time),-" \
      CDEF:w_min="kWh_min,PREV(kWh_min),-,dt,/,3600000,*" \
      CDEF:w_avg="kWh_avg,PREV(kWh_avg),-,dt,/,3600000,*" \
      CDEF:w_max="kWh_max,PREV(kWh_max),-,dt,/,3600000,*" \
      CDEF:w_diff=w_max,w_min,- \
      VDEF:w_minmin=w_min,MINIMUM \
      VDEF:w_avgavg=w_avg,AVERAGE \
      VDEF:w_maxmax=w_max,MAXIMUM \
      TEXTALIGN:left \
      COMMENT:"      Minimum   Average   Maximum\n" \
      AREA:w_min#ffffff \
      AREA:w_diff#ffb2b2::STACK \
      LINE1:w_avg#ff0000:"W" \
      GPRINT:w_minmin:"%6.2lf%S" \
      GPRINT:w_avgavg:"%6.2lf%S" \
      GPRINT:w_maxmax:"%6.2lf%S" \
      COMMENT:"\n"

  done
done

mv S0*.png /home/user/berni/public_html/powermeter_sub/power/

