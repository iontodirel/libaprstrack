// **************************************************************** //
// libaprstrack - APRS tracking library                             // 
// Version 0.1.0                                                    //
// https://github.com/iontodirel/libaprstrack                       //
// Copyright (c) 2025 Ion Todirel                                   //
// **************************************************************** //
//
// libaprstrack.hpp
// 
// MIT License
// 
// Copyright (c) 2025 Ion Todirel
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// **************************************************************** //
// APRS-IS test data                                                //
// **************************************************************** //
// 
// Some of the packets and data used in these tests have been taken from APRS-IS.
// I do not not hold any copyright to them, the test data is in the public domain.

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <locale>

#include <fmt/format.h>
#include <fmt/color.h>

#include "../libaprstrack.hpp"

using namespace aprs::track;
using namespace aprs::track::detail;

TEST(data_ext, encode_altitude)
{
    EXPECT_TRUE(encode_altitude(12) == "/A=000012");
    EXPECT_TRUE(encode_altitude(123) == "/A=000123");
    EXPECT_TRUE(encode_altitude(1234) == "/A=001234");
    EXPECT_TRUE(encode_altitude(12345) == "/A=012345");
}

TEST(data_ext, encode_speed_course)
{ 
    EXPECT_TRUE(encode_course_speed(88, 36) == "088/036");
}

TEST(packet_type, packet_type_1_2)
{
    EXPECT_TRUE(packet_type_1(true) == '=');
    EXPECT_TRUE(packet_type_1(false) == '!');
    EXPECT_TRUE(packet_type_2(true) == '@');
    EXPECT_TRUE(packet_type_2(false) == '/');
}

TEST(packet, encode_header)
{
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "WIDE1-1,WIDE2-2") == "N0CALL>APRS,WIDE1-1,WIDE2-2:");
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "WIDE1-1") == "N0CALL>APRS,WIDE1-1:");
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "") == "N0CALL>APRS:");
}

TEST(position, dd_to_dms)
{
    {
        auto [d, m, s] = dd_to_dms(37.7749);
        EXPECT_TRUE(d == 37);
        EXPECT_TRUE(m == 46);
        EXPECT_NEAR(s, 29.64, 0.01);
    }
}

TEST(position, dd_to_ddm)
{
    {
        auto [dd, m] = dd_to_ddm(37.7749);
        EXPECT_TRUE(dd == 37);
        EXPECT_NEAR(m, 46.494, 0.001);
    }

    {
        auto ddm = dd_to_ddm(37.7749, -122.4194);
        EXPECT_TRUE(ddm.lat == 'N');
        EXPECT_TRUE(ddm.lat_d == 37);
        EXPECT_NEAR(ddm.lat_m, 46.494, 0.001);
        EXPECT_TRUE(ddm.lon == 'W');
        EXPECT_TRUE(ddm.lon_d == 122);
        EXPECT_NEAR(ddm.lon_m, 25.164, 0.001);
    }
}

TEST(position, to_ddm_short_string)
{
    {
        auto ddm = dd_to_ddm(37.7749, -122.4194);
        auto ddm_str = to_ddm_short_string(ddm, 0);
        EXPECT_TRUE(ddm_str.lat == "3746.49N");
        EXPECT_TRUE(ddm_str.lon == "12225.16W");
    }

    {
        auto ddm = dd_to_ddm(37.7749, -122.4194);
        auto ddm_str = to_ddm_short_string(ddm, 1);
        EXPECT_TRUE(ddm_str.lat == "3746.4 N");
        EXPECT_TRUE(ddm_str.lon == "12225.16W");
    }

    {
        auto ddm = dd_to_ddm(37.7749, -122.4194);
        auto ddm_str = to_ddm_short_string(ddm, 2);
        EXPECT_TRUE(ddm_str.lat == "3746.  N");
        EXPECT_TRUE(ddm_str.lon == "12225.16W");
    }

    {
        auto ddm = dd_to_ddm(1.4648333333333, 103.73783333333);
        auto ddm_str = to_ddm_short_string(ddm, 0);
        EXPECT_TRUE(ddm_str.lat == "0127.89N");
        EXPECT_TRUE(ddm_str.lon == "10344.27E");
    }

    {
        auto ddm = dd_to_ddm(51.627333333333, 0.020333333333333);
        auto ddm_str = to_ddm_short_string(ddm, 0);
        EXPECT_TRUE(ddm_str.lat == "5137.64N");
        EXPECT_TRUE(ddm_str.lon == "00001.22E");
    }
}

TEST(format, format_number_to_string)
{
    EXPECT_TRUE(format_number_to_string(37.7749, 4) == "37.7749");
    EXPECT_TRUE(format_number_to_string(37.7749, 3) == "37.775");
    EXPECT_TRUE(format_number_to_string(37.7749, 2) == "37.77");
    EXPECT_TRUE(format_number_to_string(37.7749, 1) == "37.8");
    EXPECT_TRUE(format_number_to_string(37.7749, 0) == "37");
    EXPECT_TRUE(format_number_to_string(37.7749, 5, 2) == "037.77");
    EXPECT_TRUE(format_number_to_string(0.7749, 5, 2) == "000.77");
    EXPECT_TRUE(format_number_to_string(0.07749, 5, 2) == "000.08");
    EXPECT_TRUE(format_number_to_string(37, 0) == "37");
    EXPECT_TRUE(format_number_to_string(37, 3, 0) == "037");
}

TEST(format, format_n_digits_string)
{
    EXPECT_TRUE(format_two_digits_string(0) == "00");
    EXPECT_TRUE(format_two_digits_string(1) == "01");
    EXPECT_TRUE(format_two_digits_string(9) == "09");
    EXPECT_TRUE(format_two_digits_string(10) == "10");
    EXPECT_TRUE(format_two_digits_string(99) == "99");

    EXPECT_TRUE(format_n_digits_string(0, 2) == "00");
    EXPECT_TRUE(format_n_digits_string(1, 2) == "01");
    EXPECT_TRUE(format_n_digits_string(9, 2) == "09");
    EXPECT_TRUE(format_n_digits_string(10, 2) == "10");
    EXPECT_TRUE(format_n_digits_string(99, 2) == "99");
    EXPECT_TRUE(format_n_digits_string(12, 6) == "000012");
}

TEST(conversion, unit_conversions)
{
    EXPECT_NEAR(meters_to_feet(1), 3.28084, 0.00001);
    EXPECT_NEAR(meters_s_to_knots(1), 1.94384, 0.00001);
}

TEST(time, encode_timestamp_dhm)
{
    EXPECT_TRUE(encode_timestamp_dhm(1, 2, 3) == "010203/");
    EXPECT_TRUE(encode_timestamp_dhm(10, 20, 30) == "102030/");
}

TEST(time, encode_utc_timestamp_dhm)
{
    EXPECT_TRUE(encode_utc_timestamp_dhm(1, 2, 3) == "010203z");
    EXPECT_TRUE(encode_utc_timestamp_dhm(10, 20, 30) == "102030z");
}

TEST(time, encode_utc_timestamp_hms)
{
    EXPECT_TRUE(encode_utc_timestamp_hms(1, 2, 3) == "010203h");
    EXPECT_TRUE(encode_utc_timestamp_hms(10, 20, 30) == "102030h");
}

TEST(position, encode_compressed_lat_lon)
{
    {
        std::string lat_lon_str = encode_compressed_lat_lon(36.354730315074, -119.3034815161);
        EXPECT_TRUE(lat_lon_str == "<+_*0@\"k");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(48.374306295711, 14.424449893155);
        EXPECT_TRUE(lat_lon_str == "6$h/R-fC");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(42.543281372235, 1.7318376797593);
        EXPECT_TRUE(lat_lon_str == "8{!lNuld");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(-23.483196211338, 150.32534403007);
        EXPECT_TRUE(lat_lon_str == "ZB5]tMLw");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(42.116505568011, -71.687041577629);
        EXPECT_TRUE(lat_lon_str == "93[=<C3#");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(2.4028105196285, 102.19811196925);
        EXPECT_TRUE(lat_lon_str == "M:Kmh>S\\");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(-22.890086263474, -47.049936208082);
        EXPECT_TRUE(lat_lon_str == "Z&vsBWnM");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(51.048599990549, -0.0778523912781);
        EXPECT_TRUE(lat_lon_str == "4_fTNL4&");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(51.549272037089, 0.064085937951205);
        EXPECT_TRUE(lat_lon_str == "4HceNOL-");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(1.4840021421483, 103.93799320603);
        EXPECT_TRUE(lat_lon_str == "MdcyhfU5");
    }

    {
        std::string lat_lon_str = encode_compressed_lat_lon(-29.990935247266, -71.327365420055);
        EXPECT_TRUE(lat_lon_str == "]\\V&<KKl");
    }
}

