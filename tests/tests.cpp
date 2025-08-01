﻿// **************************************************************** //
// libaprstrack - APRS tracking library                             // 
// Version 0.1.0                                                    //
// https://github.com/iontodirel/libaprstrack                       //
// Copyright (c) 2025 Ion Todirel                                   //
// **************************************************************** //
//
// aprstrack.hpp
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

#include <boost/beast/core/detail/base64.hpp>

#include "../aprstrack.hpp"

#include "tests.hpp"

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
    EXPECT_TRUE(encode_course_speed(0, 0) == "000/000");
}

TEST(packet_type, packet_type_1_2)
{
    EXPECT_TRUE(packet_type_without_timestamp(true) == '=');
    EXPECT_TRUE(packet_type_without_timestamp(false) == '!');
    EXPECT_TRUE(packet_type_with_timestamp(true) == '@');
    EXPECT_TRUE(packet_type_with_timestamp(false) == '/');
}

TEST(packet, encode_header)
{
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "WIDE1-1,WIDE2-2") == "N0CALL>APRS,WIDE1-1,WIDE2-2:");
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "WIDE1-1") == "N0CALL>APRS,WIDE1-1:");
    EXPECT_TRUE(encode_header("N0CALL", "APRS", "") == "N0CALL>APRS:");
    EXPECT_TRUE(encode_header("N0CALL", "", "") == "N0CALL>:");
    EXPECT_TRUE(encode_header("", "", "") == ">:");
}

