#!/usr/bin/perl
use strict;
use warnings;
use Ham::APRS::FAP qw(parseaprs);
use JSON::PP;
use File::Slurp;
use Tie::IxHash;
use MIME::Base64;
use LWP::UserAgent;
use HTTP::Request;
use Try::Tiny;

# Constants
my $INPUT_FILE = '../assets/mic_e_packets.txt';
my $OUTPUT_FILE = '../assets/mic_e_packets.json';
my $API_TIMEOUT = 10; # seconds
my $USER_AGENT = 'APRSParserScript/1.0';

process_packets_from_file($INPUT_FILE, $OUTPUT_FILE);
print "Processing complete! JSON saved to $OUTPUT_FILE\n";

sub process_packets_from_file {
    my ($input_file, $output_file) = @_;

    my @packets = read_packets_from_file($input_file);
    my @parsed_packets;
    my $packet_index = 0;

    foreach my $packet (@packets) {
        next if $packet =~ /^\s*$/;  # Skip empty lines
        my $parsed_data = parse_and_format_packet($packet, $packet_index);
        if ($parsed_data) {
            push @parsed_packets, $parsed_data;
            $packet_index++;  # Increment only for valid packets
        }
    }

    # Convert the array of data structures to a single JSON string
    my $json = JSON::PP->new->utf8->pretty->encode(\@parsed_packets);

    # Write the JSON to the output file
    write_file($output_file, $json);
}

sub read_packets_from_file {
    my ($filename) = @_;
    return read_file($filename, chomp => 1);
}

sub parse_and_format_packet {
    my ($packet, $index) = @_;
    my %packetdata;

    my $retval = parseaprs($packet, \%packetdata);
    if ($retval != 1) {
        warn "Parsing failed: $packetdata{resultmsg} ($packetdata{resultcode}) for packet: $packet\n";
        return;
    }

    my $packet_type = determine_packet_type(\%packetdata);
    if ($packet_type eq 'unknown') {
        warn "Unknown packet type for packet: $packet\n";
        return;
    }

    my $digipeaters_string = format_digipeater_path(\%packetdata);
    my $mic_e_status = ($packet_type eq 'mic_e' && exists $packetdata{'mbits'})
                      ? get_mic_e_status($packetdata{'mbits'})
                      : '';

    return format_custom_data(
        \%packetdata,
        $digipeaters_string,
        $mic_e_status,
        $packet_type,
        $index
    );
}