TEST(position, encode_compressed_course_speed)
{
    {
        std::string course_speed_str = encode_compressed_course_speed(88, 36.2);
        EXPECT_TRUE(course_speed_str == "7P");
    }

    {
        std::string course_speed_str = encode_compressed_course_speed(360, 0.0);
        EXPECT_TRUE(course_speed_str == "!!");
    }

    {
        std::string course_speed_str = encode_compressed_course_speed(244, 19.12);
        EXPECT_TRUE(course_speed_str == "^H");
    }

    {
        std::string course_speed_str = encode_compressed_course_speed(232, 2.17);
        EXPECT_TRUE(course_speed_str == "[0");
    }

    {
        std::string course_speed_str = encode_compressed_course_speed(24, 0);
        EXPECT_TRUE(course_speed_str == "'!");
    }

    {
        std::string course_speed_str = encode_compressed_course_speed(72, 42.43);
        EXPECT_TRUE(course_speed_str == "3R");
    }
}

TEST(position, encode_compressed_altitude)
{
    {
        std::string alt_str = encode_compressed_altitude(10004);
        EXPECT_TRUE(alt_str == "S]");
    }

    {
        std::string alt_str = encode_compressed_altitude(277.65);
        EXPECT_TRUE(alt_str == "?w");
    }

    {
        std::string alt_str = encode_compressed_altitude(427.50);
        EXPECT_TRUE(alt_str == "B>");
    }
}

TEST(mic_e, encode_mic_e_status)
{
    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::off_duty);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "PQY");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::en_route);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "PQ9");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::in_service);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "P1Y");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::returning);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "P19");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::commited);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "0QY");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::special);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "0Q9");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::priority);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "01Y");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::emergency);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == false);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "019");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom0);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "ABJ");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom1);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "AB9");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom2);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "A1J");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom3);
        EXPECT_TRUE(a == 1);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "A19");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom4);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "0BJ");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom5);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 1);
        EXPECT_TRUE(c == 0);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "0B9");
    }

    {
        auto [a, b, c, custom] = encode_mic_e_status(mic_e_status::custom6);
        EXPECT_TRUE(a == 0);
        EXPECT_TRUE(b == 0);
        EXPECT_TRUE(c == 1);
        EXPECT_TRUE(custom == true);
        std::string s = "019";
        encode_mic_e_status(a, b, c, custom, s);
        EXPECT_TRUE(s == "01J");
    }

    {
        std::string s = "012";
        encode_mic_e_status(0, 0, 0, false, s);
        EXPECT_TRUE(s == "012");
    }

    {
        std::string s = "332";
        encode_mic_e_status(1, 0, 0, false, s);
        EXPECT_TRUE(s == "S32");
    }

    {
        std::string s = "523";
        encode_mic_e_status(1, 0, 1, true, s);
        EXPECT_TRUE(s == "F2D");
    }

    {
        std::string s = "234";
        encode_mic_e_status(0, 0, 0, false, s);
        EXPECT_TRUE(s == "234");
    }

    {
        std::string s = "   ";
        encode_mic_e_status(0, 1, 0, false, s);
        EXPECT_TRUE(s == "LZL");
    }

    {
        std::string s = "   ";
        encode_mic_e_status(0, 1, 0, true, s);
        EXPECT_TRUE(s == "LKL");
    }

    {
        std::string s = "480";
        encode_mic_e_status(1, 1, 0, false, s);
        EXPECT_TRUE(s == "TX0");
    }

    {
        std::string s = "490";
        encode_mic_e_status(1, 1, 1, false, s);
        EXPECT_TRUE(s == "TYP");
    }
}

TEST(mic_e, encode_mic_e_lat_direction)
{
    {
        std::string destination_address = "0000";
        encode_mic_e_lat_direction('N', destination_address);
        EXPECT_TRUE(destination_address == "000P");
    }

    {
        std::string destination_address = "9999";
        encode_mic_e_lat_direction('N', destination_address);
        EXPECT_TRUE(destination_address == "999Y");
    }

    {
        std::string destination_address = "    ";
        encode_mic_e_lat_direction('N', destination_address);
        EXPECT_TRUE(destination_address == "   Z");
    }

    {
        std::string destination_address = "0000";
        encode_mic_e_lat_direction('S', destination_address);
        EXPECT_TRUE(destination_address == "0000");
    }

    {
        std::string destination_address = "    ";
        encode_mic_e_lat_direction('S', destination_address);
        EXPECT_TRUE(destination_address == "   L");
    }
}

TEST(mic_e, encode_mic_e_lon)
{
    {
        std::string lon_str = encode_mic_e_lon(-123.93883);
        EXPECT_TRUE(lon_str == "3T=");
    }

    {
        std::string lon_str = encode_mic_e_lon(-123.9495);
        EXPECT_TRUE(lon_str == "3T}");
    }

    {
        std::string lon_str = encode_mic_e_lon(-124);
        EXPECT_TRUE(lon_str == "4X\x1c");
    }

    {
        std::string lon_str = encode_mic_e_lon(-82.010333333333);
        EXPECT_TRUE(lon_str == "nXZ");
    }

    {
        std::string lon_str = encode_mic_e_lon(-82.019666666667);
        EXPECT_TRUE(lon_str == "nY.");
    }

    {
        std::string lon_str = encode_mic_e_lon(175.06591391941);
        EXPECT_TRUE(lon_str == "g[{");
    }

    {
        std::string lon_str = encode_mic_e_lon(139.74666666667);
        EXPECT_TRUE(lon_str == "CHl");
    }

    {
        std::string lon_str = encode_mic_e_lon(103.79366666667);
        EXPECT_TRUE(lon_str == "oKZ");
    }

    {
        std::string lon_str = encode_mic_e_lon(-1.0523333333333);
        EXPECT_TRUE(lon_str == "w[*");
    }

    {
        std::string lon_str = encode_mic_e_lon(11.341333333333);
        EXPECT_TRUE(lon_str == "'0L");
    }

    {
        std::string lon_str = encode_mic_e_lon(-112.129);
        EXPECT_TRUE(lon_str == "(_f");
    }

    {
        // lon direction is encoded in the destination address
        std::string lon_str = encode_mic_e_lon(112.129);
        EXPECT_TRUE(lon_str == "(_f");
    }

    {
        std::string lon_str = encode_mic_e_lon(0.0);
        EXPECT_TRUE(lon_str == "vX\x1c");
    }

    {
        std::string lon_str = encode_mic_e_lon(36.3255);
        EXPECT_TRUE(lon_str == "@/Q");
    }

    {
        std::string lon_str = encode_mic_e_lon(9.1278333333333);
        EXPECT_TRUE(lon_str == "\x7f__");
    }

    {
        std::string lon_str = encode_mic_e_lon(5.2283333333333);
        EXPECT_TRUE(lon_str == "{)b");
    }

    {
        std::string lon_str = encode_mic_e_lon(100.47716666667);
        EXPECT_TRUE(lon_str == "l8[");
    }

    {
        std::string lon_str = encode_mic_e_lon(27.5135);
        EXPECT_TRUE(lon_str == "7:m");
    }

    {
        std::string lon_str = encode_mic_e_lon(-93.4685);
        EXPECT_TRUE(lon_str == "y8'");
    }

    {
        std::string lon_str = encode_mic_e_lon(-6.8898333333333);
        EXPECT_TRUE(lon_str == "|QC");
    }

    {
        std::string lon_str = encode_mic_e_lon(133.77083333333);
        EXPECT_TRUE(lon_str == "=J5");
    }

    {
        std::string lon_str = encode_mic_e_lon(16.511833333333);
        EXPECT_TRUE(lon_str == ",:c");
    }

    {
        std::string lon_str = encode_mic_e_lon(9.8053333333333);
        EXPECT_TRUE(lon_str == "\x7fL<");
    }

    {
        std::string lon_str = encode_mic_e_lon(0.5475);
        EXPECT_TRUE(lon_str == "v<q");
    }

    {
        std::string lon_str = encode_mic_e_lon(0.063666666666667);
        EXPECT_TRUE(lon_str == "v[n");
    }

    {
        std::string lon_str = encode_mic_e_lon(-0.020833333333333);
        EXPECT_TRUE(lon_str == "vY5");
    }

    {
        std::string lon_str = encode_mic_e_lon(-117.28883333333);
        EXPECT_TRUE(lon_str == "--=");
    }
}

