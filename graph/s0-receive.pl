#!/usr/bin/perl -w
# udpqotd - UDP message server
use strict;
use IO::Socket;

$| = 1;

my $MAXLEN = 1024;
my $PORTNO = 22222;

my $sock = IO::Socket::INET->new( LocalPort => $PORTNO, Proto => 'udp') or die "socket: $@";
       
print "Listening to UDP port $PORTNO\n";

my $newmsg;
while ($sock->recv($newmsg, $MAXLEN))
{
  #my($port, $ipaddr) = sockaddr_in($sock->peername);
  #$sock->send("CONFIRMED: $newmsg ");
  my ($sec, $min, $hour, $day, $mon, $yr) = localtime();
  $yr += 1900;
  $mon++;
  my $date = sprintf("%04d-%02d-%02d", $yr, $mon, $day);

  print "$newmsg\n";
  open(my $h_file, '>>', 'data/PWR-'.$date);
  print $h_file "$newmsg\n";
  close $h_file;
} 
die "recv: $!";
