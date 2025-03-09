// **************************************************************** //
// libaprstrack - APRS tracking library                             //
// Version 0.1.0                                                    //
// https://github.com/iontodirel/libaprstrack                       //
// Copyright (c) 2025 Ion Todirel                                   //
// **************************************************************** //
//
// tests.hpp
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

#pragma once

#include <nlohmann/json.hpp>

#include <boost/beast/core/detail/base64.hpp>

#include "../libaprstrack.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>

#ifdef _MSC_VER
#include <intrin.h>
#endif

using namespace aprs::track;
using namespace aprs::track::detail;

struct packet_data
{
    std::string packet;
    std::string data;
    std::string from;
    int from_ssid = 0;
    std::string to;
    int to_ssid = 0;
    std::string path;
    double lat = 0.0;
    double lon = 0.0;
    std::string lat_str;
    std::string lon_str;
    std::string messaging_str;
    bool messaging = false;
    std::string symbol_code;
    std::string symbol_table;
    double alt = 0.0;
    std::string alt_str;
    double course = 0.0;
    double speed = 0.0;
    std::string course_str;
    std::string speed_str;
    enum mic_e_status mic_e_status = mic_e_status::unknown;
    enum packet_type packet_type = packet_type::mic_e;
    std::string comment;
    bool has_course_speed = false;
    bool has_altitude = false;
    int ambiguity = 0;
};

struct packet_data_generated
{
    std::string packet;
    double lat = 0.0;
    double lon = 0.0;
    double alt = 0.0;
    double course = 0.0;
    double speed = 0.0;
};

struct expected_packet
{
    std::string actual_packet;
    std::string expected_packet;
};

inline double kmh_to_knots(double kmh)
{
    return kmh * 0.539957;
}

inline void nop()
{
#if defined(_MSC_VER)
    __nop();
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("nop");
#else
    // Fallback for other compilers
    std::this_thread::yield();
#endif
}

inline std::string base64_encode(const std::string& input)
{
    std::string encoded;
    encoded.resize(boost::beast::detail::base64::encoded_size(input.size()));
    auto result = boost::beast::detail::base64::encode(encoded.data(), input.data(), input.size());
    encoded.resize(result);
    return encoded;
}

inline std::string base64_decode(const std::string& input)
{
    if (input.empty() || input.size() % 4 != 0)
    {
        return "";
    }

    std::string decoded;
    decoded.resize(boost::beast::detail::base64::decoded_size(input.size()));
    auto result = boost::beast::detail::base64::decode(decoded.data(), input.data(), input.size());
    decoded.resize(result.first);
    return decoded;
}

inline std::string escape_string(const std::string& input)
{
    std::string result;
    result.reserve(input.size() * 2);

    for (unsigned char c : input)
    {
        switch (c)
        {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\v': result += "\\v"; break;
            case '\f': result += "\\f"; break;
            case '\b': result += "\\b"; break;
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            default:
                if (std::isprint(c))
                {
                    result.push_back(c);
                }
                else
                {
                    char buffer[8];
                    std::snprintf(buffer, sizeof(buffer), "\\x%02X", c);
                    result.append(buffer);
                }
                break;
        }
    }

    return result;
}

inline packet_type parse_packet_type(const std::string& type_str)
{
    if (type_str == "mic_e")
    {
        return packet_type::mic_e;
    }
    else if (type_str == "position")
    {
        return packet_type::position;
    }
    else if (type_str == "position_compressed")
    {
        return packet_type::position_compressed;
    }
    else if (type_str == "position_with_timestamp")
    {
        return packet_type::position_with_timestamp;
    }
    else if (type_str == "position_with_timestamp_utc")
    {
        return packet_type::position_with_timestamp_utc;
    }
    else if (type_str == "position_with_timestamp_utc_hms")
    {
        return packet_type::position_with_timestamp_utc_hms;
    }
    else
    {
        return packet_type::position;
    }
}