TEST(mic_e, encode_mic_lon_offset)
{
    {
        std::string destination_address = "00002";
        encode_mic_lon_offset(false, destination_address);
        EXPECT_TRUE(destination_address == "00002");
    }

    {
        std::string destination_address = "00002";
        encode_mic_lon_offset(true, destination_address);
        EXPECT_TRUE(destination_address == "0000R");
    }

    {
        std::string destination_address = "0000 ";
        encode_mic_lon_offset(false, destination_address);
        EXPECT_TRUE(destination_address == "0000L");
    }

    {
        std::string destination_address = "0000 ";
        encode_mic_lon_offset(true, destination_address);
        EXPECT_TRUE(destination_address == "0000Z");
    }
}

TEST(mic_e, encode_mic_lon_direction)
{
    {
        std::string destination_address = "000002";
        encode_mic_lon_direction('E', destination_address);
        EXPECT_TRUE(destination_address == "000002");
    }

    {
        std::string destination_address = "000002";
        encode_mic_lon_direction('W', destination_address);
        EXPECT_TRUE(destination_address == "00000R");
    }
    {
        std::string destination_address = "00000 ";
        encode_mic_lon_direction('E', destination_address);
        EXPECT_TRUE(destination_address == "00000L");
    }
    {
        std::string destination_address = "00000 ";
        encode_mic_lon_direction('W', destination_address);
        EXPECT_TRUE(destination_address == "00000Z");
    }
}

TEST(mic_e, encode_mic_e_lat)
{
    {
        std::string lat_str = encode_mic_e_lat(33.4273);
        EXPECT_TRUE(lat_str == "332564");
    }

    {
        std::string lat_str = encode_mic_e_lat(-33.4273);
        EXPECT_TRUE(lat_str == "332564");
    }

    {
        std::string lat_str = encode_mic_e_lat(45.769333333333);
        EXPECT_TRUE(lat_str == "454616");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.260666666667);
        EXPECT_TRUE(lat_str == "611564");
    }

    {
        std::string lat_str = encode_mic_e_lat(1.6801666666667);
        EXPECT_TRUE(lat_str == "014081");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.546333333333);
        EXPECT_TRUE(lat_str == "613278");
    }

    {
        std::string lat_str = encode_mic_e_lat(52.309833333333);
        EXPECT_TRUE(lat_str == "521859");
    }
}

TEST(mic_e, encode_mic_e_lat_with_lon_mic_e_status)
{
    {
        std::string lat_str = encode_mic_e_lat(45.769333333333, -120.10133333333, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "T5TVQV");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.260666666667, -149.83166666667, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "V1QUVT");
    }

    {
        std::string lat_str = encode_mic_e_lat(1.6801666666667, 103.8535, mic_e_status::en_route, 0);
        EXPECT_TRUE(lat_str == "PQ4PX1");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.546333333333, -149.5855, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "V1SRWX");
    }

    {
        std::string lat_str = encode_mic_e_lat(52.309833333333, 0.38183333333333, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "URQXU9");
    }

    {
        std::string lat_str = encode_mic_e_lat(35.157166666667, 140.226, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "SUPYT3");
    }

    {
        std::string lat_str = encode_mic_e_lat(35.575, 139.6615, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "S5STU0");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.903666666667, -6.8901666666667, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "VQUTRR");
    }

    {
        std::string lat_str = encode_mic_e_lat(-33.355, 149.49766666667, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "SSR1S0");
    }

    {
        std::string lat_str = encode_mic_e_lat(30.286833333333, -82.962666666667, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "S0QW2Q");
    }

    {
        std::string lat_str = encode_mic_e_lat(9.0218333333333, -79.527833333333, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "P9PQ3Q");
    }

    {
        std::string lat_str = encode_mic_e_lat(55.462166666667, 8.442, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "UURWW3");
    }

    {
        std::string lat_str = encode_mic_e_lat(5.688, 100.5385, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "P5TQR8");
    }

    {
        std::string lat_str = encode_mic_e_lat(39.417, 2.7461666666667, mic_e_status::special, 0);
        EXPECT_TRUE(lat_str == "3Y2UP2");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.904333333333, -6.89, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "VQUTRV");
    }

    {
        std::string lat_str = encode_mic_e_lat(61.904166666667, -6.8895, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "VQUTRU");
    }

    {
        std::string lat_str = encode_mic_e_lat(45.49, -122.4875, mic_e_status::custom6, 0);
        EXPECT_TRUE(lat_str == "45CYTP");
    }

    {
        std::string lat_str = encode_mic_e_lat(-40.879738095238, 175.06654578755, mic_e_status::en_route, 0);
        EXPECT_TRUE(lat_str == "TP52W8");
    }

    {
        std::string lat_str = encode_mic_e_lat(13.459333333333, 144.75666666667, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "Q3RWU6");
    }

    {
        std::string lat_str = encode_mic_e_lat(-41.093666666667, -71.145166666667, mic_e_status::off_duty, 0);
        EXPECT_TRUE(lat_str == "TQP56R");
    }

    {
        std::string lat_str = encode_mic_e_lat(0.0, 0.0, mic_e_status::emergency, 0);
        EXPECT_TRUE(lat_str == "000PP0");
    }

    {
        std::string lat_str = encode_mic_e_lat(55.842166666666664, 37.35366666666667, mic_e_status::en_route, 0);
        EXPECT_TRUE(lat_str == "UU5P53");
    }

    {
        std::string lat_str = encode_mic_e_lat(33.926166666667, -117.28883333333, mic_e_status::in_service, 0);
        EXPECT_TRUE(lat_str == "S3UUUW");
    }
}

TEST(mic_e, encode_mic_e_course_speed)
{
    {
        std::string course_speed = encode_mic_e_course_speed(6, 6.996);
        EXPECT_TRUE(course_speed == "lf\"");
    }

    {
        std::string course_speed = encode_mic_e_course_speed(74, 7.999);
        EXPECT_TRUE(course_speed == "lpf");
    }

    {
        std::string course_speed = encode_mic_e_course_speed(154, 11.999);
        EXPECT_TRUE(course_speed == "m5R");
    }

    {
        std::string course_speed = encode_mic_e_course_speed(193, 32.999);
        EXPECT_TRUE(course_speed == "o?y");
    }

    {
        std::string course_speed = encode_mic_e_course_speed(187, 55.999);
        EXPECT_TRUE(course_speed == "q]s");
    }

    {
        std::string course_speed = encode_mic_e_course_speed(342, 39.999);
        EXPECT_TRUE(course_speed == "p#F");
    }
}

TEST(mic_e, encode_mic_e_alt)
{
    {
        std::string alt = encode_mic_e_alt(59.06);
        EXPECT_TRUE(alt == "\"4)}");
    }

    {
        std::string alt = encode_mic_e_alt(200);
        EXPECT_TRUE(alt == "\"4T}");
    }

    {
        std::string alt = encode_mic_e_alt(308.4);
        EXPECT_TRUE(alt == "\"4u}");
    }

    {
        std::string alt = encode_mic_e_alt(400.3);
        EXPECT_TRUE(alt == "\"56}");
    }

    {
        std::string alt = encode_mic_e_alt(672.6);
        EXPECT_TRUE(alt == "\"6.}");
    }

    {
        std::string alt = encode_mic_e_alt(2136.8);
        EXPECT_TRUE(alt == "\";%}");
    }
}

TEST(mic_e, encode_mic_e_data)
{
    {
        std::string data = encode_mic_e_data('`', 0.0, -123.93883, 74, 7.999, '/', '>');
        EXPECT_TRUE(data == "`3T=lpf>/");
    }

    {
        std::string data = encode_mic_e_data('\'', 0.0, -112.129, 187, 55.999, '/', 'j');
        EXPECT_TRUE(data == "'(_fq]sj/");
    }
}

