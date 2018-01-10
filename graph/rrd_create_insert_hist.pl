#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;

$| = 1;

use Data::Dumper;
$Data::Dumper::Sortkeys = 1;
$Data::Dumper::Indent = 1;

use lib 'lib';

use BPrrd;

my $ref_config = {
  'S0_0' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_1' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_2' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_3' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_4' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_5' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_6' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
  'S0_7' => { 'type' => 'GAUGE', 'rows' => ['kWh:0:U'] },
};

my $ref_resolutions = ['12H@10S', '14d@1M', '4w@10M', '6m@1H', '5y@12H'];

BPrrd::create_all('./rrds/', $ref_config, 10, $ref_resolutions);


my $last = qx(rrdtool last rrds/S0_0.rrd);
chomp $last;

my @times  = (0, 0, 0, 0, 0, 0, 0, 0);
my @values = (0, 0, 0, 0, 0, 0, 0, 0);
my %val;

opendir(my $h_dir, "data");
my @files = grep /^PWR.*\d$/, sort readdir $h_dir;
closedir $h_dir;

print join("\n", @files,"");

foreach my $file (@files)
{

  open(my $h_file, '<', 'data/'.$file);
  foreach my $line (<$h_file>)
  {
    if ($line =~ /(\d+)\.\d+: (\d) ([\d\.]+)/)
    # 1515296647.994384: 0 4.736000 0.822
    {
      my ($time, $num, $kwh) = ($1, $2, $3);
      $values[$num] = $kwh;
      next if $time < $last;

      if ($time > $times[$num])
      {
        #printf("%d: %d=%f\n", $time, $num, $kwh);
        foreach (my $n = 0; $n < 4 ; $n++)
        {
          $val{$time}{'S0_'.$n}{'kWh'} = $values[$n];
        }
        $times[$num] = $time;
      }
    }
  }
  close $h_file;
}

foreach my $time (sort keys %val)
{
  if ($time > $last)
  {
    BPrrd::feed_all('./rrds/', $val{$time}, $time);
  }
}

