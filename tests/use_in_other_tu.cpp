#include "use_in_tu.h"

#include <cassert>

using namespace aprs::track;
using namespace aprs::track::detail;

void func()
{
    tracker t;
    t.from("N0CALL");
    t.to("APRS");
    t.mic_e_status(mic_e_status::en_route);
    t.path("WIDE1-1");
    t.symbol_table('/');
    t.symbol_code('>');

    struct s
    {
        double lat = 0.0;
        double lon = 0.0;
        double speed = 0.0;
        double track = 0.0;
        double alt = 0.0;
    };

    s data;

    data.lat = 51.6145;
    data.lon = -0.0485;
    data.speed = 3.601;
    data.track = 297.0;
    data.alt = 33.00;

    t.position(data);

    std::string packet = t.packet_string_no_message(packet_type::mic_e);

    assert(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");

    struct data data_internal;
    data_internal.lat = 39.751166666667;
    data_internal.lon = -75.085333333333;
    data_internal.track_degrees = 335;
    data_internal.speed_knots = 8;
    data_internal.day = 18;
    data_internal.hour = 16;
    data_internal.minute = 13;

    packet = encode_position_packet_with_utc_timestamp_dhm_no_message(t, data_internal);

    assert(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W>335/008");
}