TEST(mic_e, encode_mic_e_packet_no_message)
{
    {
        // VE7JAI-7>T9QPVP,WIDE1-1,WIDE2-1,qAR,VA7PF:`3T{m\<0x1f>[/`"4F}VE7JAI_$
        //                                                         ~    ~~~~~~~~
        // NOTE: '`' inserted in the 10th position is part of the message: `VE7JAI_$
        // This what APRS spec has to say about it:
        // 
        // It is envisaged that other Mic-E-compatible devices will be allocated their
        // own type codes in future. (10th byte)
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 49.176666666667, -123.94916666667, mic_e_status::in_service, 3, 15.999, '/', '[', 0, 154.2);
        EXPECT_TRUE(packet == "N0CALL>T9QPVP,WIDE1-1:`3T{m\\\x1f[/\"4F}");
    }

    {
        // IZ2AEU-9>TUSRT8,WIDE2-2,qAR,IR2CK:'<0x7f>,Lm]m>/]"4D}
        //                                                 ~
        // NOTE: Kenwood radios insert '>' or ']' in the 10th position before the encoded altitude
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 45.541333333333, 9.2746666666667, mic_e_status::off_duty, 181, 15.999, '/', '>', 0, 147.64);
        EXPECT_TRUE(packet == "N0CALL>TUSRT8,WIDE1-1:`\x7f,Lm]m>/\"4D}");        
    }

    {
        // KD6OMV-4>SS5XWY,WIDE1-1,WIDE2-1,qAR,KELLER:`-95l"Ej/"6=}
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 33.979833333333, -117.4875, mic_e_status::en_route, 241, 0, '/', 'j', 0, 721.78);
        EXPECT_TRUE(packet == "N0CALL>SS5XWY,WIDE1-1:`-95l\"Ej/\"6=}");
    }

    {
        // KD6OMV-4>SS5XWY,WIDE1-1,WIDE2-1,qAR,KELLER:`-95l"Ej/"6=}
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 33.979833333333, -117.4875, mic_e_status::en_route, 241, 0, '/', 'j', 0);
        EXPECT_TRUE(packet == "N0CALL>SS5XWY,WIDE1-1:`-95l\"Ej/");
    }

    {
        // JH1BLT-9>35CVY8,WIDE1-1,qAR,JJ1YEG-10:`D,'l^U>/`"3u}DE JH1BLT / PUSH Rx->CS By D-STAR._)
        //                                                ~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // NOTE: '`' inserted in the 10th position is part of the message: `DE JH1BLT / PUSH Rx->CS By D-STAR._)
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 35.449666666667, 140.2685, mic_e_status::custom6, 257, 5.999, '/', '>', 0, 9.84);
        EXPECT_TRUE(packet == "N0CALL>35CVY8,WIDE1-1:`D,'l^U>/\"3u}");
    }

    {
        // JG1ZLB-12>SUTQQ6,JF2ZGN-3*,WIDE2-1,qAR,JP1BHH-10:`D[sl@s>/
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 35.686, 140.0645, mic_e_status::off_duty, 287, 2.999, '/', '>', 0);
        EXPECT_TRUE(packet == "N0CALL>SUTQQ6,WIDE1-1:`D[sl@s>/");
    }

    {
        // VE7RB-9>4Y1VPX,WIDE1-1,WIDE2-1,qAR,VA7DWC-1:`4Nil <0x1c>K\]"3r}147.240MHz T141GREEDINGS FROM VE7RB=
        //                                                           ~
        // NOTE: Kenwood radios insert '>' or ']' in the 10th position before the encoded altitude
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 49.268, -124.84616666667, mic_e_status::special, 0, 0, '\\', 'K', 0, 0);
        EXPECT_TRUE(packet == "N0CALL>4Y1VPX,WIDE1-1:`4Nil \x1cK\\\"3r}");
    }

    {
        // EA1HJA-9>SW0WXQ,EA7GXY-3,ED7YAF-3*,WIDE3-1,qAO,EA7CL-10:`{SI"p4>/]"3x}=
        //                                                                  ~
        // NOTE: Kenwood radios insert '>' or ']' in the 10th position before the encoded altitude
        // r = "
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 37.130166666667, -5.9241666666667, mic_e_status::en_route, 24, 67.98, '/', '>', 0, 19.685);
        EXPECT_TRUE(packet == "N0CALL>SW0WXQ,WIDE1-1:`{SIrp4>/\"3x}");
    }

    {
        // BX6AM-2>RSPQR2,BX6AM-5*,WIDE2-1,qAS,BV6JR-2:`0)>l#V</"4K}  144.640MHz 05.69V
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 23.020333333333, 120.22233333333, mic_e_status::off_duty, 358, 0, '/', '<', 0, 170.6);
        EXPECT_TRUE(packet == "N0CALL>RSPQR2,WIDE1-1:`0)>l#V</\"4K}");
    }

    {
        // ZL2BA-2>TP56V2,WIDE1-1,WIDE2-1,qAR,ZL2BA-10:`gD2lh!j/"54}
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", -40.943666666667, 175.67033333333, mic_e_status::en_route, 205, 7, '/', 'j', 0, 393.70);
        EXPECT_TRUE(packet == "N0CALL>TP56V2,WIDE1-1:`gD2lh!j/\"54}");
    }

    {
        // CE6PL-9>S3S49V,WIDE1-1*,WIDE2-1,qAO,CE4ERM:`bForR!>/`"9r}QAP 146.520 MHz Op. Bruno PHVF98_1
        //                                                     ~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // NOTE: '`' inserted in the 10th position is part of the message
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", -33.582666666667, -70.713833333333, mic_e_status::in_service, 5, 65, '/', '>', 0, 1791.34);
        EXPECT_TRUE(packet == "N0CALL>S3S49V,WIDE1-1:`bForR!>/\"9r}");
    }

    {
        // G3RJI-9>UQ3SX0,WIDE1-1,WIDE2-1,qAR,MB7UUE:`v+~n4$>/`"4(}145.550MHz alan mobile_%
        //                                                    ~    ~~~~~~~~~~~~~~~~~~~~~~~~
        // NOTE: '`' inserted in the 10th position is part of the message
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 51.563333333333, 0.26633333333333, mic_e_status::en_route, 8, 22, '/', '>', 0, 55.77);
        EXPECT_TRUE(packet == "N0CALL>UQ3SX0,WIDE1-1:`v+~n4$>/\"4(}");
    }

    {
        // M0MDS-9>UQ3VXW,WIDE1-1,WIDE2-1,qAR,M0BPQ-1:`vZwlh}>/`"48}_1
        //                                                     ~    ~~
        // NOTE: '`' inserted in the 10th position is part of the message
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 51.6145, -0.0485, mic_e_status::en_route, 297, 7, '/', '>', 0, 108.27);
        EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");
    }
}

