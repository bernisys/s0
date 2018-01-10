#!/usr/bin/perl
 
package BPrrd;

use strict;
use warnings;
use diagnostics;
 
$| = 1;
 
use Data::Dumper;
$Data::Dumper::Sortkeys = 1;
$Data::Dumper::Indent = 1;
 
use RRDs;


# example for multi RRD creation:
# 
# In this example we will handle 5 DB files in total:
#  - DB1 with RRA name "value" from 0 to infinity
#  - DB2 with RRA names "value1" and "value2" each from -100 to 100
#  - DB3 with RRA name "value" from -500 to 2500
#  - DB3_sub with RRA names "value1" from -1 to 1 and "value2" from 0 to 10
#  - DB3_sub_part with RRA names "value1" and "value2" with all valid numbers
#
# all DBs shall be of the "GAUGE" type (see rrdtool manual)
#
# all DBs shall be created with the same resolution, keeping
#  - 12 hours worth of data with a resolution of 10 seconds
#  - 14 days at 1 minute resolution
#  - 4 weeks at 10 minutes
#  - 6 months at 1 hour
#  - 5 years at 12 hours
#
# We use this configuration for the time lines and resolutions:
# my $ref_resolutions = ['12H@10S', '14d@1M', '4w@10M', '6m@1H', '5y@12H'];
#
# And this one takes care of the DBs:
# my $ref_config = {
#   'DB1' => { 'type' => 'GAUGE', 'rows' => ['value:0:U'] },
#   'DB2' => { 'type' => 'GAUGE', 'rows' => ['value1:-100:100', 'value2:-100:100'] },
#   'DB3' => { 'type' => 'GAUGE', 'rows' => ['value:-500:2500'] },
#   'DB3_sub' => { 'type' => 'GAUGE', 'rows' => ['value1:-1:1', 'value2:0:10'] },
#   'DB3_sub_part' => { 'type' => 'GAUGE', 'rows' => ['value1:-U:U', 'value2:-U:U'] }
# };
#
# Then we call the creator engine:
# create_all("./rrds", $ref_config, 10, $ref_resolutions);
#
# That's all :)
#
#
# To update the RRD files all in a bunch, create a hash that looks like this:
# my $ref_values = {
#   'DB1' => { 'value' => '1234.567' },
#   'DB2' => { 'value1' => '1234.567', 'value2' => '2345.678' },
#   'DB3' => { 'value' => '298.806' },
#   'DB3_sub' => { 'value1' => '11.222', 'value2' => '12.345' },
#   'DB3_sub_part' => { 'value1' => '33.444', 'value2' => '54.321' },
# };
#
# And call the feeder engine:
# feed_all("./rrd", $ref_values);
#
# Hint:
# You can also refer to a different pattern, which produces the same result.
# If you use sub hashes, these will automatically be concatenated:
#   {'DB3'}->{'sub'}  results in  "DB3_sub"
#   {'DB3'}->{'sub'}->{'part'}  results in  "DB3_sub_part"
#
# my $ref_values = {
#   'DB1' => { 'value' => '1234.567' },
#   'DB2' => { 'value1' => '1234.567', 'value2' => '2345.678' },
#   'DB3' => { 'value' => '298.806',
#     'sub' => { 'value1' => '11.222', 'value2' => '12.345',
#       'part' => { 'value1' => '33.444', 'value2' => '54.321' },
#     },
#   },
# };
#



sub create_all {
  my $folder = shift;
  my $ref_params = shift;
  my $step = shift;
  my $ref_resolutions = shift;

  foreach my $key (keys %{$ref_params})
  {
    my $ref_param = $ref_params->{$key};
    create_rrd($folder, $key, $step, $ref_param, $ref_resolutions);
  }
}


# example for single RRD creation:
# my $ref_hash = {
#   'type' => 'GAUGE',
#   'rows' => ['value1:-20000:20000', 'value2:-20000:20000', 'value3:-20000:20000', ],
# },
#
# my $ref_resolutions = ['12H@10S', '14d@1M', '4w@10M', '6m@1H', '5y@12H', ];
# create_rrd('./rrd', 'DB', 10,  $ref_hash, $ref_resolutions);

sub create_rrd {
  my $folder = shift;
  my $name = shift;
  my $base_step = shift;
  my $ref_params = shift;
  my $ref_resolutions = shift;

  my $type = $ref_params->{'type'};
  my $ref_rows = $ref_params->{'rows'};

  my @rows;
  foreach my $row (@{$ref_rows})
  {
    my ($rowname, $min, $max) = split(/:/, $row);
    push @rows, sprintf('DS:%s:%s:%d:%s:%s', $rowname, $type, 6*$base_step, $min, $max);
  }

  my @resolutions;
  my $count = 0;
  foreach my $resolution (@{$ref_resolutions})
  {
    my ($span, $step) = split(/@/, $resolution);
    my $step_seconds = time_to_seconds($step);
    my $span_seconds = time_to_seconds($span);
    my $factor = 0.3;
    my $num_steps = $span_seconds/$step_seconds;
    my $num_base_steps = $step_seconds/$base_step;
    #printf("Span: %5s %9ds Step: %5s %9ds = %9d\n", $span, $span_seconds, $step, $step_seconds, $num_steps);

    foreach my $consolidation ('AVERAGE', 'MIN', 'MAX')
    {
      push @resolutions, sprintf('RRA:%s:%s:%d:%s', $consolidation, $factor, $num_base_steps, $num_steps);
      last if ($count == 0);
    }
    $count++;
  }

  my $fullpath = $folder.'/'.$name.'.rrd';
  if (! -f $fullpath)
  {
    RRDs::create($fullpath, '--step', $base_step, '--start', '-5y', @rows, @resolutions);
    my $error = RRDs::error();
    if ($error) {
      warn("RRDs error: $error\n");
    }
  }
}

sub time_to_seconds {
  my $timestring = shift;

  my %factors = ( 'S' => 1, 'M' => 60, 'H' => 3600, 'd' => 86400, 'w' => 86400*7, 'm' => 86400*31, 'y' => 86400*366, );

  $timestring =~ /^(\d+)(\w)$/;
  my ($num, $unit) = ($1, $2);
  return 0 if (!exists $factors{$unit});
  return $num * $factors{$unit};
}



sub feed_all {
  my $path = shift || '';
  my $ref_values = shift;
  my $time = int(shift);

  $time = "N" if (!defined $time);

  my %values = ();
  foreach my $key (sort keys %{$ref_values})
  {
    if (ref $ref_values->{$key} eq 'HASH')
    {
      feed_all($path.'_'.$key, $ref_values->{$key}, $time);
    }
    else
    {
      $values{$key} = $ref_values->{$key};
    }
  }
  if (keys %values)
  {
    my $update_pattern = '';
    my $update_values = '';
    foreach my $key (sort keys %values)
    {
      $update_pattern .= ':'.$key;
      $update_values .= ':'.$values{$key};
    }

    $path =~ s/^_//;
    $path =~ s/\/_/\//;
    $path .= '.rrd';
    $update_pattern =~ s/^://;
    $update_values =~ s/^://;
    #print "updating $path with: $update_pattern / $update_values\n";

    RRDs::update($path, '--template', $update_pattern, $time.":".$update_values);
    my $error = RRDs::error();
    if ($error) {
      warn("RRDs error: $error\n");
    }
  }
}



1;
