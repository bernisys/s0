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
  'DB1' => { 'type' => 'GAUGE', 'rows' => ['value:0:U'] },
  'DB2' => { 'type' => 'GAUGE', 'rows' => ['value1:-100:100', 'value2:-100:100'] },
  'DB3' => { 'type' => 'GAUGE', 'rows' => ['value:-500:2500'] },
  'DB3_sub' => { 'type' => 'GAUGE', 'rows' => ['value1:-1:1', 'value2:0:10'] },
  'DB3_sub_part' => { 'type' => 'GAUGE', 'rows' => ['value1:-U:U', 'value2:-U:U'] }
};

my $ref_resolutions = ['12H@10S', '14d@1M', '4w@10M', '6m@1H', '5y@12H'];

BPrrd::create_all('./rrds/', $ref_config, 10, $ref_resolutions);



my $ref_values = {
  'DB1' => { 'value' => '1234.567' },
  'DB2' => { 'value1' => '1234.567', 'value2' => '2345.678' },
  'DB3' => { 'value' => '298.806',
    'sub' => { 'value1' => '11.222', 'value2' => '12.345',
      'part' => { 'value1' => '33.444', 'value2' => '54.321' },
    },
  },
};

BPrrd::feed_all('./rrds/', $ref_values);

