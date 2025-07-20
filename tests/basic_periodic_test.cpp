// **************************************************************** //
// libaprstrack - APRS tracking library                             //
// Version 0.1.0                                                    //
// https://github.com/iontodirel/libaprstrack                       //
// Copyright (c) 2025 Ion Todirel                                   //
// **************************************************************** //
//
// basic_periodic_test.cpp
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

#include <gtest/gtest.h>

#include "../aprstrack.hpp"

using namespace aprs::track;
using namespace aprs::track::detail;

#define LIBAPRSTRACK_DISABLE_PERIODIC

TEST(tracker, basic_periodic)
{
#ifdef LIBAPRSTRACK_DISABLE_PERIODIC
    EXPECT_TRUE(true) << "Periodic tracking is disabled, skipping test.";
    return;
#endif

    tracker t;
    t.from("N0CALL");
    t.mic_e_status(mic_e_status::en_route);
    t.path("WIDE1-1");
    t.symbol_table('/');
    t.symbol_code('>');
    t.algorithm(algorithm::periodic);

    int interval_seconds = 1;

    t.interval(std::chrono::seconds(interval_seconds));

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

    int count = 10;
    int i = 0;
    bool first = true;

    auto last_update = std::chrono::high_resolution_clock::now();

    while (i < count)
    {
        t.position(data);

        t.update();

        if (t.updated())
        {
            std::string packet = t.packet_string_no_message(packet_type::mic_e);

            EXPECT_TRUE(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");

            auto now = std::chrono::high_resolution_clock::now();
            double elapsed_seconds = std::chrono::duration<double>(now - last_update).count();
            last_update = std::chrono::high_resolution_clock::now();
            i++;

            if (!first)
            {
                EXPECT_NEAR(elapsed_seconds, interval_seconds, 0.5);
            }

            first = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}