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

sub determine_packet_type {
    my ($packetdata) = @_;
    my $packet_type = 'position'; # Default
    
    if ($packetdata->{'format'} eq 'compressed' && $packetdata->{'type'} eq 'location') {
        $packet_type = 'position_compressed';
    } elsif ($packetdata->{'format'} eq 'mice' && $packetdata->{'type'} eq 'location') {
        $packet_type = 'mic_e';
    } elsif ($packetdata->{'format'} eq 'uncompressed' && $packetdata->{'type'} eq 'location') {
        if ($packetdata->{'timestamp'}) {
            my $ts_type = $packetdata->{'timestamp_type'} || '';
            if ($ts_type eq 'DHM') {
                $packet_type = 'position_with_timestamp';
            } elsif ($ts_type eq 'HMS') {
                $packet_type = 'position_with_timestamp_utc_mhs';  
            } elsif ($ts_type eq 'zDHM') {
                $packet_type = 'position_with_timestamp_utc';
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

sub format_custom_data {
    my ($packetdata, $digipeaters_string, $mic_e_status, $packet_type) = @_;
    
    #my $location_info = get_location_info(
    #    $packetdata->{'latitude'}, 
    #    $packetdata->{'longitude'}
    #);

    tie my %custom_data, 'Tie::IxHash';
    %custom_data = (
        packet => $packetdata->{'origpacket'} // '',
        packet_base64 => encode_base64($packetdata->{'origpacket'} // '', ''),
        data => $packetdata->{'body'} // '',
        data_base64 => encode_base64($packetdata->{'body'} // '', ''),
        from => $packetdata->{'srccallsign'} // '',
        to => $packetdata->{'dstcallsign'} // '',
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
        comment_base64 => encode_base64($packetdata->{'comment'} // '', '')
    );

    #if ($location_info) {
    #    $custom_data{country} = $location_info->{country};
    #    $custom_data{city} = $location_info->{city};
    #    $custom_data{county} = $location_info->{county};
    #    $custom_data{state} = $location_info->{state};
    #    $custom_data{postcode} = $location_info->{postcode};
    #    $custom_data{display_name} = $location_info->{display_name};
    #}
    
    return \%custom_data;
}

sub parse_and_format_packet {
    my ($packet) = @_;
    my %packetdata;
    
    my $retval = parseaprs($packet, \%packetdata);
    if ($retval != 1) {
        warn "Parsing failed: $packetdata{resultmsg} ($packetdata{resultcode}) for packet: $packet\n";
        return;
    }

    my $packet_type = determine_packet_type(\%packetdata);
    my $digipeaters_string = format_digipeater_path(\%packetdata);
    my $mic_e_status = ($packet_type eq 'mic_e' && exists $packetdata{'mbits'}) ? get_mic_e_status($packetdata{'mbits'}) : '';

    my $formatted_data = format_custom_data(\%packetdata, $digipeaters_string, $mic_e_status, $packet_type);
    
    return $formatted_data;  # Return the Perl data structure, not JSON string
}

sub read_packets_from_file {
    my ($filename) = @_;
    return read_file($filename, chomp => 1);
}

sub write_json_output {
    my ($output_file, $json_data) = @_;
    write_file($output_file, {append => 1}, $json_data . "\n");
}

sub process_packets_from_file {
    my ($input_file, $output_file) = @_;
    my @packets = read_packets_from_file($input_file);
    my @parsed_packets;

    foreach my $packet (@packets) {
        next if $packet =~ /^\s*$/;  # Skip empty lines
        my $parsed_data = parse_and_format_packet($packet);
        push @parsed_packets, $parsed_data if $parsed_data;
    }

    # Convert the array of data structures to a single JSON string
    my $json = JSON::PP->new->utf8->pretty->encode(\@parsed_packets);
    
    # Write the JSON to the output file
    write_file($output_file, $json);
}

sub get_location_info {
    my ($lat, $lon) = @_;
    return undef unless (defined $lat && defined $lon && $lat ne '' && $lon ne '');

    my $ua = LWP::UserAgent->new;
    $ua->timeout(10);  # Set timeout to 10 seconds
    
    # Using OpenStreetMap Nominatim API (free and doesn't require API key)
    # Be sure to respect their usage policy: https://operations.osmfoundation.org/policies/nominatim/
    my $url = "https://nominatim.openstreetmap.org/reverse?format=json&lat=$lat&lon=$lon&zoom=18&addressdetails=1";
    
    my $request = HTTP::Request->new(GET => $url);
    $request->header('Accept' => 'application/json');
    $request->header('User-Agent' => 'APRSParserScript/1.0');  # Important: Nominatim requires a User-Agent
    
    my $location_info = {};
    
    try {
        my $response = $ua->request($request);
        
        if ($response->is_success) {
            my $data = JSON::PP->new->utf8->decode($response->decoded_content);
            
            # Extract relevant location information
            $location_info = {
                country => $data->{address}->{country} || '',
                city => $data->{address}->{city} || $data->{address}->{town} || $data->{address}->{village} || '',
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

# --- Run the parser ---
my $input_file = 'aprsis_seattle_1/mice.txt';   # Input file containing APRS packets (one per line)
my $output_file = 'output.json';  # Output JSON file

process_packets_from_file($input_file, $output_file);

print "Processing complete! JSON saved to $output_file\n";