#include <gtest/gtest.h>
#include <etl/string.h>
#include <cassert>

namespace aprs::track
{
    // Define custom types for APRS tracking
    using string_t = etl::string<100>;  // Example of a custom string type with a maximum length of 100 characters
}

#define APRS_TRACK_DEFINE_CUSTOM_TYPES
#include "../libaprstrack.hpp"

using namespace aprs::track;
using namespace aprs::track::detail;

TEST(tracker, encode_mic_e_packet_no_message_etl_string)
{
    tracker t;
    t.from("N0CALL");
    t.path("WIDE1-1");
    t.mic_e_status(mic_e_status::in_service);
    t.symbol_table('/');
    t.symbol_code('[');

    data d;
    d.lat = 49.176666666667;
    d.lon = -123.94916666667;
    d.track_degrees = 3;
    d.speed_knots = 15.999;
    d.alt_feet = 154.2;

    string_t packet = encode_mic_e_packet_no_message(t, d);
    EXPECT_TRUE(packet == "N0CALL>T9QPVP,WIDE1-1:`3T{m\\\x1f[/\"4F}");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}