TEST(mic_e, encode_mic_e_packet_no_message_with_tracker_data)
{
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

        std::string packet = encode_mic_e_packet_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>T9QPVP,WIDE1-1:`3T{m\\\x1f[/\"4F}");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.path("WIDE1-1");
        t.mic_e_status(mic_e_status::en_route);
        t.symbol_table('/');
        t.symbol_code('>');

        data d;
        d.lat = 51.6145;
        d.lon = -0.0485;
        d.track_degrees = 297;
        d.speed_knots = 7;
        d.alt_feet = 108.27;

        std::string packet = encode_mic_e_packet_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");
    }
}

TEST(position, encode_position_data_no_timestamp)
{
    {
        std::string data = encode_position_data_no_timestamp('!', 19.702333333333, -155.90966666667, 'T', '#', 0);
        EXPECT_TRUE(data == "!1942.14NT15554.58W#");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', 32.435, 105.85133333333, '/', '-', 0);
        EXPECT_TRUE(data == "!3226.10N/10551.08E-");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', -32.243333333333, 116.03, 'R', '&', 0);
        EXPECT_TRUE(data == "!3214.60SR11601.80E&");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', -31.994666666667, -64.939333333333, '/', '#', 0);
        EXPECT_TRUE(data == "!3159.68S/06456.36W#");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', 52.041666666667, 0.5, '/', 'r', 0);
        EXPECT_TRUE(data == "!5202.50N/00030.00Er");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', 51.627333333333, 0.020333333333333, '1', '#', 0);
        EXPECT_TRUE(data == "!5137.64N100001.22E#");
    }

    {
        std::string data = encode_position_data_no_timestamp('!', 1.4648333333333, 103.73783333333, '/', 'r', 0);
        EXPECT_TRUE(data == "!0127.89N/10344.27Er");
    }
}

TEST(position, encode_position_packet_no_timestamp_no_message)
{
    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", true, 1.4648333333333, 103.73783333333, '/', '>', 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:=0127.89N/10344.27E>");
    }

    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", false, 1.4648333333333, 103.73783333333, '/', 'j', 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!0127.89N/10344.27Ej");
    }

    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", false, 34.136833333333, -118.12883333333, '/', 'y', 0, 449.99);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!3408.21N/11807.73Wy/A=000450");
    }

    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", true, 47.392666666667, -122.17833333333, '/', 'O', 0, 4.00, 193, 316.00);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:=4723.56N/12210.70WO193/004/A=000316");
    }

    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", true, 47.392666666667, -122.17833333333, '/', 'O', 0, 4.00, 193);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:=4723.56N/12210.70WO193/004");
    }

    {
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", false, -7.4436666666667, 112.7195, '/', 'v', 0, 0, 312, 41);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!0726.62S/11243.17Ev312/000/A=000041");
    }
}

TEST(position, encode_position_packet_no_timestamp_no_message_with_tracker_data)
{
    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('v');

        data d;
        d.lat = -7.4436666666667;
        d.lon = 112.7195;
        d.track_degrees = 312;
        d.speed_knots = 0;
        d.alt_feet = 41;

        std::string packet = encode_position_packet_no_timestamp_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!0726.62S/11243.17Ev312/000/A=000041");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('O');
        t.messaging(true);

        data d;
        d.lat = 47.392666666667;
        d.lon = -122.17833333333;
        d.track_degrees = 193;
        d.speed_knots = 4;

        std::string packet = encode_position_packet_no_timestamp_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:=4723.56N/12210.70WO193/004");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('O');

        data d;
        d.lat = 47.392666666667;
        d.lon = -122.17833333333;

        std::string packet = encode_position_packet_no_timestamp_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!4723.56N/12210.70WO");
    }
}

TEST(position, encode_position_data_with_timestamp_dhm)
{
    {
        std::string data = encode_position_data_with_timestamp_dhm('@', 18, 21, 55, 39.890333333333, -86.205333333333, '/', '#', 0);
        EXPECT_TRUE(data == "@182155/3953.42N/08612.32W#");
    }

    {
        std::string data = encode_position_data_with_timestamp_dhm('@', 19, 02, 35, 48.764833333333, 19.277166666667, '/', '_', 0);
        EXPECT_TRUE(data == "@190235/4845.89N/01916.63E_");
    }
}

TEST(position, encode_position_packet_with_timestamp_dhm_no_message)
{
    {
        // OM7IT>APRS,TCPIP*,qAC,T2HUN:@190235/4845.89N/01916.63E_.../...t016h47b09870 Slovenska Lupca Radio73.sk 13.24V
        std::string packet = encode_position_packet_with_timestamp_dhm_no_message("N0CALL", "APRS", "WIDE1-1", true, 19, 02, 35, 48.764833333333, 19.277166666667, '/', '_', 0, 0, 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:@190235/4845.89N/01916.63E_000/000");
    }

    {
        // NI0Y-9>APX216,TCPIP*,qAO,NI0Y:@180004/3901.69N/09441.79Wj020/000/A=000000/
        std::string packet = encode_position_packet_with_timestamp_dhm_no_message("N0CALL", "APRS", "WIDE1-1", true, 18, 00, 04, 39.028166666667, -94.6965, '/', 'j', 0, 0, 20, 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:@180004/3901.69N/09441.79Wj020/000/A=000000");
    }
}

TEST(position, encode_position_packet_with_timestamp_dhm_no_message_with_tracker_data)
{
    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        data d;
        d.lat = 48.764833333333;
        d.lon = 19.277166666667;
        d.track_degrees = 0;
        d.speed_knots = 0;
        d.day = 19;
        d.hour = 02;
        d.minute = 35;

        std::string packet = encode_position_packet_with_timestamp_dhm_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:@190235/4845.89N/01916.63E_000/000");
    }
}

TEST(position, encode_position_data_with_utc_timestamp_hms)
{
    {
        std::string data = encode_position_data_with_utc_timestamp_hms('@', 12, 30, 00, 40.006333333333, -84.313166666667, '/', '-', 0);
        EXPECT_TRUE(data == "@123000h4000.38N/08418.79W-");
    }

    {
        std::string data = encode_position_data_with_utc_timestamp_hms('@', 21, 45, 07, 44.434666666667, 26.017833333333, '/', 'Z', 0);
        EXPECT_TRUE(data == "@214507h4426.08N/02601.07EZ");
    }
}

TEST(position, encode_position_packet_with_utc_timestamp_hms_no_message)
{
    {
        // N4JAH-02>APWW10,TCPIP*,qAC,T2TAS:@041501h3355.09N/08318.37W#Athens Radio Club 145.330 (-) PL123.0
        std::string packet = encode_position_packet_with_utc_timestamp_hms_no_message("N0CALL", "APRS", "WIDE1-1", true, 04, 15, 01, 33.918166666667, -83.306166666667, '/', '#', 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:@041501h3355.09N/08318.37W#");
    }
}

TEST(position, encode_position_packet_with_utc_timestamp_hms_no_message_with_tracker_data)
{
    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('`');
        t.messaging(true);

        data d;
        d.lat = 10.659666666667;
        d.lon = 123.159;
        d.hour = 23;
        d.minute = 45;
        d.second = 8;

        std::string packet = encode_position_packet_with_utc_timestamp_hms_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:@234508h1039.58N/12309.54E`");
    }
}

TEST(position, encode_position_data_with_utc_timestamp_dhm)
{
    {
        std::string data = encode_position_data_with_utc_timestamp_dhm('/', 19, 0, 2, 13.153833333333, 100.93083333333, '/', '#', 0);
        EXPECT_TRUE(data == "/190002z1309.23N/10055.85E#");
    }

    {
        std::string data = encode_position_data_with_utc_timestamp_dhm('/', 18, 16, 13, 39.751166666667, -75.085333333333, '/', '_', 0);
        EXPECT_TRUE(data == "/181613z3945.07N/07505.12W_");
    }
}

TEST(position, encode_position_packet_with_utc_timestamp_dhm_no_message)
{
    {
        // KB2M-2>APWW11,TCPIP*,qAC,T2SYDNEY:/181613z3945.07N/07505.12W_335/008g007t027r000p000P000h37b10213
        std::string packet = encode_position_packet_with_utc_timestamp_dhm_no_message("N0CALL", "APRS", "WIDE1-1", false, 18, 16, 13, 39.751166666667, -75.085333333333, '/', '_', 0, 8, 335);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_335/008");
    }
}

TEST(position, encode_position_packet_with_utc_timestamp_dhm_no_message_with_tracker_data)
{
    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        data d;
        d.lat = 39.751166666667;
        d.lon = -75.085333333333;
        d.track_degrees = 335;
        d.speed_knots = 8;
        d.day = 18;
        d.hour = 16;
        d.minute = 13;

        std::string packet = encode_position_packet_with_utc_timestamp_dhm_no_message(t, d);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_335/008");
    }
}

TEST(position, encode_position_data_compressed_no_timestamp)
{
    {
        std::string data = encode_position_data_compressed_no_timestamp('!', 50.006266308942, 20.168111391714, '/', 'u', 120, 45.90, 0b01001000);
        EXPECT_TRUE(data == "!/54agSVoou?SH");
    }
}

TEST(tracker, various_anonymous_structs)
{
    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
            double speed = 0.0;
            double track = 0.0;
            int day = -1;
            int hour = -1;
            int minute = -1;
            int second = -1;
        };

        s data{ 39.751166666667, -75.085333333333, 4.0, 335.0, 18, 16, 13, 0 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::position_with_timestamp_utc);

        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_335/008");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
            int day = -1;
            int hour = -1;
            int minute = -1;
            int second = -1;
        };

        s data{ 39.751166666667, -75.085333333333, 18, 16, 13, 0 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::position_with_timestamp_utc);

        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
            int day = -1;
            int hour = -1;
            int minute = -1;
            // int second = -1; - required even if not used
        };

        s data{ 39.751166666667, -75.085333333333, 18, 16, 13 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::position_with_timestamp_utc);

        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:/000000z3945.07N/07505.12W_");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('_');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
        };

        s data{ 39.751166666667, -75.085333333333 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::position);

        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!3945.07N/07505.12W_");
    }

    {
        tracker t;
        t.from("N0CALL");
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

        s data{ 51.6145, -0.0485, 3.601, 297.0, 33.00 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::mic_e);

        EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");
    }

    {
        tracker t;
        t.from("N0CALL");
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
        };

        s data{ 51.6145, -0.0485, 3.601, 297.0 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::mic_e);

        EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/");
    }

    {
        tracker t;
        t.from("N0CALL");
        t.mic_e_status(mic_e_status::en_route);
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('>');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
        };

        s data{ 51.6145, -0.0485 };

        t.position(data);

        std::string packet = t.packet_string_no_message(packet_type::mic_e);

        EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwl \x1c>/");
    }
}

TEST(tracker, position)
{
    tracker t;
    t.from("N0CALL");
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

    EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");

    data.lat = 49.176666666667;
    data.lon = -123.94916666667;

    t.mic_e_status(mic_e_status::in_service);

    t.position(data);

    packet = t.packet_string_no_message(packet_type::mic_e);

    EXPECT_TRUE(packet == "N0CALL>T9QPVP,WIDE1-1:`3T{lh}>/\"48}");
}