sub format_custom_data {
    my ($packetdata, $digipeaters_string, $mic_e_status, $packet_type, $index) = @_;

    my $from_ssid = extract_ssid($packetdata->{'srccallsign'});
    my $to_ssid = extract_ssid($packetdata->{'dstcallsign'});

    tie my %custom_data, 'Tie::IxHash';
    %custom_data = (
        index => "" . $index,
        packet => $packetdata->{'origpacket'} // '',
        packet_base64 => encode_base64($packetdata->{'origpacket'} // '', ''),
        data => $packetdata->{'body'} // '',
        data_base64 => encode_base64($packetdata->{'body'} // '', ''),
        from => $packetdata->{'srccallsign'} // '',
        from_ssid => $from_ssid,
        to => $packetdata->{'dstcallsign'} // '',
        to_ssid => $to_ssid,
        path => $digipeaters_string,
        lat => "" . ($packetdata->{'latitude'} // ''),
        lon => "" . ($packetdata->{'longitude'} // ''),
        messaging => $packetdata->{'messaging'} ? 'true' : 'false',
        symbol_code => $packetdata->{'symbolcode'} // '',
        symbol_table => $packetdata->{'symboltable'} // '',
        alt => "" . ($packetdata->{'altitude'} // ''),
        course => "" . (exists $packetdata->{'course'} ? $packetdata->{'course'} : ''),
        speed => "" . (exists $packetdata->{'speed'} ? $packetdata->{'speed'} : ''),
        mic_e_status => $mic_e_status,
        packet_type => $packet_type,
        comment => $packetdata->{'comment'} // '',
        comment_base64 => encode_base64($packetdata->{'comment'} // '', ''),
        ambiguity => "" . ($packetdata->{'posambiguity'} // ''),
    );

    return \%custom_data;
}

sub get_location_info {
    my ($lat, $lon) = @_;
    return undef unless (defined $lat && defined $lon && $lat ne '' && $lon ne '');

    my $ua = LWP::UserAgent->new;
    $ua->timeout($API_TIMEOUT);

    # Using OpenStreetMap Nominatim API
    my $url = "https://nominatim.openstreetmap.org/reverse?format=json&lat=$lat&lon=$lon&zoom=18&addressdetails=1";

    my $request = HTTP::Request->new(GET => $url);
    $request->header('Accept' => 'application/json');
    $request->header('User-Agent' => $USER_AGENT);

    my $location_info = {};
    
    try {
        my $response = $ua->request($request);
        
        if ($response->is_success) {
            my $data = JSON::PP->new->utf8->decode($response->decoded_content);
            
            # Extract relevant location information
            $location_info = {
                country => $data->{address}->{country} || '',
                city => $data->{address}->{city} ||
                        $data->{address}->{town} ||
                        $data->{address}->{village} || '',
                county => $data->{address}->{county} || '',
                state => $data->{address}->{state} || '',
                postcode => $data->{address}->{postcode} || '',
                display_name => $data->{display_name} || ''
            };
        } else {
            warn "Failed to get location information: " . $response->status_line . "\n";
        }
    } catch {
        warn "Error getting location information: $_\n";
    };

    return $location_info;
}

sub determine_packet_type {
    my ($packetdata) = @_;
    my $packet_type = 'unknown'; # Default

    if (defined $packetdata->{'format'} && defined $packetdata->{'type'}) {
        if ($packetdata->{'format'} eq 'mice' && $packetdata->{'type'} eq 'location') {
            $packet_type = 'mic_e';
        }
        elsif ($packetdata->{'format'} eq 'compressed' && $packetdata->{'type'} eq 'location') {
            $packet_type = 'position_compressed';
        }
        elsif ($packetdata->{'format'} eq 'uncompressed' && $packetdata->{'type'} eq 'location') {
            $packet_type = 'position';
        }
    }

    # if after :!, :/, :@, := contents starts with:
    #   ddddddz - position_with_timestamp_utc
    #   dddddd/ - position_with_timestamp
    #   ddddddh - position_with_timestamp_utc_mhs

    if ($packet_type =~ /^position(_compressed)?$/) {
        if (defined $packetdata->{'body'}) {
            if ($packetdata->{'body'} =~ /^[!\/@=]\d{6}z/) {
                $packet_type .= '_with_timestamp_utc';
            }
            elsif ($packetdata->{'body'} =~ /^[!\/@=]\d{6}\//) {
                $packet_type .= '_with_timestamp';
            }
            elsif ($packetdata->{'body'} =~ /^[!\/@=]\d{6}h/) {
                $packet_type .= '_with_timestamp_utc_hms';
            }
        }
    }


    return $packet_type;
}

sub format_digipeater_path {
    my ($packetdata) = @_;
    my @digipeater_entries;

    if (ref $packetdata->{'digipeaters'} eq 'ARRAY') {
        foreach my $digipeater (@{$packetdata->{'digipeaters'}}) {
            if (ref $digipeater eq 'HASH' && exists $digipeater->{'call'}) {
                my $entry = $digipeater->{'call'};
                $entry .= '*' if $digipeater->{'wasdigied'};
                push @digipeater_entries, $entry;
            }
        }
    }

    return join(',', @digipeater_entries);
}

sub get_mic_e_status {
    my $mbits = shift;
    return 'unknown' unless defined $mbits;

    # Extract the 3 bits (order is a,b,c)
    my $a = ($mbits >> 2) & 0x1;
    my $b = ($mbits >> 1) & 0x1;
    my $c = $mbits & 0x1;

    # Match the bit pattern to status strings
    my $pattern = ($a << 2) | ($b << 1) | $c;

    my %status_map = (
        0b111 => 'off_duty',   # 1 1 1
        0b110 => 'en_route',   # 1 1 0
        0b101 => 'in_service', # 1 0 1
        0b100 => 'returning',  # 1 0 0
        0b011 => 'commited',   # 0 1 1
        0b010 => 'special',    # 0 1 0
        0b001 => 'priority',   # 0 0 1
        0b000 => 'emergency'   # 0 0 0
    );

    return $status_map{$pattern} // 'unknown';
}

sub extract_ssid {
    my ($callsign) = @_;
    if ($callsign =~ /-(\d+)$/) {
        return $1;
    }
    return '';
}