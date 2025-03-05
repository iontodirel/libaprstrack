#!/usr/bin/perl
use strict;
use warnings;
use Ham::APRS::FAP qw(parseaprs make_position);

# Just process a single packet
my $packet = 'N0CALL>APRS,WIDE1-1:!2677.84S/04900.03W#';

# Parse it
my %packetdata;
my $retval = parseaprs($packet, \%packetdata);

# Simple output to stdout
if ($retval == 1) {
    # Print raw data structure
    foreach my $key (sort keys %packetdata) {
        # Skip internal/complex data structures
        next if ref($packetdata{$key}) eq 'ARRAY' || ref($packetdata{$key}) eq 'HASH';
        print "$key: $packetdata{$key}\n";
    }
} else {
    print "Parsing failed: $packetdata{resultmsg} ($packetdata{resultcode})\n";
}
