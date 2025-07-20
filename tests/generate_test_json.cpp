// **************************************************************** //
// libaprstrack - APRS tracking library                             //
// Version 0.1.0                                                    //
// https://github.com/iontodirel/libaprstrack                       //
// Copyright (c) 2025 Ion Todirel                                   //
// **************************************************************** //
//
// generate_test_json.cpp
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

#include "../aprstrack.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "tests.hpp"

using namespace aprs::track;
using namespace aprs::track::detail;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_file_path_1> <input_file_path_2> ... <output_file_path>" << std::endl;
        return 1;
    }

    std::vector<std::string> input_file_paths;

    for (int i = 1; i < (argc - 1); ++i)
    {
        input_file_paths.push_back(argv[i]);
    }

    std::string output_file_path = argv[argc - 1];

    std::vector<packet_data> input_packets;
    std::vector<packet_data> output_packets;

    for (auto& file_path : input_file_paths)
    {
        std::vector<packet_data> packets = parse_json(file_path);
        input_packets.insert(input_packets.end(), packets.begin(), packets.end());
    }

    output_packets = process_packets(input_packets);

    write_json_basic(output_file_path, output_packets);

    return 0;
}