inline mic_e_status parse_mic_e_status(const std::string& status_str)
{
    if (status_str == "off_duty")
    {
        return mic_e_status::off_duty;
    }
    else if (status_str == "en_route")
    {
        return mic_e_status::en_route;
    }
    else if (status_str == "in_service")
    {
        return mic_e_status::in_service;
    }
    else if (status_str == "returning")
    {
        return mic_e_status::returning;
    }
    else if (status_str == "commited")
    {
        return mic_e_status::commited;
    }
    else if (status_str == "special")
    {
        return mic_e_status::special;
    }
    else if (status_str == "priority")
    {
        return mic_e_status::priority;
    }
    else if (status_str == "emergency")
    {
        return mic_e_status::emergency;
    }
    else if (status_str == "custom0")
    {
        return mic_e_status::custom0;
    }
    else if (status_str == "custom1")
    {
        return mic_e_status::custom1;
    }
    else if (status_str == "custom2")
    {
        return mic_e_status::custom2;
    }
    else if (status_str == "custom3")
    {
        return mic_e_status::custom3;
    }
    else if (status_str == "custom4")
    {
        return mic_e_status::custom4;
    }
    else if (status_str == "custom5")
    {
        return mic_e_status::custom5;
    }
    else if (status_str == "custom6")
    {
        return mic_e_status::custom6;
    }
    else
    {
        return mic_e_status::unknown;
    }
}

inline std::vector<packet_data> parse_json(const std::string& file_path)
{
    std::vector<packet_data> packets;
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return packets;
    }

    nlohmann::json json_data;

    file >> json_data;

    for (const auto& item : json_data)
    {
        packet_data packet;
        packet.packet = base64_decode(item.value("packet_base64", ""));
        packet.data = base64_decode(item.value("data_base64", ""));
        packet.from = item.value("from", "");
        packet.to = item.value("to", "");
        packet.path = item.value("path", "");
        packet.lat_str = item.value("lat", "");
        packet.lon_str = item.value("lon", "");
        packet.messaging_str = item.value("messaging", "");
        packet.symbol_code = item.value("symbol_code", "");
        packet.symbol_table = item.value("symbol_table", "");
        packet.course_str = item.value("course", "");
        packet.speed_str = item.value("speed", "");
        packet.alt_str = item.value("alt", "");
        packet.mic_e_status = parse_mic_e_status(item.value("mic_e_status", ""));
        packet.packet_type = parse_packet_type(item.value("packet_type", ""));
        packet.comment = base64_decode(item.value("comment_base64", ""));

        try
        {
            packet.lat = item.value("lat", "").empty() ? 0.0 : std::stod(item.value("lat", "0.0"));
            packet.lon = item.value("lon", "").empty() ? 0.0 : std::stod(item.value("lon", "0.0"));
            packet.alt = item.value("alt", "").empty() ? 0.0 : std::stod(item.value("alt", "0.0"));
            packet.speed = item.value("speed", "").empty() ? 0.0 : std::stod(item.value("speed", "0.0"));
            packet.course = item.value("course", "").empty() ? 0.0 : std::stod(item.value("course", "0.0"));
            packet.ambiguity = item.value("ambiguity", "").empty() ? 0 : std::stoi(item.value("ambiguity", "0"));
            packet.from_ssid = item.value("from_ssid", "").empty() ? 0 : std::stoi(item.value("from_ssid", "0"));
            packet.to_ssid = item.value("to_ssid", "").empty() ? 0 : std::stoi(item.value("to_ssid", "0"));
        }
        catch (const std::invalid_argument&)
        {
            std::cerr << "Invalid number format in JSON file: " << file_path << std::endl;
        }
        catch (const std::out_of_range&)
        {
            std::cerr << "Number out of range in JSON file: " << file_path << std::endl;
        }

        packets.push_back(packet);
    }

    return packets;
}

