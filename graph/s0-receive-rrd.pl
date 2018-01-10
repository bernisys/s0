#!/usr/bin/perl -w
# udpqotd - UDP message server
use strict;
use IO::Socket;

$| = 1;

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


my $MAXLEN = 1024;
my $PORTNO = 22222;

BPrrd::create_all('./rrds/', $ref_config, 10, $ref_resolutions);

my $sock = IO::Socket::INET->new( LocalPort => $PORTNO, Proto => 'udp') or die "socket: $@";
       
print "Listening to UDP port $PORTNO\n";

my @times  = (0, 0, 0, 0, 0, 0, 0, 0);
my @values = (0, 0, 0, 0, 0, 0, 0, 0);

my $newmsg;
while ($sock->recv($newmsg, $MAXLEN)) {
    my($port, $ipaddr) = sockaddr_in($sock->peername);

    print "$newmsg\n";

    open(my $h_file, '>', 'data/PWR-'.$date);
    print $h_file "$newmsg\n";
    close $h_file;

    if ($newmsg =~ /(\d+)\.\d+: (\d) ([\d\.]+)/)
    # 1515296647.994384: 0 4.736000 0.822
    {
      my ($time, $num, $kwh) = ($1, $2, $3);
      $values[$num] = $kwh;

      if ($time > $times[$num])
      {
        #printf("%d: %d=%f\n", $time, $num, $kwh);
        foreach (my $n = 0; $n < 4 ; $n++)
        {
          $val{'S0_'.$n}{'kWh'} = $values[$n];
          BPrrd::feed_all('./rrds/', $val{$time}, $time);
        }
        $times[$num] = $time;
      }
    }
    #$sock->send("CONFIRMED: $newmsg ");
} 
die "recv: $!";
