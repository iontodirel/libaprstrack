#include <gtest/gtest.h>
#include <etl/string.h>

#include "../aprstrack.hpp"

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

    etl::string<100> packet = encode_mic_e_packet_no_message_to<etl::string<100>>(t, d);
    EXPECT_TRUE(packet == "N0CALL>T9QPVP,WIDE1-1:`3T{m\\\x1f[/\"4F}");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}