inline std::vector<packet_data> parse_json_basic(const std::string& file_path)
{
    std::vector<packet_data> packets;
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return packets;
    }

    nlohmann::json json_data;

    file >> json_data;

    for (const auto& item : json_data)
    {
        packet_data packet;
        packet.packet = base64_decode(item.value("packet_base64", ""));
        packet.data = base64_decode(item.value("data_base64", ""));
        packet.from = item.value("from", "");
        packet.to = item.value("to", "");
        packet.path = item.value("path", "");
        packet.messaging_str = item.value("messaging", "");
        packet.symbol_code = item.value("symbol_code", "");
        packet.symbol_table = item.value("symbol_table", "");
        packet.mic_e_status = parse_mic_e_status(item.value("mic_e_status", ""));
        packet.packet_type = parse_packet_type(item.value("packet_type", ""));
        packet.has_altitude = item.value("has_altitude", "false") == "true";
        packet.has_course_speed = item.value("has_course_speed", "false") == "true";

        try
        {
            packet.lat = item.value("lat", "").empty() ? 0.0 : std::stod(item.value("lat", "0.0"));
            packet.lon = item.value("lon", "").empty() ? 0.0 : std::stod(item.value("lon", "0.0"));
            packet.alt = item.value("alt", "").empty() ? 0.0 : std::stod(item.value("alt", "0.0"));
            packet.speed = item.value("speed", "").empty() ? 0.0 : std::stod(item.value("speed", "0.0"));
            packet.course = item.value("course", "").empty() ? 0.0 : std::stod(item.value("course", "0.0"));
            packet.ambiguity = item.value("ambiguity", "").empty() ? 0 : std::stoi(item.value("ambiguity", "0"));
            packet.from_ssid = item.value("from_ssid", "").empty() ? 0 : std::stoi(item.value("from_ssid", "0"));
            packet.to_ssid = item.value("to_ssid", "").empty() ? 0 : std::stoi(item.value("to_ssid", "0"));
        }
        catch (const std::invalid_argument&)
        {
            std::cerr << "Invalid number format in JSON file: " << file_path << std::endl;
        }
        catch (const std::out_of_range&)
        {
            std::cerr << "Number out of range in JSON file: " << file_path << std::endl;
        }

        packets.push_back(packet);
    }

    return packets;
}

inline void write_json(const std::string& file_path, const std::vector<packet_data>& packets)
{
    nlohmann::ordered_json json_data;

    int i = 0;

    for (const auto& packet : packets)
    {
        nlohmann::ordered_json item;
        item["index"] = std::to_string(i);
        item["packet_base64"] = base64_encode(packet.packet);
        item["data_base64"] = base64_encode(packet.data);
        item["from"] = packet.from;
        item["from_ssid"] = packet.from_ssid;
        item["to"] = packet.to;
        item["to_ssid"] = packet.to_ssid;
        item["path"] = packet.path;
        item["lat"] = packet.lat_str;
        item["lon"] = packet.lon_str;
        item["messaging"] = packet.messaging_str;
        item["symbol_code"] = packet.symbol_code;
        item["symbol_table"] = packet.symbol_table;
        item["course"] = packet.course_str;
        item["speed"] = packet.speed_str;
        item["alt"] = packet.alt_str;
        item["mic_e_status"] = to_string(packet.mic_e_status);
        item["packet_type"] = to_string(packet.packet_type);
        item["comment_base64"] = base64_encode(packet.comment);
        json_data.push_back(item);
        i++;
    }

    std::ofstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return;
    }

    file << json_data.dump(4);
}