TEST(tracker, message_and_packet_bytes)
{
    tracker t;
    t.from("N0CALL");
    t.to("APRS");
    t.path("WIDE1-1");
    t.symbol_table('/');
    t.symbol_code('_');

    struct s
    {
        double lat = 0.0;
        double lon = 0.0;
    };

    s data{ 39.751166666667, -75.085333333333 };

    t.position(data);

    // set message using a vector of bytes, by passing in the range

    std::string message = "Hello World";
    std::vector<unsigned char> message_bytes = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 };
    t.message(message_bytes);

    std::string packet = t.packet_string_no_message(packet_type::position);

    EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!3945.07N/07505.12W_");

    // get packet bytes, by passing in an output iterator

    std::vector<unsigned char> packet_bytes;
    t.packet(packet_type::position, std::back_inserter(packet_bytes));

    std::string packet_from_bytes(packet_bytes.begin(), packet_bytes.end());

    EXPECT_TRUE(packet_from_bytes == packet + message);

    // set message using a vector of bytes, by passing in the input/output iterators

    t.message(message_bytes.begin(), message_bytes.end());

    // get packet bytes, by passing in a range

    std::vector<unsigned char> packet_bytes_copy;
    packet_bytes_copy.resize(packet_bytes.size());
    t.packet(packet_type::position, packet_bytes_copy);

    std::string packet_from_bytes_copy(packet_bytes_copy.begin(), packet_bytes_copy.end());

    EXPECT_TRUE(packet_from_bytes_copy == packet + message);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









//
//
//
//
//constexpr double kPolylinePrecision = 1E6;
//constexpr double kInvPolylinePrecision = 1.0 / kPolylinePrecision;
//
//struct position_dd
//{
//    double lat;
//    double lon;
//};
//
//std::vector<position_dd> decode(const std::string& encoded) {
//    size_t i = 0;     // what byte are we looking at
//
//    // Handy lambda to turn a few bytes of an encoded string into an integer
//    auto deserialize = [&encoded, &i](const int previous) {
//        // Grab each 5 bits and mask it in where it belongs using the shift
//        int byte, shift = 0, result = 0;
//        do {
//            byte = static_cast<int>(encoded[i++]) - 63;
//            result |= (byte & 0x1f) << shift;
//            shift += 5;
//        } while (byte >= 0x20);
//        // Undo the left shift from above or the bit flipping and add to previous
//        // since its an offset
//        return previous + (result & 1 ? ~(result >> 1) : (result >> 1));
//        };
//
//    // Iterate over all characters in the encoded string
//    std::vector<position_dd> shape;
//    int last_lon = 0, last_lat = 0;
//    while (i < encoded.length()) {
//        // Decode the coordinates, lat first for some reason
//        int lat = deserialize(last_lat);
//        int lon = deserialize(last_lon);
//
//        // Shift the decimal point 5 places to the left
//        shape.emplace_back(static_cast<float>(static_cast<double>(lat) *
//            kInvPolylinePrecision),
//            static_cast<float>(static_cast<double>(lon) *
//                kInvPolylinePrecision));
//
//        // Remember the last one we encountered
//        last_lon = lon;
//        last_lat = lat;
//    }
//    return shape;
//}
//
//nlohmann::ordered_json generate_geojson(const std::vector<position_dd>& coordinates)
//{
//    nlohmann::ordered_json geojson;
//    geojson["type"] = "FeatureCollection";
//    geojson["features"] = {
//        {
//            {"type", "Feature"},
//            {"geometry", {
//                {"type", "LineString"},
//                {"coordinates", nlohmann::json::array()}
//            }},
//            {"properties", {
//                {"name", "Generated Route"},
//                {"stroke", "blue"},        // Set line color to blue
//                {"stroke-width", 3},       // Set line thickness
//                {"stroke-opacity", 0.6}    // Set line transparency
//            }}
//        }
//    };
//
//    for (const auto& point : coordinates)
//    {
//        geojson["features"][0]["geometry"]["coordinates"].push_back({ point.lon, point.lat });
//    }
//
//    return geojson;
//}
//
//void write_geojson_to_file(const nlohmann::json& geojson, const std::string& filename)
//{
//    std::ofstream file(filename);
//    if (!file)
//    {
//        std::cerr << "Error: Could not open " << filename << " for writing\n";
//        return;
//    }
//    file << geojson.dump(2) << std::endl; // Pretty print JSON with 2-space indentation
//    file.close();
//    std::cout << "GeoJSON file '" << filename << "' has been created successfully.\n";
//}
//
//void write_coordinates_to_file(const std::vector<position_dd>& coordinates, const std::string& filename)
//{
//    std::ofstream file(filename);
//    if (!file)
//    {
//        std::cerr << "Error: Could not open " << filename << " for writing\n";
//        return;
//    }
//
//    //33.76672456808148, -118.171424833816
//    file << std::fixed << std::setprecision(10); // Ensure full decimal precision
//
//    for (const auto& point : coordinates)
//    {
//        file << point.lat << ", " << point.lon << "\n";
//    }
//
//    file.close();
//
//    std::cout << "Coordinates file '" << filename << "' has been created successfully.\n";
//}
//
//std::vector<position_dd> read_coordinates_from_file(const std::string& filename)
//{
//    std::vector<position_dd> coordinates;
//    std::ifstream file(filename);
//
//    if (!file)
//    {
//        std::cerr << "Error: Could not open " << filename << " for reading\n";
//        return coordinates;
//    }
//
//    std::string line;
//    while (std::getline(file, line))
//    {
//        std::stringstream ss(line);
//        position_dd point;
//        char comma;
//
//        if (ss >> point.lat >> comma >> point.lon)
//        {
//            coordinates.push_back(point);
//        }
//    }
//
//    file.close();
//    std::cout << "Read " << coordinates.size() << " coordinates from " << filename << "\n";
//
//    return coordinates;
//}
//
//double haversine_distance(const position_dd& p1, const position_dd& p2)
//{
//    constexpr double M_PI = 3.14159265358979323846;
//    constexpr double R = 6371000; // Earth radius in meters
//    double lat1 = p1.lat * M_PI / 180.0;
//    double lon1 = p1.lon * M_PI / 180.0;
//    double lat2 = p2.lat * M_PI / 180.0;
//    double lon2 = p2.lon * M_PI / 180.0;
//
//    double dLat = lat2 - lat1;
//    double dLon = lon2 - lon1;
//
//    double a = sin(dLat / 2) * sin(dLat / 2) +
//        cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
//    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
//
//    return R * c; // Distance in meters
//}
//
//double calculate_bearing(const position_dd& p1, const position_dd& p2)
//{
//    constexpr double M_PI = 3.14159265358979323846;
//    constexpr double DEG_TO_RAD = M_PI / 180.0;
//    constexpr double RAD_TO_DEG = 180.0 / M_PI;
//
//    double lat1 = p1.lat * DEG_TO_RAD;
//    double lat2 = p2.lat * DEG_TO_RAD;
//    double dLon = (p2.lon - p1.lon) * DEG_TO_RAD;
//
//    double y = sin(dLon) * cos(lat2);
//    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
//    double bearing = atan2(y, x) * RAD_TO_DEG;
//
//    return fmod(bearing + 360.0, 360.0); // Normalize to 0-360 degrees
//}


//void run()
//{
//    if (route.empty()) {
//        std::cerr << "Error: Route is empty!\n";
//        return;
//    }

//    size_t index = 0;
//    double remaining_distance = 0.0;
//    auto last_update = std::chrono::high_resolution_clock::now();

//    PointLL current_position = route[index];

//    while (index < route.size() - 1) {
//        auto now = std::chrono::high_resolution_clock::now();
//        auto elapsed = std::chrono::duration<double>(now - last_update).count();
//        last_update = now;

//        PointLL next_position = route[index + 1];
//        double segment_distance = haversineDistance(current_position, next_position); // meters
//        double travel_distance = speed_mps * elapsed; // meters based on actual elapsed time
//        double heading = calculateBearing(current_position, next_position);

//        if (remaining_distance + travel_distance >= segment_distance) {
//            // Move to the next point
//            remaining_distance = (remaining_distance + travel_distance) - segment_distance;
//            index++;

//            if (index < route.size()) {
//                current_position = route[index];
//            }
//        }
//        else {
//            // Interpolate between current and next point based on actual time elapsed
//            double ratio = (remaining_distance + travel_distance) / segment_distance;
//            current_position.lat = current_position.lat + ratio * (next_position.lat - current_position.lat);
//            current_position.lon = current_position.lon + ratio * (next_position.lon - current_position.lon);
//            remaining_distance += travel_distance;
//        }

//        // Output simulated GPS position
//        std::cout << "Simulated GPS: " << current_position.lat << ", " << current_position.lon
//            << ", heading: " << heading << "°, speed: " << speed_mps * 2.23694 << " mph" << std::endl;

