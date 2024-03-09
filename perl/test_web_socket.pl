#!/usr/bin/perl

use Mojo::UserAgent;

my $ua = Mojo::UserAgent->new;
$ua->websocket('ws://localhost:5100/quote' => sub {
  my ($ua, $tx) = @_;
  print "WebSocket handshake failed!\n" and return unless $tx->is_websocket;

  $tx->on(finish => sub {
    my ($tx, $code, $reason) = @_;
    print "WebSocket closed with status $code.\n";
  });

  $tx->on(message => sub {
    my ($tx, $msg) = @_;
    print "WebSocket message: $msg\n";
    $tx->finish;
  });

  $tx->send('Hi!');
});
Mojo::IOLoop->start unless Mojo::IOLoop->is_running;