inline void write_json_basic(const std::string& file_path, const std::vector<packet_data>& packets)
{
    nlohmann::ordered_json json_data;

    int i = 0;

    for (const auto& packet : packets)
    {
        nlohmann::ordered_json item;
        item["index"] = std::to_string(i);
        item["packet"] = escape_string(packet.packet);
        item["packet_base64"] = base64_encode(packet.packet);
        item["from"] = packet.from;
        item["from_ssid"] = std::to_string(packet.from_ssid);
        item["to"] = packet.to;
        item["to_ssid"] = std::to_string(packet.to_ssid);
        item["path"] = packet.path;
        item["lat"] = std::to_string(packet.lat);
        item["lon"] = std::to_string(packet.lon);
        item["messaging"] = packet.messaging_str;
        item["symbol_code"] = packet.symbol_code;
        item["symbol_table"] = packet.symbol_table;
        item["course"] = std::to_string(packet.course);
        item["speed"] = std::to_string(packet.speed);
        item["alt"] = std::to_string(packet.alt);
        item["mic_e_status"] = to_string(packet.mic_e_status);
        item["packet_type"] = to_string(packet.packet_type);
        item["ambiguity"] = std::to_string(packet.ambiguity);
        item["has_course_speed"] = packet.has_course_speed ? "true" : "false";
        item["has_altitude"] = packet.has_altitude ? "true" : "false";
        json_data.push_back(item);
        i++;
    }

    std::ofstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return;
    }

    file << json_data.dump(4);
}

inline void append_to_json(const std::string& input_file_path, const std::string& output_file_path, const std::vector<packet_data_generated>& generated_packets)
{
    std::ifstream input_file(input_file_path);
    if (!input_file.is_open())
    {
        std::cerr << "Could not open input file: " << input_file_path << std::endl;
        return;
    }

    nlohmann::ordered_json json_data;
    input_file >> json_data;
    input_file.close();

    if (!json_data.is_array() || json_data.empty())
    {
        std::cerr << "Input JSON is not an array or is empty" << std::endl;
        return;
    }

    if (json_data.size() != generated_packets.size())
    {
        std::cerr << "Mismatch between input JSON size and generated packets size" << std::endl;
        return;
    }

    for (size_t i = 0; i < generated_packets.size(); ++i)
    {
        json_data[i]["encoded"] = {
            {"packet", generated_packets[i].packet},
            {"packet_base64", base64_encode(generated_packets[i].packet)},
            {"lat", std::to_string(generated_packets[i].lat)},
            {"lon", std::to_string(generated_packets[i].lon)},
            {"alt", std::to_string(generated_packets[i].alt)},
            {"course", std::to_string(generated_packets[i].course)},
            {"speed", std::to_string(generated_packets[i].speed)}
        };
    }

    std::ofstream output_file(output_file_path);
    if (!output_file.is_open())
    {
        std::cerr << "Could not open output file: " << output_file_path << std::endl;
        return;
    }

    output_file << json_data.dump(4); // Pretty print with 4-space indentation
    output_file.close();

    std::cout << "JSON file '"
              << output_file_path << "' has been updated successfully with "
              << generated_packets.size() << " generated packets." << std::endl;
}

inline void write_json(const std::string& file_path, const std::vector<expected_packet>& packets)
{
    nlohmann::ordered_json json_data;
    for (const auto& packet : packets)
    {
        nlohmann::ordered_json item;
        item["actual_packet"] = packet.actual_packet;
        item["expected_packet"] = packet.expected_packet;
        json_data.push_back(item);
    }
    std::ofstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return;
    }

    file << json_data.dump(4);
}