//        // Wait for next update interval while accounting for processing time
//        auto processing_time = std::chrono::high_resolution_clock::now() - now;
//        auto sleep_time = std::chrono::duration<double>(update_interval) - processing_time;
//        if (sleep_time.count() > 0) {
//            std::this_thread::sleep_for(sleep_time);
//        }
//    }

//    std::cout << "Simulation completed.\n";
//}


// simulate a GPS route at a constant speed
// just traverse a route - no speed limit

//void timer_handler(const boost::system::error_code& ec)
//{
//    if (ec) {
//        return;
//    }
//
//    if (index_ >= route.size() - 1) {
//        std::cout << "Simulation completed.\n";
//        return;
//    }
//
//    auto now = std::chrono::high_resolution_clock::now();
//    auto elapsed_seconds = std::chrono::duration<double>(now - last_update_).count();
//    last_update_ = now;
//
//    position_dd next_position = route[index_ + 1];
//
//    double segment_distance = haversine_distance(current_position, next_position);
//    double travel_distance = speed_mps * elapsed_seconds * time_factor;
//    current_heading = calculate_bearing(current_position, next_position);
//
//    segment_progress_ += travel_distance / segment_distance;
//
//    if (segment_progress_ >= 1.0)
//    {
//        index_++;
//        if (index_ >= route.size() - 1)
//        {
//            return;
//        }
//
//        current_position = route[index_];
//        segment_progress_ = 0.0;
//    }
//    else
//    {
//        current_position.lat = route[index_].lat + segment_progress_ * (route[index_ + 1].lat - route[index_].lat);
//        current_position.lon = route[index_].lon + segment_progress_ * (route[index_ + 1].lon - route[index_].lon);
//    }
//
//    // Output simulated GPS position
//    std::cout << "Simulated GPS: " << current_position.lat << ", " << current_position.lon << ", " << current_heading << std::endl;
//
//    // Schedule next update
//    timer_.expires_after(std::chrono::milliseconds(static_cast<int>(update_interval * 1000.0 / time_factor)));
//    timer_.async_wait(std::bind(&route_simulator::timer_handler, this, std::placeholders::_1));
//}
//
//void run()
//{
//    if (route.empty())
//    {
//        return;
//    }
//
//    index_ = 0;
//    segment_progress_ = 0.0;
//    last_update_ = std::chrono::high_resolution_clock::now();
//    current_position = route[index_];
//
//    // Start the timer
//    timer_.expires_after(std::chrono::milliseconds(static_cast<int>(update_interval * 1000.0 / time_factor)));
//    timer_.async_wait(std::bind(&route_simulator::timer_handler, this, std::placeholders::_1));
//
//    // Run the io_context
//    io_context_.run();
//}
//
//  position_dd current_position;
//double current_heading;
//std::vector<position_dd> route;
//double speed_mps; // meters per second  
//int update_interval; // seconds
//int time_factor; // speed up simulation by this factor
//
//boost::asio::io_context io_context_;
//boost::asio::steady_timer timer_;
//size_t index_ = 0;
//double segment_progress_ = 0.0;
//std::chrono::time_point<std::chrono::high_resolution_clock> last_update_;
//
//struct route_speed_simulator
//{
//    route_speed_simulator()
//    {
//    }
//
//    route_speed_simulator(const std::vector<position_dd>& route, double speed_mps, int update_interval_seconds, int time_factor = 1)
//        : route(route), speed_mps(speed_mps), update_interval(update_interval_seconds), time_factor(time_factor)
//    {
//    }
//
//    void run()
//    {
//        if (route.empty())
//        {
//            return;
//        }
//
//        size_t index = 0;
//        double segment_progress = 0.0; // progress along current segment (0.0 to 1.0)
//        auto last_update = std::chrono::high_resolution_clock::now();
//
//        current_position = route[index];
//
//        while (index < route.size() - 1)
//        {
//            auto now = std::chrono::high_resolution_clock::now();
//            auto elapsed_seconds = std::chrono::duration<double>(now - last_update).count();
//            last_update = now;
//
//            position_dd next_position = route[index + 1];
//
//            double segment_distance = haversine_distance(current_position, next_position); // meters
//            double travel_distance = speed_mps * elapsed_seconds * time_factor; // meters per update
//            current_heading = calculate_bearing(current_position, next_position);
//
//            segment_progress += travel_distance / segment_distance;
//
//            if (segment_progress >= 1.0)
//            {
//                // Move to the next point
//                index++;
//                if (index >= route.size() - 1)
//                {
//                    // end of route
//                    break;
//                }
//
//                current_position = route[index];
//                segment_progress = 0.0;
//            }
//            else
//            {
//                current_position.lat = route[index].lat + segment_progress * (route[index + 1].lat - route[index].lat);
//                current_position.lon = route[index].lon + segment_progress * (route[index + 1].lon - route[index].lon);
//            }
//
//            double update_interval_ms = update_interval * 1000.0;
//            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(update_interval_ms / time_factor)));
//
//            // Output simulated GPS position
//            std::cout << "Simulated GPS: " << current_position.lat << ", " << current_position.lon << ", " << current_heading << std::endl;
//        }
//
//        std::cout << "Simulation completed.\n";
//    }
//
//    position_dd current_position;
//    double current_heading;
//    std::vector<position_dd> route;
//    double speed_mps; // meters per second
//    int update_interval; // seconds
//    int time_factor; // speed up simulation by this factor
//};
//
//struct route_static_simulator
//{
//
//};
//
//int main2()
//{
//    // Decode the polyline string
//    aprs::track::detail::encode_mic_e_alt(200);
//
//    std::string encoded = "ymwxyAnqkhhFElbA?xD?tDElcA?xEAfGAl]AbIAhV?bIA`IC~W@|k@A`E@lCDlCHlCJjCLjCPjCRhCVhCXlC^jC^jCd@hCd@fCj@bCj@bCn@`Cl@jBFRp@xBrHzU~O~g@tCzJHX`@nAJ^p@vBZtA^nAzAvEnLr^xAjEvF~PvDnLdDrKxCxJrAjEjB|FjDnK`EhMvKh]fAfDpAdErAhEvGbTbJxYhB~FtAfExLp_@zClJlDvKZ|@t@bCjApDvWfz@nAxDiDpCkQ|NeQvNiBzAaAhC_CnGgEzHuGnHwm@lh@sb@v_@{SnRsZpW_QvOuJbHmGfDeGpCmIvDeSzKiGfAkGz@mGn@mGb@mGXoGJsIFsIIsI[sIk@qI_AoIoAmIaBeGkAwkAa\\mc@mKkIyCcOcG{Ny@q^iMeOkEyD_AgM{Cat@iSe~@uVga@}Isa@eGg_@qEcMiAgIo@iQkAuPq@_n@eBet@_A_dJKcGKaGScG[_Gg@aGo@_Gw@_GaA}FkA{FsA{F}AyFgBwFmBuFyBsF_CqFiCoFsCkFyCiFcDgFkDeFsDaF}D}EcE{EkEsn@yn@qu@ww@sG_HuGwGsFiFyF_F{FuE{BeBaJmG{FmDwEmCyEgCyE_CuFeCuF}BwFsB{FkBu{Asd@_LcDcLuCcLiCgL}BgLqBgLeBkLwAkLkAkL_AmLq@mLg@gISyGKgp@y@}u@aAul@}Fs|@oCck@kCcDe@_Dk@wCy@eEwAsEyBiEqCwDuCyE{E{CqD{D_G{CoFaDgH_DuIoB_HaBiHoAwHuAiK_Juy@yAoOqAaQaDk_@aEsx@kCee@qHyxAkAsXsAqa@e@}WMsXmAyrDSwU_@qSa@eMeAgXcB_UiCwXw]mmCiDiXyD{\\sDy_@yDmc@{Fig@cDg^sA_Uc@}TiCsbAaAcPqAwVeBmWqBgg@aDcr@aJoaBoEckAkAgd@w@qd@Wkn@V{_@p@_a@bNkjFhMu{EzL}bFtAod@tBsc@hCgc@hDud@tKwfAvLwfAxZuuCbTe`ChE{j@pEkp@npA}oOj_Ea_e@l[crCtJg{@dWiyA`Ns{@~Fkb@p@_F~AeLpF}f@dByOpBaZ|@gM|KaeBrGc{@vGqc@vGqn@fBqNhFsh@rAwNfAsKjAwPpA_Rh@eLp@uMjA}HbAka@MgGY_DMkCFmC\\gCl@wB|@eBjAmArAu@zK_CpAk@tEkBzCGdHI~h@RdAHhBEtAEbmAHnKSpIItECjQS`DFAeH?cTA}VGq~ACsm@?_GCax@EqrAEqrAAqa@Cqh@Csy@Cgu@?kCAsD@qDA}DCoN?m@SklCC}{@Euv@Gsf@?sD?_D@yCBsDKwN@sHQswB?uEAyCAcE?{AM}_@";
//    std::vector<position_dd> points = decode(encoded);  
//    nlohmann::json geojson = generate_geojson(points);
//    //std::cout << geojson.dump(2) << std::endl;
//    write_geojson_to_file(geojson, "route.geojson");
//    write_coordinates_to_file(points, "coordinates.txt");
//
//
//    //points = readCoordinatesFromFile("/home/iontodirel/route1.points.txt");
//
//
//    route_speed_simulator simulator;
//    simulator.route = points;
//    simulator.speed_mps = 26.8224; // 60 MPH = 26.8224 m/s
//    simulator.update_interval = 1.0; // 1 second
//    simulator.time_factor = 1; // real time
//    //simulator.run();
//
//    //aprs::track::detail::encode_compressed_lon(-72.75);
//    //aprs::track::detail::encode_compressed_course_speed(88, 36);
//
//    //aprs::track::detail::encode_position_data_compressed_no_timestamp('=', 49.5, -72.75, '/', '>', 88, 36, '[');
//    //aprs::track::detail::encode_mic_e_data('`', 33.4274, -72.75, '/', '>');
//    std::string rr = "S32U6T";
//    aprs::track::detail::add_mic_e_position_ambiguity(rr, 6);
//    aprs::track::detail::encode_mic_e_lat(49.172666666667, -124.03383333333, aprs::track::mic_e_status::en_route, 0);
//    aprs::track::detail::encode_mic_e_data('`', 49.172666666667, -124.03383333333, 61, 28, '/', '>');
//
//
//
//
//
//
//    gpsd_client gpsd("192.168.1.11", 8888);
//    gpsd.open();
//
//    aprs::track::tracker t;
//
//    t.message("hello, world!");
//    t.algorithm(aprs::track::algorithm::smart_beaconing);
//    t.message("Hello, world!");
//    t.symbol_code('I');
//    t.symbol_table('/');
//    t.from("N0CALL");
//    t.to("APRS"); // not used for mic-e
//    t.path("WIDE1-1,WIDE2-2");
//    t.interval(std::chrono::seconds(5)); // not used for smart beaconing
//    t.ambiguity(0); // no ambiguity (default)
//    t.mic_e_status(aprs::track::mic_e_status::en_route);
//    t.messaging(false); // no messaging (default)
//    // smart beaconing
//    t.low_speed(5); // 5 mph
//    t.high_speed(60); // 60 mph
//    t.slow_rate(60); // sec
//    t.fast_rate(10); // sec
//    t.turn_angle(28); // degrees
//    t.turn_time(30); // sec
//    t.turn_slope(26); // 1 to 255
//
//    while (true)
//    {
//        gnss_info info;
//        gpsd.try_get_gps_info(info, gnss_include_info::all, 1000);
//        
//        // generic, expects any type which contains fields: lat, lon
//        // and optionally will use fields if they exist: speed, track, alt, day, hour, minute, second
//        t.position(info);
//
//        // update the period calculations
//        t.update();
//
//        if (t.updated())
//        {
//            // get the full packet string
//            // other packet types supported: position, position_compressed, position_with_timestamp,
//            // position_with_timestamp_utc, position_with_timestamp_utc_mhs
//            std::string packet = t.packet_string(aprs::track::packet_type::mic_e);
//
//            fmt::println("{}", packet);
//        }
//    }
//
//    
//
//
//
//    // speed
//    // utf8
//    // max size 36 for the message
//    // max size 43 for the comment
//    // with or without messaging
//
//    // 0b00100000 - current, other, compressed
//    // 0b00000000 - old, other, compressed
//    // 0b00111000 - current, RMC, compressed
//    // 0b00011000 - old, RMC, compressed
//    // https://aprs.to/v/packet_decoder
//
//
//    return 0;
//}