TEST(position, dd_to_dms)
{
    {
        auto [d, m, s] = dd_to_dms(37.7749);
        EXPECT_TRUE(d == 37);
        EXPECT_TRUE(m == 46);
        EXPECT_NEAR(s, 29.64, 0.01);
    }

    {
        auto [d, m, s] = dd_to_dms(42.15188);
        EXPECT_TRUE(d == 42);
        EXPECT_TRUE(m == 9);
        EXPECT_NEAR(s, 6.768, 0.001);
    }

    {
        auto [d, m, s] = dd_to_dms(47.636453031500814);
        EXPECT_TRUE(d == 47);
        EXPECT_TRUE(m == 38);
        EXPECT_NEAR(s, 11.23091, 0.00001);
    }

    {
        auto [d, m, s] = dd_to_dms(-122.28699440021819);
        EXPECT_TRUE(d == -122);
        EXPECT_TRUE(m == 17);
        EXPECT_NEAR(s, 13.17984, 0.00001);
    }

    {
        auto [d, m, s] = dd_to_dms(-0.12967150593338872);
        EXPECT_TRUE(d == 0);
        EXPECT_TRUE(m == 7);
        EXPECT_NEAR(s, 46.81742, 0.00001);
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
        auto [dd, m] = dd_to_ddm(-37.7749);
        EXPECT_TRUE(dd == -37);
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

    {
        auto ddm = dd_to_ddm(51.469041, -0.038108);
        EXPECT_TRUE(ddm.lat == 'N');
        EXPECT_TRUE(ddm.lat_d == 51);
        EXPECT_NEAR(ddm.lat_m, 28.1425, 0.0001);
        EXPECT_TRUE(ddm.lon == 'W');
        EXPECT_TRUE(ddm.lon_d == 0);
        EXPECT_NEAR(ddm.lon_m, 2.2865, 0.0001);
    }

    {
        auto ddm = dd_to_ddm(-0.775198, -77.707248);
        EXPECT_TRUE(ddm.lat == 'S');
        EXPECT_TRUE(ddm.lat_d == 0);
        EXPECT_NEAR(ddm.lat_m, 46.5119, 0.0001);
        EXPECT_TRUE(ddm.lon == 'W');
        EXPECT_TRUE(ddm.lon_d == 77);
        EXPECT_NEAR(ddm.lon_m, 42.4349, 0.0001);
    }

    {
        auto ddm = dd_to_ddm(1.464833, 103.737833);
        EXPECT_TRUE(ddm.lat == 'N');
        EXPECT_TRUE(ddm.lat_d == 1);
        EXPECT_NEAR(ddm.lat_m, 27.89, 0.001);
        EXPECT_TRUE(ddm.lon == 'E');
        EXPECT_TRUE(ddm.lon_d == 103);
        EXPECT_NEAR(ddm.lon_m, 44.27, 0.001);
    }

    {
        auto ddm = dd_to_ddm(-0.987982, 16.276024);
        EXPECT_TRUE(ddm.lat == 'S');
        EXPECT_TRUE(ddm.lat_d == 0);
        EXPECT_NEAR(ddm.lat_m, 59.2789, 0.0001);
        EXPECT_TRUE(ddm.lon == 'E');
        EXPECT_TRUE(ddm.lon_d == 16);
        EXPECT_NEAR(ddm.lon_m, 16.5614, 0.001);
    }

    {
        auto ddm = dd_to_ddm(0.0, 0.0);
        EXPECT_TRUE(ddm.lat == 'N'); // or 'S', but typically 'N' is used for 0 latitude
        EXPECT_TRUE(ddm.lat_d == 0);
        EXPECT_NEAR(ddm.lat_m, 0.0, 0.001);
        EXPECT_TRUE(ddm.lon == 'E'); // or 'W', but typically 'E' is used for 0 longitude
        EXPECT_TRUE(ddm.lon_d == 0);
        EXPECT_NEAR(ddm.lon_m, 0.0, 0.001);
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

    {
        auto ddm = dd_to_ddm(-27.2973333333333, -49.0005);
        auto ddm_str = to_ddm_short_string(ddm, 0);
        EXPECT_TRUE(ddm_str.lat == "2717.84S"); // NOTE: FAP decoded 2677.84S as -27.2973333333333
        EXPECT_TRUE(ddm_str.lon == "04900.03W");
    }

    {
        auto ddm = dd_to_ddm(-22.9061007326007, -43.2896996336996);
        auto ddm_str = to_ddm_short_string(ddm, 0);
        // should it be 2254.36S?
        // original packet: PU1XMG-14>APRS,TCPIP*,qAC,T2PANAMA:!2254.36S/04317.38W
        // real DDM: 22° 54.366' S
        // it seems that 2254.37S is more accurate than 2254.36S
        EXPECT_TRUE(ddm_str.lat == "2254.37S");
        EXPECT_TRUE(ddm_str.lon == "04317.38W");
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
    EXPECT_TRUE(format_number_to_string(999999.9999, 4) == "999999.9999");
    EXPECT_TRUE(format_number_to_string(999999.9999, 2) == "1000000.00");
    EXPECT_TRUE(format_number_to_string(0.000001, 6) == "0.000001");
    EXPECT_TRUE(format_number_to_string(0.000001, 3) == "0.000");
    EXPECT_TRUE(format_number_to_string(-37.7749, 4) == "-37.7749");
    EXPECT_TRUE(format_number_to_string(-37.7749, 2) == "-37.77");
    EXPECT_TRUE(format_number_to_string(-0.7749, 5, 2) == "-00.77");
    EXPECT_TRUE(format_number_to_string(0.0, 4) == "0.0000");
    EXPECT_TRUE(format_number_to_string(0.0, 0) == "0");
    EXPECT_TRUE(format_number_to_string(0.0, 5, 2) == "000.00");
    EXPECT_TRUE(format_number_to_string(1.5555, 3) == "1.556");
    EXPECT_TRUE(format_number_to_string(1.5555, 0) == "1");
    EXPECT_TRUE(format_number_to_string(-1.5555, 3) == "-1.556");
    EXPECT_TRUE(format_number_to_string(5.5, 8, 4) == "0005.5000");
    EXPECT_TRUE(format_number_to_string(0.5, 8, 4) == "0000.5000");
    EXPECT_TRUE(format_number_to_string(-5.5, 8, 4) == "-005.5000");
}

TEST(format, format_n_digits_string)
{
    EXPECT_TRUE(format_two_digits_string(0) == "00");
    EXPECT_TRUE(format_two_digits_string(1) == "01");
    EXPECT_TRUE(format_two_digits_string(9) == "09");
    EXPECT_TRUE(format_two_digits_string(10) == "10");
    EXPECT_TRUE(format_two_digits_string(99) == "99");
    EXPECT_TRUE(format_two_digits_string(199) == "199");

    EXPECT_TRUE(format_n_digits_string(0, 2) == "00");
    EXPECT_TRUE(format_n_digits_string(1, 2) == "01");
    EXPECT_TRUE(format_n_digits_string(9, 2) == "09");
    EXPECT_TRUE(format_n_digits_string(10, 2) == "10");
    EXPECT_TRUE(format_n_digits_string(99, 2) == "99");
    EXPECT_TRUE(format_n_digits_string(12, 6) == "000012");
    EXPECT_TRUE(format_n_digits_string(12, 0) == "12");
    EXPECT_TRUE(format_n_digits_string(12, 1) == "12");
    EXPECT_TRUE(format_n_digits_string(12, 2) == "12");
    EXPECT_TRUE(format_n_digits_string(12, -1) == "12");
    EXPECT_TRUE(format_n_digits_string(12, -2) == "12");
}

TEST(conversion, unit_conversions)
{
    EXPECT_NEAR(meters_to_feet(1), 3.28084, 0.00001);
    EXPECT_NEAR(mps_to_knots(1), 1.94384, 0.00001);
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

    {
        std::string course_speed_str = encode_compressed_course_speed(360, 0.166383);
        EXPECT_TRUE(course_speed_str == "!#");
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

TEST(position, compression_type_to_int)
{
    {
        int type = compression_type_to_int(compression_type::current_gga_compressed);
        EXPECT_TRUE(type == 0b00110000);
    }

    {
        int type = compression_type_to_int(compression_type::current_rmc_digipeater);
        EXPECT_TRUE(type == 0b00111111);
    }

    {
        int type = compression_type_to_int(compression_type::old_other_compressed);
        EXPECT_TRUE(type == 0b00000000);
    }

    {
        int type = compression_type_to_int(compression_type::current_gga_tbd_1);
        EXPECT_TRUE(type == 0b00110011);
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

    {
        std::string lon_str = encode_mic_e_lon(-123.38964468864501);
        EXPECT_TRUE(lon_str == "33A");

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

TEST(mic_e, encode_mic_e_alt_feet)
{
    {
        std::string alt = encode_mic_e_alt_feet(59.06);
        EXPECT_TRUE(alt == "\"4)}");
    }

    {
        std::string alt = encode_mic_e_alt_feet(200);
        EXPECT_TRUE(alt == "\"4T}");
    }

    {
        std::string alt = encode_mic_e_alt_feet(308.4);
        EXPECT_TRUE(alt == "\"4u}");
    }

    {
        std::string alt = encode_mic_e_alt_feet(400.3);
        EXPECT_TRUE(alt == "\"56}");
    }

    {
        std::string alt = encode_mic_e_alt_feet(672.6);
        EXPECT_TRUE(alt == "\"6.}");
    }

    {
        std::string alt = encode_mic_e_alt_feet(2136.8);
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
        // This is what APRS spec has to say about it:
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

    {
        // K2EHE>S9UW2T,W2ZQ*,WIDE2-1,qAO,W2ZQ:`fPel+L-/>"3e}=
        //                                              ~
        // NOTE: '>' inserted in the 10th position is part of the message
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 39.954, -74.878833333333, mic_e_status::in_service, 148, 1, '/', '-', 0, -42.65);
        EXPECT_TRUE(packet == "N0CALL>S9UW2T,WIDE1-1:`fPel+L-/\"3e}");
    }

    {
        // JJ2EJL-9>STUXRL,JF2ZGN-2*,WIDE2-1,qAR,JA1YCQ-10:`C^-l!;j/`"4?}_(
        //                                                          ~
        // NOTE: '`' inserted in the 10th position is part of the message
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 34.970833333333, 139.1025, mic_e_status::off_duty, 131, 0, '/', 'j', 1, 131.23);
        EXPECT_TRUE(packet == "N0CALL>STUXRL,WIDE1-1:`C^+l!;j/\"4?}");
        //                                              ~
        // NOTE: very small difference due to floating point error
    }

    {
        // 2E0KFG-9>U1UQY3,WIDE1-1,WIDE2-1,qAR,MB7UBE:`vSb<0x1c><0x1c>F>/"6#}
        std::string packet = encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 51.8655, 0.92833333333333, mic_e_status::in_service, 42, 0, '/', '>', 0, 636.48);
        EXPECT_TRUE(packet == "N0CALL>U1UQY3,WIDE1-1:`vSbl F>/\"6#}");
        std::string course_speed_str = encode_mic_e_course_speed(42, 0);
        EXPECT_TRUE(course_speed_str == "l F");
        std::string course_speed_str_alt = encode_mic_e_course_speed_alternate(42, 0);
        EXPECT_TRUE(course_speed_str_alt == "\x1c\x1c""F");
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

    {
        // FAP parses N0CALL>APRS,WIDE1-1:!2677.84S/04900.03W# to -27.2973333333333, -49.0005
        // Original packet: N0CALL>APRS,WIDE1-1:!2677.84S/04900.03W#
        std::string packet = encode_position_packet_no_timestamp_no_message("N0CALL", "APRS", "WIDE1-1", false, -27.2973333333333, -49.0005, '/', '#', 0);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!2717.84S/04900.03W#");
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

    {
        // 2E0WXF-3>APDW17,WIDE2-1,qAO,2E0WXF-2:!b4KfKNN3c#
        // NOTE: why is the symbol table 'b' instead of 1 when decoded by FAP?
        std::string data = encode_position_data_compressed_no_timestamp('!', 51.4834062258811, 0.0089466195533987, '1', '#', 0b01001000);
        EXPECT_TRUE(data == "!14KfKNN3c#H");
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

    {
        tracker t;
        t.from("N0CALL");
        t.to("APRS");
        t.path("WIDE1-1");
        t.symbol_table('/');
        t.symbol_code('[');

        struct s
        {
            double lat = 0.0;
            double lon = 0.0;
            double speed = 0.0;
            double track = 0.0;
        };

        s data{ 50.656773231546 , 6.4228800344424, 0.0856036, 360 };

        t.position(data);

        // Y - compression type
        std::string packet = t.packet_string(packet_type::position_compressed);
        EXPECT_TRUE(packet == "N0CALL>APRS,WIDE1-1:!/4qheP+c)[!#Y");
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

TEST(tracker, u8packet_string)
{
    tracker t;
    t.from("N0CALL");
    t.path("WIDE1-1");
    t.symbol_table('/');
    t.symbol_code('>');
    t.mic_e_status(mic_e_status::en_route);

    t.message(u8"Hello 世界");

    t.position(51.6145, -0.0485);
    t.speed(3.601);
    t.track(297.0);
    t.alt(33.00);

    std::u8string u8packet = t.u8packet_string(packet_type::mic_e);

    EXPECT_TRUE(u8packet == u8"N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}Hello 世界");
}

TEST(tracker, auto_tests)
{
    std::string file_path = INPUT_TEST_FILE;

    std::vector<packet_data> packets = parse_json_basic(file_path);

    EXPECT_TRUE(!packets.empty());

    for (const auto& packet : packets)
    {
        std::string expected_packet_string = packet.packet;
        std::string actual_packet_string;

        if (packet.packet_type == packet_type::mic_e)
        {
            if (!packet.alt_str.empty())
            {
                // Mic-E with altitude
                actual_packet_string = encode_mic_e_packet_no_message(
                    packet.from,
                    packet.path,
                    packet.lat,
                    packet.lon,
                    packet.mic_e_status,
                    packet.course,
                    kmh_to_knots(packet.speed),
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0,
                    meters_to_feet(packet.alt));
            }
            else
            {
                // Mic-E without altitude
                actual_packet_string = encode_mic_e_packet_no_message(
                    packet.from,
                    packet.path,
                    packet.lat,
                    packet.lon,
                    packet.mic_e_status,
                    packet.course,
                    kmh_to_knots(packet.speed),
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0);
            }
        }
        else if (packet.packet_type == packet_type::position)
        {
            if (packet.alt_str.empty() && packet.course_str.empty() && packet.speed_str.empty())
            {
                // Position without altitude, course, or speed
                actual_packet_string = encode_position_packet_no_timestamp_no_message(
                    packet.from,
                    packet.to,
                    packet.path,
                    packet.messaging_str == "true",
                    packet.lat,
                    packet.lon,
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0);
            }
            else if (packet.alt_str.empty() && (!packet.course_str.empty() || !packet.speed_str.empty()))
            {
                // Position without altitude, but with course or speed
                actual_packet_string = encode_position_packet_no_timestamp_no_message(
                    packet.from,
                    packet.to,
                    packet.path,
                    packet.messaging_str == "true",
                    packet.lat,
                    packet.lon,
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0,
                    kmh_to_knots(packet.speed),
                    packet.course);
            }
            else if (!packet.alt_str.empty() && packet.course_str.empty() && packet.speed_str.empty())
            {
                // Position with altitude, but without course or speed
                actual_packet_string = encode_position_packet_no_timestamp_no_message(
                    packet.from,
                    packet.to,
                    packet.path,
                    packet.messaging_str == "true",
                    packet.lat,
                    packet.lon,
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0,
                    meters_to_feet(packet.alt));
            }
            else if (!packet.alt_str.empty() && (!packet.course_str.empty() || !packet.speed_str.empty()))
            {
                // Position with altitude, course, and speed
                actual_packet_string = encode_position_packet_no_timestamp_no_message(
                    packet.from,
                    packet.to,
                    packet.path,
                    packet.messaging_str == "true",
                    packet.lat,
                    packet.lon,
                    packet.symbol_table[0],
                    packet.symbol_code[0],
                    0,
                    kmh_to_knots(packet.speed),
                    packet.course,
                    meters_to_feet(packet.alt));
            }
        }

        bool result = expected_packet_string.starts_with(actual_packet_string);

        if (!result)
        {
            printf("Actual: %s\n", actual_packet_string.c_str());
            printf("Expected: %s\n", expected_packet_string.c_str());
        }

        EXPECT_TRUE(result);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