std::vector<packet_data> process_packets(const std::vector<packet_data>& packets)
{
    int failure_count = 0;
    int success_count = 0;
    int skipped_count = 0;
    int total_count = (int)packets.size();
    std::vector<packet_data> success_packets;

    for (const auto& packet : packets)
    {
        if (packet.ambiguity > 0)
        {
            skipped_count++;
            continue;
        }

        std::string expected_packet_string = packet.packet;
        std::string packet_string;
        std::string actual_packet_string;

        if (packet.packet_type == packet_type::mic_e)
        {
            if (!packet.alt_str.empty())
            {
                packet_string = encode_mic_e_packet_no_message(
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
                packet_string = encode_mic_e_packet_no_message(
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

            actual_packet_string = packet_string;

            std::string course_speed_alt = encode_mic_e_course_speed_alternate(packet.course, kmh_to_knots(packet.speed));
            size_t course_speed_alt_index = packet.from.size() + 1 + packet.to.size() + 1 + packet.path.size() + 1 + 3 + 1;
            std::string expected_course_speed = expected_packet_string.substr(course_speed_alt_index, 3);
            std::string actual_course_speed = packet_string.substr(course_speed_alt_index, 3);

            if (expected_course_speed != actual_course_speed && expected_course_speed == course_speed_alt)
            {
                packet_string.replace(course_speed_alt_index, 3, course_speed_alt);
            }

            if (!packet.alt_str.empty() && packet.data[9] != '"')
            {
                size_t vendor_index = packet.from.size() + 1 + packet.to.size() + 1 + packet.path.size() + 1 + 9;
                packet_string.insert(vendor_index, 1, packet.data[9]);
            }

            if (packet.to_ssid > 0)
            {
                size_t to_end_index = packet.from.size() + 1 + packet.to.size();
                std::string ssid_string = "-" + std::to_string(packet.to_ssid);
                to_end_index -= ssid_string.size();
                packet_string.insert(to_end_index, ssid_string);
            }
        }
        else if (packet.packet_type == packet_type::position)
        {
            if (packet.alt_str.empty() && packet.course_str.empty() && packet.speed_str.empty())
            {
                packet_string = encode_position_packet_no_timestamp_no_message(
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
                packet_string = encode_position_packet_no_timestamp_no_message(
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
                packet_string = encode_position_packet_no_timestamp_no_message(
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
                packet_string = encode_position_packet_no_timestamp_no_message(
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

            actual_packet_string = packet_string;
        }

        bool result = expected_packet_string.starts_with(packet_string);

        if (result)
        {
            packet_data generated_packet;
            generated_packet.packet = actual_packet_string;
            generated_packet.packet_type = packet.packet_type;
            generated_packet.from = packet.from;
            generated_packet.from_ssid = packet.from_ssid;
            generated_packet.to = packet.to;
            generated_packet.to_ssid = packet.to_ssid;
            generated_packet.path = packet.path;
            generated_packet.messaging_str = packet.messaging_str;
            generated_packet.lat = packet.lat;
            generated_packet.lon = packet.lon;
            generated_packet.mic_e_status = packet.mic_e_status;
            generated_packet.course = packet.course;
            generated_packet.speed = packet.speed;
            generated_packet.symbol_table = packet.symbol_table;
            generated_packet.symbol_code = packet.symbol_code;
            generated_packet.alt = packet.alt;
            generated_packet.messaging = (packet.messaging_str == "true");
            generated_packet.has_altitude = !packet.alt_str.empty();
            generated_packet.has_course_speed = !packet.course_str.empty() || !packet.speed_str.empty();
            success_packets.push_back(generated_packet);
            success_count++;
        }
        else
        {
#ifdef LIBAPRSTRACK_PRINT_FAILED_PACKETS
            printf("expected: %d %s\n", (int)expected_packet_string.size(), expected_packet_string.c_str());
            printf("  actual: %d %s\n", (int)actual_packet_string.size(), actual_packet_string.c_str());
            printf("expected hex: ");
            for (char c : expected_packet_string)
            {
                printf("%02x ", static_cast<unsigned char>(c));
            }
            printf("\n");
            printf("  actual hex: ");
            for (char c : actual_packet_string)
            {
                printf("%02x ", static_cast<unsigned char>(c));
            }
            printf("\n");
            printf("-----------------------------------------------\n");
#endif
            failure_count++;
        }
    }

    printf("total count %d, success count %d, failure count %d, skipped count %d\n", total_count, success_count, failure_count, skipped_count);

    return success_packets;
}