/*


  /// @cond undocumented
  template<typename _CharT, typename _Traits, typename _Allocator>
    std::basic_string<_CharT, _Traits, _Allocator>
    path::_S_str_convert(basic_string_view<value_type> __str,
             const _Allocator& __a)
    {
      static_assert(!is_same_v<_CharT, value_type>);

      using _WString = basic_string<_CharT, _Traits, _Allocator>;

      if (__str.size() == 0)
    return _WString(__a);

#ifndef _GLIBCXX_FILESYSTEM_IS_WINDOWS
      string_view __u8str = __str;
#else
      // First convert native string from UTF-16 to to UTF-8.
      // XXX This assumes that the execution wide-character set is UTF-16.
      std::codecvt_utf8_utf16<value_type> __cvt;

      using _CharAlloc = __alloc_rebind<_Allocator, char>;
      using _String = basic_string<char, char_traits<char>, _CharAlloc>;
      _String __u8str{_CharAlloc{__a}};
      const value_type* __wfirst = __str.data();
      const value_type* __wlast = __wfirst + __str.size();
      if (!__str_codecvt_out_all(__wfirst, __wlast, __u8str, __cvt))
    __detail::__throw_conversion_error();
      if constexpr (is_same_v<_CharT, char>)
    return __u8str; // XXX assumes native ordinary encoding is UTF-8.
      else
#endif
    {
      const char* __first = __u8str.data();
      const char* __last = __first + __u8str.size();

      // Convert UTF-8 string to requested format.
#ifdef _GLIBCXX_USE_CHAR8_T
      if constexpr (is_same_v<_CharT, char8_t>)
        return _WString(__first, __last, __a);
      else
#endif
        {
          // Convert UTF-8 to wide string.
          _WString __wstr(__a);
          path::_Codecvt<_CharT> __cvt;
          if (__str_codecvt_in_all(__first, __last, __wstr, __cvt))
        return __wstr;
        }
    }
      __detail::__throw_conversion_error();
    }
  /// @endcond

  template<typename _CharT, typename _Traits, typename _Allocator>
    inline basic_string<_CharT, _Traits, _Allocator>
    path::string(const _Allocator& __a) const
    {
      if constexpr (is_same_v<_CharT, value_type>)
    return { _M_pathname.c_str(), _M_pathname.length(), __a };
      else
    return _S_str_convert<_CharT, _Traits>(_M_pathname, __a);
    }

  inline std::string
  path::string() const { return string<char>(); }

#if _GLIBCXX_USE_WCHAR_T
  inline std::wstring
  path::wstring() const { return string<wchar_t>(); }
#endif

#ifdef _GLIBCXX_USE_CHAR8_T
  inline std::u8string
  path::u8string() const { return string<char8_t>(); }
#else
  inline std::string
  path::u8string() const
  {
#ifdef _GLIBCXX_FILESYSTEM_IS_WINDOWS
    std::string __str;
    // convert from native wide encoding (assumed to be UTF-16) to UTF-8
    std::codecvt_utf8_utf16<value_type> __cvt;
    const value_type* __first = _M_pathname.data();
    const value_type* __last = __first + _M_pathname.size();
    if (__str_codecvt_out_all(__first, __last, __str, __cvt))
      return __str;
    __detail::__throw_conversion_error();
#else
    return _M_pathname;
#endif
  }
#endif // _GLIBCXX_USE_CHAR8_T

  inline std::u16string
  path::u16string() const { return string<char16_t>(); }

  inline std::u32string
  path::u32string() const { return string<char32_t>(); }

  template<typename _CharT, typename _Traits, typename _Allocator>
    inline std::basic_string<_CharT, _Traits, _Allocator>
    path::generic_string(const _Allocator& __a) const
    {
#ifdef _GLIBCXX_FILESYSTEM_IS_WINDOWS
      const value_type __slash = L'/';
#else
      const value_type __slash = '/';
#endif
      using _Alloc2 = typename allocator_traits<_Allocator>::template
    rebind_alloc<value_type>;
      basic_string<value_type, char_traits<value_type>, _Alloc2> __str(__a);

      if (_M_type() == _Type::_Root_dir)
    __str.assign(1, __slash);
      else
    {
      __str.reserve(_M_pathname.size());
      bool __add_slash = false;
      for (auto& __elem : *this)
        {
#ifdef _GLIBCXX_FILESYSTEM_IS_WINDOWS
          if (__elem._M_type() == _Type::_Root_dir)
        {
          __str += __slash;
          continue;
        }
#endif
          if (__add_slash)
        __str += __slash;
          __str += basic_string_view<value_type>(__elem._M_pathname);
          __add_slash = __elem._M_type() == _Type::_Filename;
        }
    }

      if constexpr (is_same_v<_CharT, value_type>)
    return __str;
      else
    return _S_str_convert<_CharT, _Traits>(__str, __a);
    }


*/