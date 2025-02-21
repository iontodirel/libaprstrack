# libaprstrack

C++ header only standalone APRS encoding library for APRS tracking.

This libary is designed for implementing APRS trackers, and contains an implementation of the Smart Beaconing (TM) algorithm.

Automatic Packet Reporting System (APRS) (TM) is a system for real-time digital data communication over ham radio.

See more info at http://www.aprs.org/ and https://groups.io/g/APRS.

SmartBeaconing (TM) is a beaconing algorithm invented by Tony Arnerich KD7TA and Steve Bragg KA9MVA.

APRS is a registered trademark of Bob Bruninga, WB4APR.

## Examples

### Updating the position and encoding a packet

``` cpp
tracker t;
t.from("N0CALL");
t.mic_e_status(mic_e_status::en_route);
t.path("WIDE1-1");
t.symbol_table('/');
t.symbol_code('>');

struct s
{
    double lat   = 0.0;
    double lon   = 0.0;
    double speed = 0.0;
    double track = 0.0;
    double alt   = 0.0;
};

s data { 51.6145, -0.0485, 3.601, 297.0, 33.00 };

t.position(data);

std::string packet = t.packet_string(packet_type::mic_e);

assert(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");
```

### Specifying other packet types

Position packet with UTC DMS timestamp:

``` cpp
std::string packet = t.packet_string(packet_type::position_with_timestamp_utc);
assert(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_");
```

Compressed position packet:

``` cpp
std::string packet = t.packet_string(packet_type::position_with_timestamp_utc);
assert(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_");
```

### Smart beaconing

An external GNSS client is used in this example to supply position inforomation to the tracker library, there is no coupling between the *gpsd_client* and the library.

``` cpp
// using an GNSS client to supply the position info to the library
gpsd_client gpsd("192.168.1.11", 8888);
gpsd.open();

aprs::track::tracker t;

t.message("hello, world!");
t.algorithm(aprs::track::algorithm::smart_beaconing);
t.message("Hello, world!");
t.symbol_code('I');
t.symbol_table('/');
t.from("N0CALL");
t.to("APRS");
t.path("WIDE1-1,WIDE2-2");
t.interval(std::chrono::seconds(5));
t.ambiguity(0);
t.mic_e_status(aprs::track::mic_e_status::en_route);
t.messaging(false);

t.low_speed(5);
t.high_speed(60);
t.slow_rate(60);
t.fast_rate(10);
t.turn_angle(28);
t.turn_time(30);
t.turn_slope(26);

while (true)
{
    // using an GNSS client to supply the position info to the library
    gnss_info info;
    gpsd.try_get_gps_info(info, gnss_include_info::all, 1000);

    t.position(info);
    t.update();

    if (t.updated())
    {
        std::string packet = t.packet_string(aprs::track::packet_type::mic_e);

        fmt::println("{}", packet);
    }
}
```

### Encoding a mic-e packet:

Lower level functions in the library can also be used for convenience or utility:

``` cpp
std::string packet = aprs::track::detail::encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 35.449666666667, 140.2685, mic_e_status::custom6, 257, 5.999, '/', '>', 0, 9.84);
assert(packet == "N0CALL>35CVY8,WIDE1-1:`D,'l^U>/\"3u}");
```

## Testing

The ***tests*** directory contain the tests. `tests/tests.cpp` contains a comprehensive sets of tests, written using the Google Test framework.

Use the tests as examples of how to use the library. The tests also provide a comprehensive insight into how the APRS protocol works, and will help other implementers test or implement APRS solutions.

The ***assets*** directory contains route information generated from OSM and Valhalla, and is used to simulate and test tracking.

# License

MIT License, Copyright (C) 2025 Ion Todirel

Test data and assets used by tests was obtained from APRS-IS. I do not hold any copyright or license claim over this data, the data is in the public domain.
