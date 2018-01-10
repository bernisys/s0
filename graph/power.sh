#!/bin/sh

BASEPARAM="--width 600 --height 200 --lazy --slope-mode --alt-autoscale-max --alt-y-grid -l 0 -u 1000 --font TITLE:13"

for GPIO in 0 1 2 3 ; do

  rrdtool graph "S0_"$GPIO"_1h.png" \
    --start -1h \
    --width 600 \
    --height 200 \
    --lazy \
    --slope-mode \
    --alt-autoscale-max \
    --alt-y-grid \
    -l 0 -u 1000 \
    --font "TITLE:13" \
    --title "S0.$GPIO Power (Watt) last 1h" \
    DEF:kWh_avg=rrds/S0_"$GPIO".rrd:kWh:AVERAGE \
    CDEF:time=kWh_avg,POP,TIME \
    CDEF:time_p="PREV(time)" \
    CDEF:time_pp="PREV(time_p)" \
    CDEF:time_ppp="PREV(time_pp)" \
    CDEF:time_pppp="PREV(time_ppp)" \
    CDEF:time_ppppp="PREV(time_pppp)" \
    CDEF:dt=time,time_ppppp,- \
    CDEF:kWh_avg_p="PREV(kWh_avg)" \
    CDEF:kWh_avg_pp="PREV(kWh_avg_p)" \
    CDEF:kWh_avg_ppp="PREV(kWh_avg_pp)" \
    CDEF:kWh_avg_pppp="PREV(kWh_avg_ppp)" \
    CDEF:kWh_avg_ppppp="PREV(kWh_avg_pppp)" \
    CDEF:w_avg="kWh_avg,kWh_avg_ppppp,-,dt,/,3600000,*" \
    VDEF:w_avgmin=w_avg,MINIMUM \
    VDEF:w_avgavg=w_avg,AVERAGE \
    VDEF:w_avgmax=w_avg,MAXIMUM \
    TEXTALIGN:left \
    COMMENT:"      Minimum   Average   Maximum\n" \
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
      --width 600 \
      --height 200 \
      --lazy \
      --slope-mode \
      --alt-autoscale-max \
      --alt-y-grid \
      -l 0 -u 1000 \
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


for TIME in 1h:1h 6h:6h 1d:day 1w:week 1m:month 1y:year ; do
  START=$(echo $TIME | sed -e "s/:.*//")
  TIMEFRAME=$(echo $TIME | sed -e "s/.*://")

  rrdtool graph S0_all_$TIMEFRAME.png \
    --start -$START \
    --width 600 \
    --height 200 \
    --lazy \
    --slope-mode \
    --alt-autoscale-max \
    --alt-y-grid \
    -l 0 -u 1000 \
    --font "TITLE:13" \
    --title "All Power (Watt) last $TIMEFRAME" \
    DEF:kWh_0=rrds/S0_0.rrd:kWh:AVERAGE \
    DEF:kWh_1=rrds/S0_1.rrd:kWh:AVERAGE \
    DEF:kWh_2=rrds/S0_2.rrd:kWh:AVERAGE \
    DEF:kWh_3=rrds/S0_3.rrd:kWh:AVERAGE \
    CDEF:time=kWh_0,POP,TIME \
    CDEF:time_p="PREV(time)" \
    CDEF:time_pp="PREV(time_p)" \
    CDEF:time_ppp="PREV(time_pp)" \
    CDEF:time_pppp="PREV(time_ppp)" \
    CDEF:time_ppppp="PREV(time_pppp)" \
    CDEF:dt=time,time_ppppp,- \
    CDEF:kWh_0_p="PREV(kWh_0)" \
    CDEF:kWh_0_pp="PREV(kWh_0_p)" \
    CDEF:kWh_0_ppp="PREV(kWh_0_pp)" \
    CDEF:kWh_0_pppp="PREV(kWh_0_ppp)" \
    CDEF:kWh_0_ppppp="PREV(kWh_0_pppp)" \
    CDEF:w_0="kWh_0,kWh_0_ppppp,-,dt,/,3600000,*" \
    VDEF:w_0_min=w_0,MINIMUM \
    VDEF:w_0_avg=w_0,AVERAGE \
    VDEF:w_0_max=w_0,MAXIMUM \
    CDEF:kWh_1_p="PREV(kWh_1)" \
    CDEF:kWh_1_pp="PREV(kWh_1_p)" \
    CDEF:kWh_1_ppp="PREV(kWh_1_pp)" \
    CDEF:kWh_1_pppp="PREV(kWh_1_ppp)" \
    CDEF:kWh_1_ppppp="PREV(kWh_1_pppp)" \
    CDEF:w_1="kWh_1,kWh_1_ppppp,-,dt,/,3600000,*" \
    VDEF:w_1_min=w_1,MINIMUM \
    VDEF:w_1_avg=w_1,AVERAGE \
    VDEF:w_1_max=w_1,MAXIMUM \
    CDEF:kWh_2_p="PREV(kWh_2)" \
    CDEF:kWh_2_pp="PREV(kWh_2_p)" \
    CDEF:kWh_2_ppp="PREV(kWh_2_pp)" \
    CDEF:kWh_2_pppp="PREV(kWh_2_ppp)" \
    CDEF:kWh_2_ppppp="PREV(kWh_2_pppp)" \
    CDEF:w_2="kWh_2,kWh_2_ppppp,-,dt,/,3600000,*" \
    VDEF:w_2_min=w_2,MINIMUM \
    VDEF:w_2_avg=w_2,AVERAGE \
    VDEF:w_2_max=w_2,MAXIMUM \
    CDEF:kWh_3_p="PREV(kWh_3)" \
    CDEF:kWh_3_pp="PREV(kWh_3_p)" \
    CDEF:kWh_3_ppp="PREV(kWh_3_pp)" \
    CDEF:kWh_3_pppp="PREV(kWh_3_ppp)" \
    CDEF:kWh_3_ppppp="PREV(kWh_3_pppp)" \
    CDEF:w_3="kWh_3,kWh_3_ppppp,-,dt,/,3600000,*" \
    VDEF:w_3_min=w_3,MINIMUM \
    VDEF:w_3_avg=w_3,AVERAGE \
    VDEF:w_3_max=w_3,MAXIMUM \
    TEXTALIGN:left \
    COMMENT:"      Minimum   Average   Maximum\n" \
    AREA:"w_0#ff0000:W" \
    GPRINT:w_0_min:"%6.2lf%S" \
    GPRINT:w_0_avg:"%6.2lf%S" \
    GPRINT:w_0_max:"%6.2lf%S" \
    COMMENT:"\n" \
    STACK:"w_1#00ff00:W" \
    GPRINT:w_1_min:"%6.2lf%S" \
    GPRINT:w_1_avg:"%6.2lf%S" \
    GPRINT:w_1_max:"%6.2lf%S" \
    COMMENT:"\n" \
    STACK:"w_2#0000ff:W" \
    GPRINT:w_2_min:"%6.2lf%S" \
    GPRINT:w_2_avg:"%6.2lf%S" \
    GPRINT:w_2_max:"%6.2lf%S" \
    COMMENT:"\n" \
    STACK:"w_3#ffff00:W" \
    GPRINT:w_3_min:"%6.2lf%S" \
    GPRINT:w_3_avg:"%6.2lf%S" \
    GPRINT:w_3_max:"%6.2lf%S" \
    COMMENT:"\n"

done


mv S0*.png /home/user/berni/public_html/powermeter_sub/power/

