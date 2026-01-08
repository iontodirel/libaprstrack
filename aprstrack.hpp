// **************************************************************** //
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
// This library is reusing code from position-lib: https://github.com/iontodirel/position-lib
// Copyright (C) 2023 Ion Todirel
//
// This library is reusing code from libaprs: https://github.com/iontodirel/libaprs
// Copyright (C) 2023 Ion Todirel

// References:
//
//   - APRS Specification: https://www.aprs.org/doc/APRS101.PDF (APRS 1.0)
//   - UTF-8 Specification: https://www.aprs.org/aprs12/utf-8.txt
//   - Mic-E examples: https://www.aprs.org/aprs12/mic-e-examples.txt
//   - Mic-E types: https://www.aprs.org/aprs12/mic-e-types.txt
//   - Understanding APRS Packets: https://raw.githubusercontent.com/wb2osz/aprsspec/main/Understanding-APRS-Packets.pdf

#pragma once

#include <string>
#include <optional>
#include <tuple>
#include <chrono>
#include <iterator>
#include <ranges>
#include <cmath>
#include <algorithm>
#include <array>

#ifndef APRS_TRACK_NAMESPACE
#define APRS_TRACK_NAMESPACE aprs::track
#endif
#ifndef APRS_TRACK_NAMESPACE_BEGIN
#define APRS_TRACK_NAMESPACE_BEGIN namespace APRS_TRACK_NAMESPACE {
#endif
#ifndef APRS_TRACK_DETAIL_NAMESPACE
#define APRS_TRACK_DETAIL_NAMESPACE detail
#endif
#ifndef APRS_TRACK_DETAIL_NAMESPACE_BEGIN
#define APRS_TRACK_DETAIL_NAMESPACE_BEGIN namespace APRS_TRACK_DETAIL_NAMESPACE {
#endif
#ifndef APRS_TRACK_NAMESPACE_END
#define APRS_TRACK_NAMESPACE_END }
#endif
#ifndef APRS_TRACK_DETAIL_NAMESPACE_END
#define APRS_TRACK_DETAIL_NAMESPACE_END }
#endif
#ifndef APRS_TRACK_NAMESPACE_USE
#define APRS_TRACK_NAMESPACE_USE using namespace APRS_TRACK_NAMESPACE;
#endif
#ifndef APRS_TRACK_DETAIL_NAMESPACE_USE
#define APRS_TRACK_DETAIL_NAMESPACE_USE using namespace APRS_TRACK_DETAIL_NAMESPACE;
#endif
#ifndef APRS_TRACK_DETAIL_NAMESPACE_REFERENCE
#define APRS_TRACK_DETAIL_NAMESPACE_REFERENCE APRS_TRACK_DETAIL_NAMESPACE ::
#endif
#ifndef APRS_TRACK_INLINE
#define APRS_TRACK_INLINE inline
#endif
#ifndef APRS_TRACK_INLINE_NO_DISABLE
#define APRS_TRACK_INLINE_NO_DISABLE inline
#endif
#ifdef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY
// Intentionally left empty
#endif
#ifndef APRS_TRACK_SMART_BEACONING_DEBUG
#define APRS_TRACK_SMART_BEACONING_DEBUG(...)
#endif

// **************************************************************** //
//                                                                  //
//                                                                  //
// private declarations                                             //
//                                                                  //
//                                                                  //
// **************************************************************** //

APRS_TRACK_NAMESPACE_BEGIN

struct tracker; // forward declaration
enum class mic_e_status; // forward declaration

APRS_TRACK_DETAIL_NAMESPACE_BEGIN

template<typename T>
concept has_speed = requires(T t)
{
    // expected in m/s
    { t.speed } -> std::convertible_to<double>;
};

template<typename T>
concept has_track = requires(T t)
{
    // expected in degrees
    { t.track } -> std::convertible_to<double>;
};

template<typename T>
concept has_day_hour_minute_seconds = requires(T t)
{
    { t.second } -> std::convertible_to<int>;
    { t.minute } -> std::convertible_to<int>;
    { t.day } -> std::convertible_to<int>;
    { t.hour } -> std::convertible_to<int>;
};

template<typename T>
concept has_altitude = requires(T t)
{
    // expected in meters
    { t.alt } -> std::convertible_to<double>;
};

struct data
{
    double lat = 0.0;
    double lon = 0.0;
    std::optional<double> speed_knots;
    std::optional<double> track_degrees;
    std::optional<double> alt_feet;
    int day = 0;
    int hour = 0; // 24 hour format, 0-23
    int minute = 0;
    int second = 0;
};

template<typename Container = std::string> Container encode_position_packet_no_timestamp_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_no_timestamp_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_timestamp_dhm_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_timestamp_dhm_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_utc_timestamp_hms_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_utc_timestamp_hms_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_utc_timestamp_dhm_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_with_utc_timestamp_dhm_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_compressed_no_timestamp_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_position_packet_compressed_no_timestamp_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_mic_e_packet_no_message_to(const tracker& t, const data& d);
template<typename Container = std::string> Container encode_mic_e_packet_to(const tracker& t, const data& d);

bool smart_beaconing_test(int speed, int prev_course, int course, int low_speed, int high_speed, int slow_interval_seconds, int fast_interval_seconds, int min_turn_degrees, int turn_interval_seconds, int turn_slope, int last_update_seconds);

double meters_to_feet(double meters);
double mps_to_knots(double mps);
double knots_to_mps(double knots);

APRS_TRACK_NAMESPACE_END

APRS_TRACK_DETAIL_NAMESPACE_END

// **************************************************************** //
//                                                                  //
//                                                                  //
// public declarations                                              //
//                                                                  //
//                                                                  //
// **************************************************************** //

APRS_TRACK_NAMESPACE_BEGIN

template<typename T>
concept Position = requires(T p)
{
    { p.lat } -> std::convertible_to<double>;
    { p.lon } -> std::convertible_to<double>;
};

enum class packet_type
{
    mic_e,
    position,
    position_compressed,
    position_with_timestamp,
    position_with_timestamp_utc,
    position_with_timestamp_utc_hms,
    position_compressed_with_timestamp,
    position_compressed_with_timestamp_utc,
    position_compressed_with_timestamp_utc_hms,
};

enum class mic_e_status
{
    off_duty,   // 1 1 1
    en_route,   // 1 1 0
    in_service, // 1 0 1
    returning,  // 1 0 0
    commited,   // 0 1 1
    special,    // 0 1 0
    priority,   // 0 0 1
    emergency,  // 0 0 0
    custom0,    // 1 1 1
    custom1,    // 1 1 0
    custom2,    // 1 0 1
    custom3,    // 1 0 0
    custom4,    // 0 1 1
    custom5,    // 0 1 0
    custom6,    // 0 0 1
    unknown
};

enum class compression_type
{
    old_other_compressed,           // 0 00 000
    old_other_tnc_b_text,           // 0 00 001
    old_other_software,             // 0 00 010
    old_other_tbd_1,                // 0 00 011
    old_other_kpc3,                 // 0 00 100
    old_other_pico,                 // 0 00 101
    old_other_other_tracker,        // 0 00 110
    old_other_digipeater,           // 0 00 111

    old_gll_compressed,             // 0 01 000
    old_gll_tnc_b_text,             // 0 01 001
    old_gll_software,               // 0 01 010
    old_gll_tbd_1,                  // 0 01 011
    old_gll_kpc3,                   // 0 01 100
    old_gll_pico,                   // 0 01 101
    old_gll_other_tracker,          // 0 01 110
    old_gll_digipeater,             // 0 01 111

    old_gga_compressed,             // 0 10 000
    old_gga_tnc_b_text,             // 0 10 001
    old_gga_software,               // 0 10 010
    old_gga_tbd_1,                  // 0 10 011
    old_gga_kpc3,                   // 0 10 100
    old_gga_pico,                   // 0 10 101
    old_gga_other_tracker,          // 0 10 110
    old_gga_digipeater,             // 0 10 111

    old_rmc_compressed,             // 0 11 000
    old_rmc_tnc_b_text,             // 0 11 001
    old_rmc_software,               // 0 11 010
    old_rmc_tbd_1,                  // 0 11 011
    old_rmc_kpc3,                   // 0 11 100
    old_rmc_pico,                   // 0 11 101
    old_rmc_other_tracker,          // 0 11 110
    old_rmc_digipeater,             // 0 11 111

    current_other_compressed,       // 1 00 000
    current_other_tnc_b_text,       // 1 00 001
    current_other_software,         // 1 00 010
    current_other_tbd_1,            // 1 00 011
    current_other_kpc3,             // 1 00 100
    current_other_pico,             // 1 00 101
    current_other_other_tracker,    // 1 00 110
    current_other_digipeater,       // 1 00 111

    current_gll_compressed,         // 1 01 000
    current_gll_tnc_b_text,         // 1 01 001
    current_gll_software,           // 1 01 010
    current_gll_tbd_1,              // 1 01 011
    current_gll_kpc3,               // 1 01 100
    current_gll_pico,               // 1 01 101
    current_gll_other_tracker,      // 1 01 110
    current_gll_digipeater,         // 1 01 111

    current_gga_compressed,         // 1 10 000
    current_gga_tnc_b_text,         // 1 10 001
    current_gga_software,           // 1 10 010
    current_gga_tbd_1,              // 1 10 011
    current_gga_kpc3,               // 1 10 100
    current_gga_pico,               // 1 10 101
    current_gga_other_tracker,      // 1 10 110
    current_gga_digipeater,         // 1 10 111

    current_rmc_compressed,         // 1 11 000
    current_rmc_tnc_b_text,         // 1 11 001
    current_rmc_software,           // 1 11 010
    current_rmc_tbd_1,              // 1 11 011
    current_rmc_kpc3,               // 1 11 100
    current_rmc_pico,               // 1 11 101
    current_rmc_other_tracker,      // 1 11 110
    current_rmc_digipeater          // 1 11 111
};

enum class device_id
{
    original,
    unknown,
    th_d7a,
    tm_d700,
    tm_d710,
    th_d72a,
    th_d74a,
    with_messaging,
    no_messaging,
    vx_8,
    ftm_350,
    vx_8g,
    ft_1d,
    ftm_400dr,
};

enum class algorithm
{
    smart_beaconing,
    periodic,
    none,
};

struct tracker
{
    void algorithm(enum algorithm a);
    enum algorithm algorithm() const;

    void symbol_code(char);
    void symbol_table(char);
    char symbol_code() const;
    char symbol_table() const;

    void from(std::string_view f);
    void to(std::string_view t);
    void path(std::string_view p);
    std::string_view from() const;
    std::string_view to() const;
    std::string_view path() const;

    void ambiguity(int a);
    int ambiguity() const;

    void messaging(bool m);
    bool messaging() const;

    void mic_e_status(enum mic_e_status s);
    enum mic_e_status mic_e_status() const;

    void low_speed(double speed_mps);
    double low_speed() const;
    void high_speed(double speed_mps);
    double high_speed() const;
    void slow_rate(int seconds);
    int slow_rate() const;
    void fast_rate(int seconds);
    int fast_rate() const;
    void turn_time(int seconds);
    int turn_time() const;
    void turn_angle(int degrees);
    int turn_angle() const;
    void turn_slope(int value);
    int turn_slope() const;

    template<typename CharType, typename Traits>
    void message(const std::basic_string_view<CharType, Traits>& m);

    template<typename CharType>
    void message(const CharType* m);

    template<typename CharType>
    void message(const CharType* m, size_t count);

    template<std::ranges::input_range InputRange>
    void message(InputRange&& input_range);

    template<std::input_iterator InputIterator>
    void message(InputIterator begin, InputIterator end);

    template<std::output_iterator<unsigned char> OutputIterator>
    OutputIterator message(OutputIterator output) const;

    std::string message() const;
    std::u8string u8message() const;

    template<class Rep, class Period>
    void interval(std::chrono::duration<Rep, Period> interval);

    void interval_seconds(int interval_seconds);

    template<Position T>
    void position(const T& p);

    void position(double lat, double lon);
    void position(double lat, double lon, double speed, double track_degrees);
    void position(double lat, double lon, double speed, double track_degrees, double alt_meters);
    void position(double lat, double lon, double speed, double track_degrees, double alt_meters, int day, int hour, int minute, int second);

    void time(int day, int hour, int minute, int second);
    void time(int hour, int minute, int second);
    void time(int minute, int second);

    void speed(double speed_mps);
    void alt(double alt_meters);
    void track(double track_degrees);

    void update();
    bool updated() const;

    template<typename Container = std::string>
    Container packet_string_no_message_to(packet_type p) const;

    template<typename Container = std::string>
    Container packet_string_to(packet_type p) const;

    template<std::output_iterator<unsigned char> OutputIterator>
    OutputIterator packet(packet_type p, OutputIterator output) const;

    template<std::ranges::output_range<unsigned char> OutputRange>
    void packet(packet_type p, OutputRange&& output_range) const;

    bool smart_beaconing_test();

private:
    APRS_TRACK_DETAIL_NAMESPACE_REFERENCE data data_;
    std::vector<APRS_TRACK_DETAIL_NAMESPACE_REFERENCE data> data_list_;
    std::array<char, 10> from_{};
    std::array<char, 10> to_{};
    std::array<char, 88> path_{};
    enum algorithm algorithm_ = algorithm::none;
    std::array<unsigned char, 256> message_data_{};
    size_t message_data_length_ = 0;
    size_t message_data_size_ = 0;
    char symbol_code_ = '>';
    char symbol_table_ = '/';
    unsigned int interval_seconds_ = 30;
    unsigned int last_update_seconds = 0;
    std::optional<double> previous_track_degrees_;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time;
    double low_speed_knots_ = 4.0;  // 5 mph -> 4 knots
    double high_speed_knots_ = 52.0; // 60 mph -> 52 knots
    int slow_rate_ = 60; // 60 seconds (1 minute)
    int fast_rate_ = 30; // 30 seconds
    int turn_time_ = 15; // 15 seconds
    int turn_angle_ = 15; // 15 degrees
    int turn_slope_ = 255; // 255 (no slope)
    int ambiguity_ = 0;
    bool aprs_messaging_ = false;
    bool updated_ = false;
    enum mic_e_status mic_e_status_ = mic_e_status::in_service;
};

std::string to_string(mic_e_status status);

std::string to_string(packet_type type);

APRS_TRACK_NAMESPACE_END

// **************************************************************** //
//                                                                  //
//                                                                  //
// public definitions                                               //
//                                                                  //
//                                                                  //
// **************************************************************** //

APRS_TRACK_NAMESPACE_BEGIN

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::string to_string(mic_e_status status)
{
    switch (status)
    {
        case mic_e_status::off_duty: return "off_duty";
        case mic_e_status::en_route: return "en_route";
        case mic_e_status::in_service: return "in_service";
        case mic_e_status::returning: return "returning";
        case mic_e_status::commited: return "commited";
        case mic_e_status::special: return "special";
        case mic_e_status::priority: return "priority";
        case mic_e_status::emergency: return "emergency";
        case mic_e_status::custom0: return "custom0";
        case mic_e_status::custom1: return "custom1";
        case mic_e_status::custom2: return "custom2";
        case mic_e_status::custom3: return "custom3";
        case mic_e_status::custom4: return "custom4";
        case mic_e_status::custom5: return "custom5";
        case mic_e_status::custom6: return "custom6";
        default:
            break;
    }
    return "unknown";
}

APRS_TRACK_INLINE std::string to_string(packet_type type)
{
    switch (type)
    {
        case packet_type::mic_e: return "mic_e";
        case packet_type::position: return "position";
        case packet_type::position_compressed: return "position_compressed";
        case packet_type::position_with_timestamp: return "position_with_timestamp";
        case packet_type::position_with_timestamp_utc: return "position_with_timestamp_utc";
        case packet_type::position_with_timestamp_utc_hms: return "position_with_timestamp_utc_hms";
        case packet_type::position_compressed_with_timestamp: return "position_compressed_with_timestamp";
        case packet_type::position_compressed_with_timestamp_utc: return "position_compressed_with_timestamp_utc";
        case packet_type::position_compressed_with_timestamp_utc_hms: return "position_compressed_with_timestamp_utc_hms";
        default:
            break;
    }
    return "";
}

APRS_TRACK_INLINE void tracker::algorithm(enum algorithm a)
{
    algorithm_ = a;
}

APRS_TRACK_INLINE enum algorithm tracker::algorithm() const
{
    return algorithm_;
}

APRS_TRACK_INLINE void tracker::symbol_code(char c)
{
    symbol_code_ = c;
}

APRS_TRACK_INLINE char tracker::symbol_code() const
{
    return symbol_code_;
}

APRS_TRACK_INLINE void tracker::symbol_table(char t)
{
    symbol_table_ = t;
}

APRS_TRACK_INLINE char tracker::symbol_table() const
{
    return symbol_table_;
}

APRS_TRACK_INLINE void tracker::from(std::string_view f)
{
    auto len = (std::min)(f.size(), from_.size() - 1);
    std::copy_n(f.data(), len, from_.data());
    from_[len] = '\0';
}

APRS_TRACK_INLINE std::string_view tracker::from() const
{
    return from_.data();
}

APRS_TRACK_INLINE void tracker::to(std::string_view t)
{
    auto len = (std::min)(t.size(), to_.size() - 1);
    std::copy_n(t.data(), len, to_.data());
    to_[len] = '\0';
}

APRS_TRACK_INLINE std::string_view tracker::to() const
{
    return to_.data();
}

APRS_TRACK_INLINE void tracker::path(std::string_view p)
{
    auto len = (std::min)(p.size(), path_.size() - 1);
    std::copy_n(p.data(), len, path_.data());
    path_[len] = '\0';
}

APRS_TRACK_INLINE std::string_view tracker::path() const
{
    return path_.data();
}

APRS_TRACK_INLINE void tracker::ambiguity(int a)
{
    ambiguity_ = a;
}

APRS_TRACK_INLINE int tracker::ambiguity() const
{
    return ambiguity_;
}

APRS_TRACK_INLINE void tracker::messaging(bool m)
{
    aprs_messaging_ = m;
}

APRS_TRACK_INLINE bool tracker::messaging() const
{
    return aprs_messaging_;
}

APRS_TRACK_INLINE void tracker::mic_e_status(enum mic_e_status s)
{
    mic_e_status_ = s;
}

APRS_TRACK_INLINE enum mic_e_status tracker::mic_e_status() const
{
    return mic_e_status_;
}

APRS_TRACK_INLINE void tracker::low_speed(double speed_mps)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    low_speed_knots_ = mps_to_knots(speed_mps);
}

APRS_TRACK_INLINE double tracker::low_speed() const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    return knots_to_mps(low_speed_knots_);
}

APRS_TRACK_INLINE void tracker::high_speed(double speed_mps)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    high_speed_knots_ = mps_to_knots(speed_mps);
}

APRS_TRACK_INLINE double tracker::high_speed() const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    return knots_to_mps(high_speed_knots_);
}

APRS_TRACK_INLINE void tracker::slow_rate(int seconds)
{
    slow_rate_ = seconds;
}

APRS_TRACK_INLINE int tracker::slow_rate() const
{
    return slow_rate_;
}

APRS_TRACK_INLINE void tracker::fast_rate(int seconds)
{
    fast_rate_ = seconds;
}

APRS_TRACK_INLINE int tracker::fast_rate() const
{
    return fast_rate_;
}

APRS_TRACK_INLINE void tracker::turn_time(int seconds)
{
    turn_time_ = seconds;
}

APRS_TRACK_INLINE int tracker::turn_time() const
{
    return turn_time_;
}

APRS_TRACK_INLINE void tracker::turn_angle(int degrees)
{
    turn_angle_ = degrees;
}

APRS_TRACK_INLINE int tracker::turn_angle() const
{
    return turn_angle_;
}

APRS_TRACK_INLINE void tracker::turn_slope(int value)
{
    turn_slope_ = value;
}

APRS_TRACK_INLINE int tracker::turn_slope() const
{
    return turn_slope_;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

template<typename CharType, typename Traits>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const std::basic_string_view<CharType, Traits>& m)
{
    message_data_length_ = m.size();
    const unsigned char* data = reinterpret_cast<const unsigned char*>(m.data());
    size_t size = (std::min)(m.size() * sizeof(CharType), message_data_.size());
    std::copy_n(data, size, message_data_.begin());
    message_data_size_ = size;
}

template<typename CharType>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const CharType* m)
{
    message(std::basic_string_view<CharType>(m));
}

template<typename CharType>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const CharType* m, size_t count)
{
    message(std::basic_string_view<CharType>(m, count));
}

template<std::ranges::input_range InputRange>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(InputRange&& input_range)
{
    message(std::ranges::begin(input_range), std::ranges::end(input_range));
}

template<std::input_iterator InputIterator>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(InputIterator begin, InputIterator end)
{
    message_data_length_ = std::distance(begin, end);
    message_data_size_ = (std::min)(static_cast<size_t>(message_data_length_), message_data_.size());
    std::copy_n(begin, message_data_size_, message_data_.begin());
}

template<std::output_iterator<unsigned char> OutputIterator>
APRS_TRACK_INLINE_NO_DISABLE OutputIterator tracker::message(OutputIterator output) const
{
    output = std::copy_n(message_data_.begin(), message_data_size_, output);
    return output;
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::string tracker::message() const
{
    const char* data = reinterpret_cast<const char*>(message_data_.data());
    return std::string(data, message_data_length_);
}

APRS_TRACK_INLINE std::u8string tracker::u8message() const
{
    const char8_t* data = reinterpret_cast<const char8_t*>(message_data_.data());
    return std::u8string(data, message_data_length_);
}

#endif

template<class Rep, class Period>
APRS_TRACK_INLINE_NO_DISABLE void tracker::interval(std::chrono::duration<Rep, Period> interval)
{
    interval_seconds_ = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(interval).count());
}

template<Position T>
APRS_TRACK_INLINE_NO_DISABLE void tracker::position(const T& p)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = p.lat;
    data_.lon = p.lon;

    if constexpr (has_speed<T>)
    {
        data_.speed_knots = mps_to_knots(p.speed);
    }

    if constexpr (has_track<T>)
    {
        data_.track_degrees = p.track;
    }

    if constexpr (has_day_hour_minute_seconds<T>)
    {
        data_.day = p.day;
        data_.hour = p.hour;
        data_.minute = p.minute;
        data_.second = p.second;
    }

    if constexpr (has_altitude<T>)
    {
        data_.alt_feet = meters_to_feet(p.alt);
    }
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE void tracker::interval_seconds(int interval_seconds)
{
    interval_seconds_ = interval_seconds;
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon)
{
    data_.lat = lat;
    data_.lon = lon;
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon, double speed_mps, double track_degrees)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = lat;
    data_.lon = lon;
    data_.speed_knots = mps_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon, double speed_mps, double track_degrees, double alt_meters)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = lat;
    data_.lon = lon;
    data_.speed_knots = mps_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
    data_.alt_feet = meters_to_feet(alt_meters);
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon, double speed_mps, double track_degrees, double alt_meters, int day, int hour, int minute, int second)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = lat;
    data_.lon = lon;
    data_.speed_knots = mps_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
    data_.alt_feet = meters_to_feet(alt_meters);
    data_.day = day;
    data_.hour = hour;
    data_.minute = minute;
    data_.second = second;
}

APRS_TRACK_INLINE void tracker::time(int day, int hour, int minute, int second)
{
    data_.day = day;
    data_.hour = hour;
    data_.minute = minute;
    data_.second = second;
}

APRS_TRACK_INLINE void tracker::time(int hour, int minute, int second)
{
    data_.hour = hour;
    data_.minute = minute;
    data_.second = second;
}

APRS_TRACK_INLINE void tracker::time(int minute, int second)
{
    data_.minute = minute;
    data_.second = second;
}

APRS_TRACK_INLINE void tracker::speed(double speed_mps)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.speed_knots = mps_to_knots(speed_mps);
}

APRS_TRACK_INLINE void tracker::alt(double alt_meters)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.alt_feet = meters_to_feet(alt_meters);
}

APRS_TRACK_INLINE void tracker::track(double track_degrees)
{
    data_.track_degrees = track_degrees;
}

APRS_TRACK_INLINE void tracker::update()
{
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count();
    
    last_update_seconds = static_cast<unsigned int>(elapsed_time);

    if (algorithm_ == algorithm::smart_beaconing)
    {
        if (smart_beaconing_test())
        {
            last_time = std::chrono::high_resolution_clock::now();
            previous_track_degrees_ = data_.track_degrees;
            updated_ = true;
            return;
        }
    }
    else if (algorithm_ == algorithm::periodic)
    {
        if (last_update_seconds >= interval_seconds_)
        {
            last_time = std::chrono::high_resolution_clock::now();
            updated_ = true;
            return;
        }
    }

    updated_ = false;
}

APRS_TRACK_INLINE bool tracker::updated() const
{
    return updated_;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container tracker::packet_string_no_message_to(packet_type p) const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    switch (p)
    {
        case aprs::track::packet_type::mic_e:
            return encode_mic_e_packet_no_message_to<Container>(*this, data_);
        case aprs::track::packet_type::position:
            return encode_position_packet_no_timestamp_no_message_to<Container>(*this, data_);
        case aprs::track::packet_type::position_compressed:
            return encode_position_packet_compressed_no_timestamp_no_message_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp:
            return encode_position_packet_with_timestamp_dhm_no_message_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp_utc:
            return encode_position_packet_with_utc_timestamp_dhm_no_message_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp_utc_hms:
            return encode_position_packet_with_utc_timestamp_hms_no_message_to<Container>(*this, data_);
        default:
            return Container{};
    }
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container tracker::packet_string_to(packet_type p) const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    switch (p)
    {
        case aprs::track::packet_type::mic_e:
            return encode_mic_e_packet_to<Container>(*this, data_);
        case aprs::track::packet_type::position:
            return encode_position_packet_no_timestamp_to<Container>(*this, data_);
        case aprs::track::packet_type::position_compressed:
            return encode_position_packet_compressed_no_timestamp_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp:
            return encode_position_packet_with_timestamp_dhm_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp_utc:
            return encode_position_packet_with_utc_timestamp_dhm_to<Container>(*this, data_);
        case aprs::track::packet_type::position_with_timestamp_utc_hms:
            return encode_position_packet_with_utc_timestamp_hms_to<Container>(*this, data_);
        default:
            return Container{};
    }
}

template<std::output_iterator<unsigned char> OutputIterator>
APRS_TRACK_INLINE_NO_DISABLE OutputIterator tracker::packet(packet_type p, OutputIterator output) const
{
    auto packet = packet_string_no_message_to(p);

    output = std::copy(reinterpret_cast<const unsigned char*>(packet.data()), reinterpret_cast<const unsigned char*>(packet.data() + packet.size()), output);

    output = std::copy_n(message_data_.begin(), message_data_size_, output);

    return output;
}

template<std::ranges::output_range<unsigned char> OutputRange>
APRS_TRACK_INLINE_NO_DISABLE void tracker::packet(packet_type p, OutputRange&& output_range) const
{
    packet(p, std::ranges::begin(output_range));
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE bool tracker::smart_beaconing_test()
{
    int speed_knots = static_cast<int>(data_.speed_knots.value_or(0.0));
    int course_degrees = static_cast<int>(data_.track_degrees.value_or(0));
    int prev_course_degrees = static_cast<int>(previous_track_degrees_.value_or(0));
    int low_speed_knots_int = static_cast<int>(std::round(low_speed_knots_));
    int high_speed_knots_int = static_cast<int>(std::round(high_speed_knots_));

    bool result = APRS_TRACK_DETAIL_NAMESPACE_REFERENCE smart_beaconing_test(speed_knots, prev_course_degrees, course_degrees, low_speed_knots_int, high_speed_knots_int, slow_rate_,
        fast_rate_, turn_time_, turn_angle_, turn_slope_, last_update_seconds);

    return result;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_NAMESPACE_END

// **************************************************************** //
//                                                                  //
//                                                                  //
// private definitions                                              //
//                                                                  //
//                                                                  //
// **************************************************************** //

APRS_TRACK_NAMESPACE_BEGIN

APRS_TRACK_DETAIL_NAMESPACE_BEGIN

template<typename Container> Container encode_position_packet_no_timestamp_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_no_timestamp_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_timestamp_dhm_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_timestamp_dhm_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_utc_timestamp_hms_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_utc_timestamp_hms_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_utc_timestamp_dhm_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_with_utc_timestamp_dhm_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_compressed_no_timestamp_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_position_packet_compressed_no_timestamp_to(const tracker& t, const data& d);
template<typename Container> Container encode_mic_e_packet_no_message_to(const tracker& t, const data& d);
template<typename Container> Container encode_mic_e_packet_to(const tracker& t, const data& d);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_alt_feet(double alt_feet, OutputIt out);
char packet_type_with_timestamp(bool m);

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_no_timestamp_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    encode_position_packet_no_timestamp_no_message(t.from(), t.to(), t.path(), t.messaging(), d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity(), std::back_inserter(packet));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        encode_course_speed(d.track_degrees.value(), d.speed_knots.value(), std::back_inserter(packet));
    }

    if (d.alt_feet.has_value())
    {
        encode_altitude(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_no_timestamp_to(const tracker& t, const data& d)
{
    Container packet = encode_position_packet_no_timestamp_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_timestamp_dhm_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    encode_position_packet_with_timestamp_dhm_no_message(t.from(), t.to(), t.path(), packet_type_with_timestamp(t.messaging()), d.day, d.hour, d.minute, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity(), std::back_inserter(packet));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        encode_course_speed(d.track_degrees.value(), d.speed_knots.value(), std::back_inserter(packet));
    }

    if (d.alt_feet.has_value())
    {
        encode_altitude(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_timestamp_dhm_to(const tracker& t, const data& d)
{
    Container packet = encode_position_packet_with_timestamp_dhm_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_utc_timestamp_hms_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    encode_position_packet_with_utc_timestamp_hms_no_message(t.from(), t.to(), t.path(), t.messaging(), d.hour, d.minute, d.second, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity(), std::back_inserter(packet));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        encode_course_speed(d.track_degrees.value(), d.speed_knots.value(), std::back_inserter(packet));
    }

    if (d.alt_feet.has_value())
    {
        encode_altitude(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_utc_timestamp_hms_to(const tracker& t, const data& d)
{
    Container packet = encode_position_packet_with_utc_timestamp_hms_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_utc_timestamp_dhm_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    encode_position_packet_with_utc_timestamp_dhm_no_message(t.from(), t.to(), t.path(), t.messaging(), d.day, d.hour, d.minute, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity(), std::back_inserter(packet));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        encode_course_speed(d.track_degrees.value(), d.speed_knots.value(), std::back_inserter(packet));
    }

    if (d.alt_feet.has_value())
    {
        encode_altitude(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_with_utc_timestamp_dhm_to(const tracker& t, const data& d)
{
    Container packet = encode_position_packet_with_utc_timestamp_dhm_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_compressed_no_timestamp_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    char compression_type = 0b00111000 + 33; // current, RMC, compressed

    encode_position_packet_compressed_no_timestamp_no_message(t.from(), t.to(), t.path(), t.messaging(), d.lat, d.lon, t.symbol_table(), t.symbol_code(), d.track_degrees.value_or(0), d.speed_knots.value_or(0), compression_type, std::back_inserter(packet));

    if (d.alt_feet.has_value())
    {
        encode_altitude(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_position_packet_compressed_no_timestamp_to(const tracker& t, const data& d)
{
    Container packet = encode_position_packet_compressed_no_timestamp_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_mic_e_packet_no_message_to(const tracker& t, const data& d)
{
    Container packet;

    encode_mic_e_packet_no_message(t.from(), t.path(), d.lat, d.lon, t.mic_e_status(), d.track_degrees.value_or(0), d.speed_knots.value_or(0), t.symbol_table(), t.symbol_code(), t.ambiguity(), std::back_inserter(packet));

    if (d.alt_feet.has_value())
    {
        encode_mic_e_alt_feet(d.alt_feet.value(), std::back_inserter(packet));
    }

    return packet;
}

template<typename Container>
APRS_TRACK_INLINE_NO_DISABLE Container encode_mic_e_packet_to(const tracker& t, const data& d)
{
    Container packet = encode_mic_e_packet_no_message_to<Container>(t, d);

    t.message(std::back_inserter(packet));

    return packet;
}

APRS_TRACK_DETAIL_NAMESPACE_END

APRS_TRACK_NAMESPACE_END

// **************************************************************** //
//                                                                  //
//                                                                  //
//                                                                  //
//                                                                  //
// encoding library                                                 //
//                                                                  //
//                                                                  //
//                                                                  //
//                                                                  //
// **************************************************************** //

// **************************************************************** //
//                                                                  //
//                                                                  //
// private definitions                                              //
//                                                                  //
//                                                                  //
// **************************************************************** //

#include <string_view> // for std::string_view.
#include <tuple> // for std::tuple and std::tie.
#include <cmath> // for mathematical functions like std::abs, std::modf, and std::round.
#include <cstdio> // for std::snprintf.
#include <array> // for std::array.
#include <algorithm> // for std::copy_n and std::copy.
#include <iterator> // for iterator concepts.

APRS_TRACK_NAMESPACE_BEGIN

APRS_TRACK_DETAIL_NAMESPACE_BEGIN

char packet_type_without_timestamp(bool m);
char packet_type_with_timestamp(bool m);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE char packet_type_without_timestamp(bool messaging)
{
    return messaging ? '=' : '!';
}

APRS_TRACK_INLINE char packet_type_with_timestamp(bool messaging)
{
   return messaging ? '@' : '/';
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// common position handling                                         //
//                                                                  //
// **************************************************************** //

struct position_ddm
{
    char lat = '\0';
    int lat_d = 0;
    double lat_m = 0.0;
    char lon = '\0';
    int lon_d = 0;
    double lon_m = 0.0;
};

struct position_ddm_string
{
    std::array<char, 8> lat;
    std::array<char, 9> lon;
};

template<std::output_iterator<char> OutputIt> OutputIt format_number_to_string(double number, int width, int precision, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt format_number_to_string(double number, int precision, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt format_n_digits_string(int number, int width, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt format_two_digits_string(int number, OutputIt out);
template<std::bidirectional_iterator BidirIt> void add_position_ambiguity(BidirIt begin, BidirIt end, int ambiguity);
std::tuple<int, int, double> dd_to_dms(double dd);
std::tuple<int, double> dd_to_ddm(double dd);
position_ddm dd_to_ddm(double lat, double lon);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);
std::array<char, 4> encode_compressed_lon(double lon);
std::array<char, 4> encode_compressed_lat(double lat);
std::array<char, 8> encode_compressed_lat_lon(double lat, double lon);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt format_number_to_string(double number, int width, int precision, OutputIt out)
{
    // Converts a double to a formatted string representation with specific width and precision
    //
    // Parameters:
    //
    //   number    - the floating-point value to format
    //   width     - minimum width for the resulting string (padding with leading zeros if > 0)
    //   precision - decimal precision (number of digits after decimal point)
    //               if precision is 0, formats as integer without decimal point
    //               the number is rounded to the specified precision
    //
    // Examples:
    //
    //   format_number_to_string(37.7749, 4)    ->  37.7749
    //   format_number_to_string(37.7749, 1)    ->  37.8
    //   format_number_to_string(37.7749, 0)    ->  37
    //   format_number_to_string(37.7749, 5, 2) ->  037.77
    //   format_number_to_string(0.7749, 5, 2)  ->  000.77
    //   format_number_to_string(5.5, 8, 4)     ->  0005.5000
    //   format_number_to_string(0.5, 8, 4)     ->  0000.5000
    //   format_number_to_string(-5.5, 8, 4)    -> -005.5000

    char buffer[32];
    int len;

    if (precision == 0)
    {
        double i;
        double f = std::modf(number, &i);
        (void)f;

        if (width > 0)
        {
            len = std::snprintf(buffer, sizeof(buffer), "%0*d", width, static_cast<int>(i));
        }
        else
        {
            len = std::snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(i));
        }
    }
    else
    {
        if (width > 0)
        {
            len = std::snprintf(buffer, sizeof(buffer), "%0*.*f", width + 1, precision, number);
        }
        else
        {
            len = std::snprintf(buffer, sizeof(buffer), "%.*f", precision, number);
        }
    }

    size_t count = static_cast<size_t>((std::min)(len, 31));
    return std::copy_n(buffer, count, out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt format_number_to_string(double number, int precision, OutputIt out)
{
    return format_number_to_string(number, 0, precision, out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt format_n_digits_string(int number, int width, OutputIt out)
{
    char buffer[32];
    int len;

    if (width <= 0)
    {
        len = std::snprintf(buffer, sizeof(buffer), "%d", number);
    }
    else
    {
        len = std::snprintf(buffer, sizeof(buffer), "%0*d", width, number);
    }

    size_t count = static_cast<size_t>((std::min)(len, 31));
    return std::copy_n(buffer, count, out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt format_two_digits_string(int number, OutputIt out)
{
    return format_n_digits_string(number, 2, out);
}

template<std::bidirectional_iterator BidirIt>
APRS_TRACK_INLINE_NO_DISABLE void add_position_ambiguity(BidirIt begin, BidirIt end, int ambiguity)
{
    if (!(ambiguity > 0))
    {
        return;
    }

    if (end - begin < 2)
    {
        return;
    }

    auto it = end - 2;

    int count = 0;

    while (it != begin && count < ambiguity)
    {
        if (*it != '.')
        {
            *it = ' ';
            count++;
        }
        --it;
    }
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::tuple<int, int, double> dd_to_dms(double dd)
{
    // Example algorithm:
    //
    // Input DD = 37.7749
    //
    // D = integer part of 37.7749 = 37
    // DM = fractional part of 37.7749 * 60 = 0.7749 * 60 = 46.494
    // M = integer part of 46.494 = 46
    // S = fractional part of 46.494 * 60 = 0.494 * 60 = 29.64
    //
    // Output DMS = 37° 46' 29.64"

    double d, dm, m, s;
    double dd_abs = std::abs(dd);
    dm = std::modf(dd_abs, &d);
    if (dd < 0)
    {
        d = -d;
    }
    dm = dm * 60.0;
    s = std::modf(dm, &m);
    s = s * 60.0;
    return std::make_tuple(static_cast<int>(d), static_cast<int>(m), s);
}

APRS_TRACK_INLINE std::tuple<int, double> dd_to_ddm(double dd)
{
    int d, m;
    double s;
    std::tie(d, m, s) = dd_to_dms(dd);
    double m_s = m + (s / 60.0);
    return std::make_tuple(d, m_s);
}

APRS_TRACK_INLINE position_ddm dd_to_ddm(double lat, double lon)
{
    position_ddm ddm;
    std::tie(ddm.lat_d, ddm.lat_m) = dd_to_ddm(lat);
    std::tie(ddm.lon_d, ddm.lon_m) = dd_to_ddm(lon);
    ddm.lat_d = std::abs(ddm.lat_d);
    ddm.lon_d = std::abs(ddm.lon_d);
    ddm.lat = lat >= 0 ? 'N' : 'S';
    ddm.lon = lon >= 0 ? 'E' : 'W';
    return ddm;
}

APRS_TRACK_INLINE position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity)
{
    // Converts a position in DDM format to a short string representation
    //
    // Lat: DDMM.MMd
    // Lon: DDDMM.MMd

    position_ddm_string s;

    auto lat_it = s.lat.begin();
    lat_it = format_number_to_string(p.lat_d, 2, 0, lat_it);   // DD
    lat_it = format_number_to_string(p.lat_m, 4, 2, lat_it);   // MM.MM
    *lat_it++ = p.lat;                                         // d

    // Position ambiguity only can be applied to latitude as per APRS spec.
    add_position_ambiguity(s.lat.begin(), lat_it, ambiguity); // DDMM.M d (if the ambiguity is 1)

    auto lon_it = s.lon.begin();
    lon_it = format_number_to_string(p.lon_d, 3, 0, lon_it);   // DDD
    lon_it = format_number_to_string(p.lon_m, 4, 2, lon_it);   // MM.MM
    *lon_it++ = p.lon;                                         // d

    return s;                                                  // DDDMM.MMd DDDMM.MMd
}

APRS_TRACK_INLINE std::array<char, 4> encode_compressed_lon(double lon)
{
    std::array<char, 4> result;

    long num = static_cast<long>(std::round(190463 * (180 + lon)));

    result[3] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[2] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[1] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[0] = static_cast<char>((num % 91) + 33);

    return result;
}

APRS_TRACK_INLINE std::array<char, 4> encode_compressed_lat(double lat)
{
    std::array<char, 4> result;

    long num = static_cast<long>(std::round(380926 * (90 - lat)));

    result[3] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[2] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[1] = static_cast<char>((num % 91) + 33);
    num /= 91;
    result[0] = static_cast<char>((num % 91) + 33);

    return result;
}

APRS_TRACK_INLINE std::array<char, 8> encode_compressed_lat_lon(double lat, double lon)
{
    // Example:
    //
    // Lat: 36.354730315074
    // Lon: -119.3034815161
    //
    // Compressed lat and lon:
    //
    //   <+_*0@"k

    std::array<char, 8> result;

    std::array<char, 4> lat_arr = encode_compressed_lat(lat);
    std::array<char, 4> lon_arr = encode_compressed_lon(lon);

    std::copy(lat_arr.begin(), lat_arr.end(), result.begin());

    std::copy(lon_arr.begin(), lon_arr.end(), result.begin() + 4);

    return result;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// timestamp                                                        //
//                                                                  //
// **************************************************************** //

std::array<char, 7> encode_timestamp_dhm(int day, int hour, int min);
std::array<char, 7> encode_utc_timestamp_dhm(int day, int hour, int min);
std::array<char, 7> encode_utc_timestamp_hms(int hour, int min, int sec);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 7> encode_timestamp_dhm(int day, int hour, int min)
{
    std::array<char, 7> dhm;

    format_n_digits_string(day, 2, dhm.begin());      // DD
    format_n_digits_string(hour, 2, dhm.begin() + 2); // HH
    format_n_digits_string(min, 2, dhm.begin() + 4);  // MM

    dhm[6] = '/';                                     // /

    return dhm;                                       // DDHHMM/
}

APRS_TRACK_INLINE std::array<char, 7> encode_utc_timestamp_dhm(int day, int hour, int min)
{
    std::array<char, 7> dhm;

    format_n_digits_string(day, 2, dhm.begin());      // DD
    format_n_digits_string(hour, 2, dhm.begin() + 2); // HH
    format_n_digits_string(min, 2, dhm.begin() + 4);  // MM

    dhm[6] = 'z';                                     // z

    return dhm;                                       // DDHHMMz
}

APRS_TRACK_INLINE std::array<char, 7> encode_utc_timestamp_hms(int hour, int min, int sec)
{
    std::array<char, 7> hms;

    format_n_digits_string(hour, 2, hms.begin());      // HH
    format_n_digits_string(min, 2, hms.begin() + 2);   // MM
    format_n_digits_string(sec, 2, hms.begin() + 4);   // SS

    hms[6] = 'h';                                      // h

    return hms;                                        // HHMMSSh
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// header                                                           //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out)
{
    // Encode the header of a packet:
    //
    // Example:
    //
    //   N0CALL>APRS,TCPIP*,qAC,N0CALL:
    //   ~~~~~~ ~~~~ ~~~~~~~~~~~~~~~~~
    //   from  >to  ,path             :

    out = std::copy(from.begin(), from.end(), out);     // from

    *out++ = '>';                                       // '>'

    out = std::copy(to.begin(), to.end(), out);         // to

    if (!path.empty())
    {
        *out++ = ',';                                   // ','
        out = std::copy(path.begin(), path.end(), out); // path
    }

    *out++ = ':';                                       // ':'

    return out;                                         // from > to , path :
}

// **************************************************************** //
//                                                                  //
// position no timestamp                                            //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
std::array<char, 20> encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);
position_ddm dd_to_ddm(double lat, double lon);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, ambiguity, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out)
{
    out = encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_course_speed(track_degrees, speed_knots, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out)
{
    out = encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out)
{
    out = encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity, speed_knots, track_degrees, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out) // outside the fw decl ifdef
{
    std::array<char, 20> data = encode_position_data_no_timestamp(type, lat, lon, symbol_table, symbol_code, ambiguity);

    out = std::copy(data.begin(), data.end(), out);

    return out;
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 20> encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    //
    //  Data Format:
    //
    //     !   Lat  Sym  Lon  Sym Code   Comment
    //     =
    //    ------------------------------------------
    //     1    8    1    9      1        0-43
    //
    //  Examples:
    //
    //    !4903.50N/07201.75W-Test 001234
    //    !4903.50N/07201.75W-Test /A=001234
    //    !49  .  N/072  .  W-
    //

    std::array<char, 20> data;

    data[0] = type;                                       // type

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    std::copy_n(ddm_str.lat.data(), 8, data.data() + 1);  // Lat: DDMM.MMd

    data[9] = symbol_table;                               // Symbol table (Sym)

    std::copy_n(ddm_str.lon.data(), 9, data.data() + 10); // Lon: DDDMM.MMd

    data[19] = symbol_code;                               // symbol code (Sym Code)

    return data;                                          // type DDMM.MMd Sym DDDMM.MMd Sym Code
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with timestamp dhm                                      //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
std::array<char, 27> encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
std::array<char, 7> encode_timestamp_dhm(int day, int hour, int min);
position_ddm dd_to_ddm(double lat, double lon);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_with_timestamp_dhm(packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out)
{
    out = encode_position_packet_with_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_course_speed(track_degrees, speed_knots, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, speed_knots, track_degrees, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    std::array<char, 27> data = encode_position_data_with_timestamp_dhm(type, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity);

    out = std::copy(data.begin(), data.end(), out);

    return out;
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 27> encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    // 
    //  Data Format:
    // 
    //     /   Time  Lat   Sym  Lon  Sym Code   Comment
    //     @
    //    -----------------------------------------------
    //     1    7     8     1    9      1        0-43
    //
    //  Examples:
    //
    //    /092345z4903.50N/07201.75W>Test1234
    //    @092345/4903.50N/07201.75W>Test1234
    //

    std::array<char, 27> data;

    data[0] = type;                                                       // type

    std::array<char, 7> timestamp = encode_timestamp_dhm(day, hour, min);

    std::copy_n(timestamp.data(), timestamp.size(), data.begin() + 1);    // Time: DDHHMM/

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    std::copy_n(ddm_str.lat.data(), 8, data.data() + 8);                  // Lat: DDMM.MMd

    data[16] = symbol_table;                                              // Symbol table (Sym)

    std::copy_n(ddm_str.lon.data(), 9, data.data() + 17);                 // Lon: DDDMM.MMd

    data[26] = symbol_code;                                               // symbol code (Sym Code)

    return data;                                                          // type DDHHMM/ DDMM.MMd Sym DDDMM.MMd Sym Code
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with timestamp hms                                      //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
std::array<char, 27> encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
std::array<char, 7> encode_utc_timestamp_hms(int hour, int min, int sec);
position_ddm dd_to_ddm(double lat, double lon);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    std::array<char, 27>  data = encode_position_data_with_utc_timestamp_hms(type, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity);

    out = std::copy(data.begin(), data.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_with_utc_timestamp_hms(packet_type_with_timestamp(messaging), hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_course_speed(track_degrees, speed_knots, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity, speed_knots, track_degrees, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 27> encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    // 
    //  Data Format:
    // 
    //     /   Time  Lat   Sym  Lon  Sym Code   Comment
    //     @
    //    -----------------------------------------------
    //     1    7     8     1    9      1        0-43
    //
    //  Examples:
    //
    //    /092345z4903.50N/07201.75W>Test1234
    //    @092345/4903.50N/07201.75W>Test1234
    //    @234517h4903.50N/07201.75W>Test1234

    std::array<char, 27> data;

    data[0] = type;                                                           // type

    std::array<char, 7> timestamp = encode_utc_timestamp_hms(hour, min, sec);

    std::copy_n(timestamp.data(), timestamp.size(), data.begin() + 1);        // Time: HHMMSSh

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    std::copy_n(ddm_str.lat.data(), ddm_str.lat.size(), data.begin() + 8);    // Lat: DDMM.MMd

    data[16] = symbol_table;                                                  // Symbol table (Sym)

    std::copy_n(ddm_str.lon.data(), ddm_str.lon.size(), data.begin() + 17);   // Lon: DDDMM.MMd

    data[26] = symbol_code;                                                   // symbol code (Sym Code)

    return data;                                                              // type HHMMSSh DDMM.MMd Sym DDDMM.MMd Sym Code
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with utc timestamp                                      //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
std::array<char, 27> encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
std::array<char, 7> encode_utc_timestamp_dhm(int day, int hour, int min);
position_ddm dd_to_ddm(double lat, double lon);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    std::array<char, 27> data = encode_position_data_with_utc_timestamp_dhm(type, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity);

    out = std::copy(data.begin(), data.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_with_utc_timestamp_dhm(packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_course_speed(track_degrees, speed_knots, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet, OutputIt out)
{
    out = encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity, speed_knots, track_degrees, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 27> encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    // 
    //  Data Format:
    // 
    //     /   Time  Lat   Sym  Lon  Sym Code   Comment
    //     @
    //    -----------------------------------------------
    //     1    7     8     1    9      1        0-43
    //
    //  Examples:
    //
    //    /092345z4903.50N/07201.75W>Test1234
    //    @092345/4903.50N/07201.75W>Test1234
    //

    std::array<char, 27> data;

    data[0] = type;                                                       // type

    std::array<char, 7> timestamp = encode_utc_timestamp_dhm(day, hour, min);

    std::copy_n(timestamp.data(), timestamp.size(), data.begin() + 1);    // Time: DDHHMMz

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    std::copy_n(ddm_str.lat.data(), 8, data.data() + 8);                  // Lat: DDMM.MMd

    data[16] = symbol_table;                                              // Symbol table (Sym)

    std::copy_n(ddm_str.lon.data(), 9, data.data() + 17);                 // Lon: DDDMM.MMd

    data[26] = symbol_code;                                               // symbol code (Sym Code)

    return data;                                                          // type DDHHMMz DDMM.MMd Sym DDDMM.MMd Sym Code
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position compression                                             //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
std::array<char, 11> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code);
std::array<char, 12> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type);
std::array<char, 14> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type);
std::array<char, 14> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type);
std::array<char, 2> encode_compressed_course_speed(double course_degrees, double speed_knots);
std::array<char, 2> encode_compressed_altitude(double altitude_feet);
int compression_type_to_int(compression_type type);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, compression_type, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, course_degrees, speed_knots, compression_type, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, alt_feet, compression_type, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, double alt_feet, OutputIt out)
{
    out = encode_header(from, to, path, out);

    out = encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, course_degrees, speed_knots, compression_type, out);

    out = encode_altitude(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, OutputIt out)
{
    auto data = encode_position_data_compressed_no_timestamp(type, lat, lon, symbol_table, symbol_code);

    return std::copy(data.begin(), data.end(), out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type, OutputIt out)
{
    auto data = encode_position_data_compressed_no_timestamp(type, lat, lon, symbol_table, symbol_code, compression_type);

    return std::copy(data.begin(), data.end(), out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, OutputIt out)
{
    auto data = encode_position_data_compressed_no_timestamp(type, lat, lon, symbol_table, symbol_code, course_degrees, speed_knots, compression_type);

    return std::copy(data.begin(), data.end(), out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type, OutputIt out)
{
    auto data = encode_position_data_compressed_no_timestamp(type, lat, lon, symbol_table, symbol_code, alt_feet, compression_type);

    return std::copy(data.begin(), data.end(), out);
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 11> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code)
{
    //
    //  Data Format:
    //
    //     !   Sym     Comp Lat    Comp Lon     Sym Code   Compressed Speed/Range/Alt  CompType  Comment
    //     =
    //    --------------------------------------------------------------------------------------------------
    //     1    1          4           4            1                   2                  1       0-40
    //
    //  Examples:
    //
    //    =/5L!!<*e7>7P[
    //

    std::array<char, 11> data;

    data[0] = type;                                               // type
    data[1] = symbol_table;                                       // symbol table (Sym)

    std::array<char, 4> lat_arr = encode_compressed_lat(lat);
    std::array<char, 4> lon_arr = encode_compressed_lon(lon);

    std::copy_n(lat_arr.data(), lat_arr.size(), data.data() + 2); // compressed latitude (Comp Lat)
    std::copy_n(lon_arr.data(), lon_arr.size(), data.data() + 6); // compressed longitude (Comp Lon)

    data[10] = symbol_code;                                       // symbol code (Sym Code)

    return data;                                                  // type Sym Comp Lat Comp Lon Sym Code
}

APRS_TRACK_INLINE std::array<char, 12> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type)
{
    //
    //  Data Format:
    //
    //     !   Sym     Comp Lat    Comp Lon     Sym Code   Compressed Speed/Range/Alt  CompType  Comment
    //     =
    //    --------------------------------------------------------------------------------------------------
    //     1    1          4           4            1                   2                  1       0-40
    //
    //  Examples:
    //
    //    =/5L!!<*e7>7P[
    //

    std::array<char, 12> data;

    data[0] = type;                                               // type

    data[1] = symbol_table;                                       // symbol table (Sym)

    std::array<char, 4> lat_arr = encode_compressed_lat(lat);
    std::array<char, 4> lon_arr = encode_compressed_lon(lon);

    std::copy_n(lat_arr.data(), lat_arr.size(), data.data() + 2); // compressed latitude (Comp Lat)
    std::copy_n(lon_arr.data(), lon_arr.size(), data.data() + 6); // compressed longitude (Comp Lon)

    data[10] = symbol_code;                                       // symbol code (Sym Code)

    data[11] = static_cast<char>(compression_type);               // compression type (CompType)

    return data;                                                  // type Sym Comp Lat Comp Lon Sym Code CompType
}

APRS_TRACK_INLINE std::array<char, 14> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type)
{
    //
    //  Data Format:
    //
    //     !   Sym     Comp Lat    Comp Lon     Sym Code   Compressed Speed/Range/Alt  CompType  Comment
    //     =
    //    --------------------------------------------------------------------------------------------------
    //     1    1          4           4            1                   2                  1       0-40
    //
    //  Examples:
    //
    //    =/5L!!<*e7>7P[
    //

    std::array<char, 14> data;

    data[0] = type;                                               // type

    data[1] = symbol_table;                                       // symbol table (Sym)

    std::array<char, 4> lat_arr = encode_compressed_lat(lat);
    std::array<char, 4> lon_arr = encode_compressed_lon(lon);

    std::copy_n(lat_arr.data(), lat_arr.size(), data.data() + 2); // compressed latitude (Comp Lat)
    std::copy_n(lon_arr.data(), lon_arr.size(), data.data() + 6); // compressed longitude (Comp Lon)

    data[10] = symbol_code;                                       // symbol code (Sym Code)

    std::array<char, 2> course_speed_arr = encode_compressed_course_speed(course_degrees, speed_knots);

    std::copy_n(course_speed_arr.data(), course_speed_arr.size(), data.data() + 11); // compressed course/speed (Comp Speed)

    data[13] = static_cast<char>(compression_type);               // compression type (CompType)

    return data;                                                  // type Sym Comp Lat Comp Lon Sym Code Comp Speed CompType
}

APRS_TRACK_INLINE std::array<char, 14> encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type)
{
    //
    //  Data Format:
    //
    //     !   Sym     Comp Lat    Comp Lon     Sym Code   Compressed Speed/Range/Alt  CompType  Comment
    //     =
    //    --------------------------------------------------------------------------------------------------
    //     1    1          4           4            1                   2                  1       0-40
    //
    //  Examples:
    //
    //    =/5L!!<*e7>7P[
    //


    std::array<char, 14> data;

    data[0] = type;                                                // type

    data[1] = symbol_table;                                        // symbol table (Sym)

    std::array<char, 4> lat_arr = encode_compressed_lat(lat);
    std::array<char, 4> lon_arr = encode_compressed_lon(lon);

    std::copy_n(lat_arr.data(), lat_arr.size(), data.data() + 2);  // compressed latitude (Comp Lat)
    std::copy_n(lon_arr.data(), lon_arr.size(), data.data() + 6);  // compressed longitude (Comp Lon)

    data[10] = symbol_code;                                        // symbol code (Sym Code)

    std::array<char, 2> alt_arr = encode_compressed_altitude(alt_feet);

    std::copy_n(alt_arr.data(), alt_arr.size(), data.data() + 11); // compressed altitude (Comp Alt)

    data[13] = static_cast<char>(compression_type);                // compression type (CompType)

    return data;                                                   // type Sym Comp Lat Comp Lon Sym Code Comp Alt CompType
}

APRS_TRACK_INLINE std::array<char, 2> encode_compressed_course_speed(double course_degrees, double speed_knots)
{
    std::array<char, 2> course_speed;

    // course degrees is expressed in degrees 0 to 359, clockwise from due north
    // if the value exceeds 359, data_it is wrapped around to 0
    while (course_degrees >= 360.0)
    {
        course_degrees -= 360.0;
    }

    int c = static_cast<int>(course_degrees / 4.0);
    int s = static_cast<int>(std::round(std::log(speed_knots + 1.0) / std::log(1.08)));

    course_speed[0] = static_cast<char>(c + 33);
    course_speed[1] = static_cast<char>(s + 33);

    return course_speed;
}

APRS_TRACK_INLINE std::array<char, 2> encode_compressed_altitude(double altitude_feet)
{
    std::array<char, 2> alt;

    int cs = static_cast<int>(std::round(std::log(altitude_feet) / std::log(1.002)));

    int c = cs / 91;
    int s = cs % 91;

    alt[0] = static_cast<char>(c + 33);
    alt[1] = static_cast<char>(s + 33);

    return alt;
}

APRS_TRACK_INLINE int compression_type_to_int(compression_type type)
{
    switch (type)
    {
        case compression_type::old_other_compressed: return 0b00000000;
        case compression_type::old_other_tnc_b_text: return 0b00000001;
        case compression_type::old_other_software: return 0b00000010;
        case compression_type::old_other_tbd_1: return 0b00000011;
        case compression_type::old_other_kpc3: return 0b00000100;
        case compression_type::old_other_pico: return 0b00000101;
        case compression_type::old_other_other_tracker: return 0b00000110;
        case compression_type::old_other_digipeater: return 0b00000111;

        case compression_type::old_gll_compressed: return 0b00001000;
        case compression_type::old_gll_tnc_b_text: return 0b00001001;
        case compression_type::old_gll_software: return 0b00001010;
        case compression_type::old_gll_tbd_1: return 0b00001011;
        case compression_type::old_gll_kpc3: return 0b00001100;
        case compression_type::old_gll_pico: return 0b00001101;
        case compression_type::old_gll_other_tracker: return 0b00001110;
        case compression_type::old_gll_digipeater: return 0b00001111;

        case compression_type::old_gga_compressed: return 0b00010000;
        case compression_type::old_gga_tnc_b_text: return 0b00010001;
        case compression_type::old_gga_software: return 0b00010010;
        case compression_type::old_gga_tbd_1: return 0b00010011;
        case compression_type::old_gga_kpc3: return 0b00010100;
        case compression_type::old_gga_pico: return 0b00010101;
        case compression_type::old_gga_other_tracker: return 0b00010110;
        case compression_type::old_gga_digipeater: return 0b00010111;

        case compression_type::old_rmc_compressed: return 0b00011000;
        case compression_type::old_rmc_tnc_b_text: return 0b00011001;
        case compression_type::old_rmc_software: return 0b00011010;
        case compression_type::old_rmc_tbd_1: return 0b00011011;
        case compression_type::old_rmc_kpc3: return 0b00011100;
        case compression_type::old_rmc_pico: return 0b00011101;
        case compression_type::old_rmc_other_tracker: return 0b00011110;
        case compression_type::old_rmc_digipeater: return 0b00011111;

        case compression_type::current_other_compressed: return 0b00100000;
        case compression_type::current_other_tnc_b_text: return 0b00100001;
        case compression_type::current_other_software: return 0b00100010;
        case compression_type::current_other_tbd_1: return 0b00100011;
        case compression_type::current_other_kpc3: return 0b00100100;
        case compression_type::current_other_pico: return 0b00100101;
        case compression_type::current_other_other_tracker: return 0b00100110;
        case compression_type::current_other_digipeater: return 0b00100111;

        case compression_type::current_gll_compressed: return 0b00101000;
        case compression_type::current_gll_tnc_b_text: return 0b00101001;
        case compression_type::current_gll_software: return 0b00101010;
        case compression_type::current_gll_tbd_1: return 0b00101011;
        case compression_type::current_gll_kpc3: return 0b00101100;
        case compression_type::current_gll_pico: return 0b00101101;
        case compression_type::current_gll_other_tracker: return 0b00101110;
        case compression_type::current_gll_digipeater: return 0b00101111;

        case compression_type::current_gga_compressed: return 0b00110000;
        case compression_type::current_gga_tnc_b_text: return 0b00110001;
        case compression_type::current_gga_software: return 0b00110010;
        case compression_type::current_gga_tbd_1: return 0b00110011;
        case compression_type::current_gga_kpc3: return 0b00110100;
        case compression_type::current_gga_pico: return 0b00110101;
        case compression_type::current_gga_other_tracker: return 0b00110110;
        case compression_type::current_gga_digipeater: return 0b00110111;

        case compression_type::current_rmc_compressed: return 0b00111000;
        case compression_type::current_rmc_tnc_b_text: return 0b00111001;
        case compression_type::current_rmc_software: return 0b00111010;
        case compression_type::current_rmc_tbd_1: return 0b00111011;
        case compression_type::current_rmc_kpc3: return 0b00111100;
        case compression_type::current_rmc_pico: return 0b00111101;
        case compression_type::current_rmc_other_tracker: return 0b00111110;
        case compression_type::current_rmc_digipeater: return 0b00111111;

        default:
            break;
    }

    return 0;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// mic-e                                                            //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_course_speed(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_alt_feet(double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_mic_e_alt(double alt_meters, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt append_mic_e_manufacturer(std::string_view manufacturer_version, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_header(std::string_view from, std::string_view to, std::string_view path, OutputIt out);
std::array<char, 9> encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code);
void add_mic_e_position_ambiguity(std::array<char, 6>& destination_address, int ambiguity);
void encode_mic_e_status(int a, int b, int c, bool custom, std::array<char, 6>& destination_address);
void encode_mic_e_lat_direction(char direction, std::array<char, 6>& destination_address);
void encode_mic_lon_offset(bool offset, std::array<char, 6>& destination_address);
void encode_mic_lon_direction(char direction, std::array<char, 6>& destination_address);
std::tuple<int, int, int, bool> encode_mic_e_status(mic_e_status status);
void encode_mic_e_status(mic_e_status status, std::array<char, 6>& destination_address);
std::array<char, 6> encode_mic_e_lat(double lat);
std::array<char, 6> encode_mic_e_lat(double lat, mic_e_status status);
std::array<char, 6> encode_mic_e_lat(double lat, double lon, mic_e_status status, int ambiguity);
char encode_mic_e_lon_degrees(int lon_d);
char encode_mic_e_lon_minutes(int lon_m);
char encode_mic_e_lon_hundred_minutes(int lon_h);
std::array<char, 3> encode_mic_e_lon(double lon);
std::array<char, 3> encode_mic_e_course_speed(double course_degrees, double speed_knots);
std::array<char, 3> encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots);
std::array<char, 4> encode_mic_e_alt(double alt_meters);
std::array<char, 4> encode_mic_e_alt_feet(double alt_feet);
double round_number_to_nearest_tenth(double number);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code, OutputIt out)
{
    std::array<char, 9> data = encode_mic_e_data(type, lat, lon, course_degrees, speed_knots, symbol_table, symbol_code);

    out = std::copy(data.begin(), data.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, OutputIt out)
{
    std::array<char, 6> lat_str = encode_mic_e_lat(lat, lon, status, ambiguity);

    out = encode_header(from, { lat_str.data(), lat_str.size() }, path, out);

    out = encode_mic_e_data('`', lat, lon, course_degrees, speed_knots, symbol_table, symbol_code, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, double alt_feet, OutputIt out)
{
    out = encode_mic_e_packet_no_message(from, path, lat, lon, status, course_degrees, speed_knots, symbol_table, symbol_code, ambiguity, out);

    out = encode_mic_e_alt_feet(alt_feet, out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_course_speed(double course_degrees, double speed_knots, OutputIt out)
{
    std::array<char, 3> course_speed = encode_mic_e_course_speed(course_degrees, speed_knots);

    out = std::copy(course_speed.begin(), course_speed.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots, OutputIt out)
{
    std::array<char, 3> course_speed = encode_mic_e_course_speed_alternate(course_degrees, speed_knots);

    out = std::copy(course_speed.begin(), course_speed.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_alt_feet(double alt_feet, OutputIt out)
{
    std::array<char, 4> alt_arr = encode_mic_e_alt_feet(alt_feet);

    out = std::copy(alt_arr.begin(), alt_arr.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_mic_e_alt(double alt_meters, OutputIt out)
{
    std::array<char, 4> alt_arr = encode_mic_e_alt(alt_meters);

    out = std::copy(alt_arr.begin(), alt_arr.end(), out);

    return out;
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt append_mic_e_manufacturer(std::string_view manufacturer_version, OutputIt out)
{
    return std::copy(manufacturer_version.begin(), manufacturer_version.end(), out);
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 9> encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code)
{
    (void)lat;

    std::array<char, 9> data;

    data[0] = type;

    std::array<char, 3> lon_arr = encode_mic_e_lon(lon);
    std::array<char, 3> course_speed_arr = encode_mic_e_course_speed(course_degrees, speed_knots);

    std::copy_n(lon_arr.data(), lon_arr.size(), data.data() + 1);
    std::copy_n(course_speed_arr.data(), course_speed_arr.size(), data.data() + 4);

    data[7] = symbol_code;
    data[8] = symbol_table;

    return data;
}

APRS_TRACK_INLINE void add_mic_e_position_ambiguity(std::array<char, 6>& destination_address, int ambiguity)
{
    if (!(ambiguity > 0))
    {
        return;
    }

    for (size_t i = destination_address.size() - 1, count = 0; count < static_cast<size_t>(ambiguity) && i < destination_address.size(); i--, count++)
    {
        if (destination_address[i] >= 'P' && destination_address[i] <= 'Y')
        {
            destination_address[i] = 'Z';
        }
        else if (destination_address[i] >= '0' && destination_address[i] <= '9')
        {
            destination_address[i] = 'L';
        }
        else if (destination_address[i] >= 'A' && destination_address[i] <= 'J')
        {
            destination_address[i] = 'K';
        }
    }
}

APRS_TRACK_INLINE void encode_mic_e_status(int a, int b, int c, bool custom, std::array<char, 6>& destination_address)
{
    int message_bits[3] = { a, b, c };

    for (size_t i = 0; i < 3; i++)
    {
        // this handling is not necessary due to how the
        // lat ambiguity is applied later, but data_it is useful in testing
        if (destination_address[i] == ' ')
        {
            if (message_bits[i] == 1 && custom)
            {
                destination_address[i] = 'K';
            }
            else if (message_bits[i] == 1 && !custom)
            {
                destination_address[i] = 'Z';
            }
            else if (message_bits[i] == 0)
            {
                destination_address[i] = 'L';
            }
            continue;
        }

        if (message_bits[i] == 1 && custom)
        {
            destination_address[i] = 'A' + (destination_address[i] - '0');
        }
        else if (message_bits[i] == 1 && !custom)
        {
            destination_address[i] = 'P' + (destination_address[i] - '0');
        }
    }
}

APRS_TRACK_INLINE void encode_mic_e_lat_direction(char direction, std::array<char, 6>& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but data_it is useful in testing
    if (destination_address[3] == ' ')
    {
        if (direction == 'N')
        {
            destination_address[3] = 'Z';
        }
        else if (direction == 'S')
        {
            destination_address[3] = 'L';
        }
        return;
    }

    if (direction == 'N')
    {
        destination_address[3] = destination_address[3] = 'P' + (destination_address[3] - '0');
    }
}

APRS_TRACK_INLINE void encode_mic_lon_offset(bool offset, std::array<char, 6>& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but data_it is useful in testing
    if (destination_address[4] == ' ')
    {
        if (offset)
        {
            destination_address[4] = 'Z';
        }
        else
        {
            destination_address[4] = 'L';
        }
        return;
    }

    if (offset)
    {
        destination_address[4] = destination_address[4] = 'P' + (destination_address[4] - '0');
    }
}

APRS_TRACK_INLINE void encode_mic_lon_direction(char direction, std::array<char, 6>& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but data_it is useful in testing
    if (destination_address[5] == ' ')
    {
        if (direction == 'W')
        {
            destination_address[5] = 'Z';
        }
        else if (direction == 'E')
        {
            destination_address[5] = 'L';
        }
        return;
    }

    if (direction == 'W')
    {
        destination_address[5] = destination_address[5] = 'P' + (destination_address[5] - '0');
    }
}

APRS_TRACK_INLINE std::tuple<int, int, int, bool> encode_mic_e_status(mic_e_status status)
{
    int a = 0;
    int b = 0;
    int c = 0;
    bool custom = false;

    //  MIC-E Status Encoding Truth Table
    //  --------------------------------
    //  Status         Custom  a b c
    //  --------------------------------
    //  off_duty        0      1 1 1
    //  en_route        0      1 1 0
    //  in_service      0      1 0 1
    //  returning       0      1 0 0
    //  committed       0      0 1 1
    //  special         0      0 1 0
    //  priority        0      0 0 1
    //  emergency       0      0 0 0
    //  custom0         1      1 1 1
    //  custom1         1      1 1 0
    //  custom2         1      1 0 1
    //  custom3         1      1 0 0
    //  custom4         1      0 1 1
    //  custom5         1      0 1 0
    //  custom6         1      0 0 1
    //  --------------------------------

    switch (status)
    {
    case mic_e_status::off_duty:
        a = 1;
        b = 1;
        c = 1;
        break;
    case mic_e_status::en_route:
        a = 1;
        b = 1;
        c = 0;
        break;
    case mic_e_status::in_service:
        a = 1;
        b = 0;
        c = 1;
        break;
    case mic_e_status::returning:
        a = 1;
        b = 0;
        c = 0;
        break;
    case mic_e_status::commited:
        a = 0;
        b = 1;
        c = 1;
        break;
    case mic_e_status::special:
        a = 0;
        b = 1;
        c = 0;
        break;
    case mic_e_status::priority:
        a = 0;
        b = 0;
        c = 1;
        break;
    case mic_e_status::emergency:
        a = 0;
        b = 0;
        c = 0;
        break;
    case mic_e_status::custom0:
        a = 1;
        b = 1;
        c = 1;
        custom = true;
        break;
    case mic_e_status::custom1:
        a = 1;
        b = 1;
        c = 0;
        custom = true;
        break;
    case mic_e_status::custom2:
        a = 1;
        b = 0;
        c = 1;
        custom = true;
        break;
    case mic_e_status::custom3:
        a = 1;
        b = 0;
        c = 0;
        custom = true;
        break;
    case mic_e_status::custom4:
        a = 0;
        b = 1;
        c = 1;
        custom = true;
        break;
    case mic_e_status::custom5:
        a = 0;
        b = 1;
        c = 0;
        custom = true;
        break;
    case mic_e_status::custom6:
        a = 0;
        b = 0;
        c = 1;
        custom = true;
        break;
    default:
        break;
    }

    return std::make_tuple(a, b, c, custom);
}

APRS_TRACK_INLINE void encode_mic_e_status(mic_e_status status, std::array<char, 6>& destination_address)
{
    int a = 0;
    int b = 0;
    int c = 0;
    bool custom = false;

    std::tie(a, b, c, custom) = encode_mic_e_status(status);

    encode_mic_e_status(a, b, c, custom, destination_address);
}

APRS_TRACK_INLINE std::array<char, 6> encode_mic_e_lat(double lat)
{
    // Converts decimal degrees coordinates to mic-e position format:
    //
    // 1. Latitude/Longitude Degree Extraction:
    //    - For a coordinate like 33.4273°, extracts the whole degrees (33°)
    //    - Uses integer truncation to remove the decimal portion from the absolute value
    //
    // 2. Minutes Conversion:
    //    - Takes the decimal portion (0.4273) and converts to minutes:
    //      * Subtracts whole degrees: 33.4273° - 33° = 0.4273°
    //      * Multiplies by 60 to get minutes: 0.4273° × 60 = 25.638 minutes
    //    
    // 3. Minutes Separation:
    //    - Uses modf() to split minutes into whole and decimal parts:
    //      * Whole minutes: 25
    //      * Decimal minutes: 0.638
    //    - Multiplies decimal by 100 to get hundredths: 0.638 × 100 = 63.8
    //
    // Final Format: DDMMHH (Degrees, Minutes, Hundredths of minutes)  
    // Example: 33° 25.638' becomes 332563
    //
    // This format matches the mic-e position specification which uses
    // degrees and decimal minutes rather than decimal degrees

    double lat_abs = std::fabs(lat);

    int lat_d = static_cast<int>(lat_abs);

    double lat_m = (lat_abs - lat_d) * 60.0;
    double lat_m_f = 0.0;
    double lat_m_i = std::modf(lat_m, &lat_m_f) * 100.0;
    lat_m_i = std::round(lat_m_i);

    // Resulting coordinates stored as: 33° 25.638' -> 332563

    char buffer[7];
    std::snprintf(buffer, sizeof(buffer), "%02d%02d%02d", lat_d, static_cast<int>(lat_m_f), static_cast<int>(lat_m_i));

    std::array<char, 6> lat_str;

    std::copy(buffer, buffer + 6, lat_str.begin());

    return lat_str;
}

APRS_TRACK_INLINE std::array<char, 6> encode_mic_e_lat(double lat, mic_e_status status)
{
    char direction = (lat >= 0.0) ? 'N' : 'S';

    std::array<char, 6> lat_str = encode_mic_e_lat(lat);

    encode_mic_e_status(status, lat_str);
    encode_mic_e_lat_direction(direction, lat_str);

    return lat_str;
}

APRS_TRACK_INLINE std::array<char, 6> encode_mic_e_lat(double lat, double lon, mic_e_status status, int ambiguity)
{
    std::array<char, 6> lat_str = encode_mic_e_lat(lat, status);

    double lon_abs = std::fabs(lon);
    int lon_d = static_cast<int>(lon_abs);
    bool lon_offset = ((lon_d >= 0 && lon_d <= 9) || lon_d >= 100) ? true : false;

    char lon_direction = lon < 0.0 ? 'W' : 'E';

    encode_mic_lon_offset(lon_offset, lat_str);
    encode_mic_lon_direction(lon_direction, lat_str);

    add_mic_e_position_ambiguity(lat_str, ambiguity);

    return lat_str;
}

APRS_TRACK_INLINE char encode_mic_e_lon_degrees(int lon_d)
{
    int result = 0;

    if (lon_d >= 0 && lon_d <= 9)
    {
        result = lon_d + 118;
    }
    else if (lon_d >= 10 && lon_d <= 99)
    {
        result = lon_d + 28;
    }
    else if (lon_d >= 100 && lon_d <= 109)
    {
        result = lon_d + 8;
    }
    else if (lon_d >= 110 && lon_d <= 179)
    {
        result = lon_d - 100 + 28;
    }

    return static_cast<char>(result);
}

APRS_TRACK_INLINE char encode_mic_e_lon_minutes(int lon_m)
{
    int result = 0;

    if (lon_m >= 0 && lon_m <= 9)
    {
        result = lon_m + 88;
    }
    else if (lon_m >= 10 && lon_m <= 59)
    {
        result = lon_m + 28;
    }

    return static_cast<char>(result);
}

APRS_TRACK_INLINE char encode_mic_e_lon_hundred_minutes(int lon_h)
{
    int result = 0;

    if (lon_h >= 0 && lon_h <= 99)
    {
        result = lon_h + 28;
    }

    return static_cast<char>(result);
}

APRS_TRACK_INLINE std::array<char, 3> encode_mic_e_lon(double lon)
{
    // Similar implementation to latitude encoding, but with additional
    // table lookups for the longitude degrees, minutes, and hundredths
    // 
    // Converts decimal degrees coordinates to mic-e position format:
    //
    // 1. Latitude/Longitude Degree Extraction:
    //    - For a coordinate like 33.4273°, extracts the whole degrees (33°)
    //    - Uses integer truncation to remove the decimal portion from the absolute value
    //
    // 2. Minutes Conversion:
    //    - Takes the decimal portion (0.4273) and converts to minutes:
    //      * Subtracts whole degrees: 33.4273° - 33° = 0.4273°
    //      * Multiplies by 60 to get minutes: 0.4273° × 60 = 25.638 minutes
    //    
    // 3. Minutes Separation:
    //    - Uses modf() to split minutes into whole and decimal parts:
    //      * Whole minutes: 25
    //      * Decimal minutes: 0.638
    //    - Multiplies decimal by 100 to get hundredths: 0.638 × 100 = 63.8

    std::array<char, 3> lon_str;

    double lon_abs = std::fabs(lon);

    int lon_d = static_cast<int>(lon_abs);

    double lon_m = (lon_abs - lon_d) * 60.0;
    double lon_m_f = 0.0;
    double lon_m_i = std::modf(lon_m, &lon_m_f) * 100.0;
    lon_m_i = round_number_to_nearest_tenth(lon_m_i);

    lon_str[0] = encode_mic_e_lon_degrees(lon_d);
    lon_str[1] = encode_mic_e_lon_minutes(static_cast<int>(lon_m));
    lon_str[2] = encode_mic_e_lon_hundred_minutes(static_cast<int>(lon_m_i));

    return lon_str;
}

APRS_TRACK_INLINE std::array<char, 3> encode_mic_e_course_speed(double course_degrees, double speed_knots)
{
    std::array<char, 3> course_speed;

    int course = static_cast<int>(round_number_to_nearest_tenth(course_degrees));
    int speed = static_cast<int>(round_number_to_nearest_tenth(speed_knots));

    int sp = (speed / 10) + 'l'; // or + 28
    int se = (course % 100) + 28;

    int dc = 0;

    if (course >= 0 && course <= 99)
    {
        dc = ' '; // or 28
    }
    else if (course >= 100 && course <= 199)
    {
        dc = '!'; // or 29
    }
    else if (course >= 200 && course <= 299)
    {
        dc = '"'; // or 30
    }
    else if (course >= 300 && course <= 360)
    {
        dc = '#'; // or 31
    }

    int speed_units = speed % 10;

    if (speed_units > 0)
    {
        dc = dc + speed_units * 10;
    }

    course_speed[0] = static_cast<char>(sp);
    course_speed[1] = static_cast<char>(dc);
    course_speed[2] = static_cast<char>(se);

    return course_speed;
}

APRS_TRACK_INLINE std::array<char, 3> encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots)
{
    std::array<char, 3> course_speed;

    int course = static_cast<int>(std::round(course_degrees));
    int speed = static_cast<int>(std::round(speed_knots));

    int sp = (speed / 10) + 28; // or  l
    int se = (course % 100) + 28;

    int dc = 0;

    if (course >= 0 && course <= 99)
    {
        dc = 28; // or ' '
    }
    else if (course >= 100 && course <= 199)
    {
        dc = 29; // or !
    }
    else if (course >= 200 && course <= 299)
    {
        dc = 30; // or "
    }
    else if (course >= 300 && course <= 360)
    {
        dc = 31; // or #
    }

    int speed_units = speed % 10;

    if (speed_units > 0)
    {
        dc = dc + speed_units * 10;
    }

    course_speed[0] = static_cast<char>(sp);
    course_speed[1] = static_cast<char>(dc);
    course_speed[2] = static_cast<char>(se);

    return course_speed;
}

APRS_TRACK_INLINE std::array<char, 4> encode_mic_e_alt(double alt_meters)
{
    // Encoded altitude in mic-e format
    //
    //   xxx}
    //
    // Example:
    //
    //   altitude = 59.06 feet = 18 meters
    //   relative altitude = 18 + 10000 = 10018
    //                       ~~
    //   base-91 encoding: 10018 = 1*91² + 19*91 + 8
    //                             ~       ~~      ~
    //   ASCII characters: 1+33=34 '"', 19+33=52 '4', 8 + 33=41 ')'
    //                     ~            ~~            ~
    //   result: "4)}

    std::array<char, 4> alt_str;

    int alt_meters_int = static_cast<int>(std::round(alt_meters));
    int relative_alt = alt_meters_int + 10000;

    int v0 = relative_alt / (91 * 91);
    int r = relative_alt % (91 * 91);
    int v1 = r / 91;
    int v2 = r % 91;

    alt_str[0] = static_cast<char>(v0 + 33);
    alt_str[1] = static_cast<char>(v1 + 33);
    alt_str[2] = static_cast<char>(v2 + 33);
    alt_str[3] = '}';

    return alt_str;
}

APRS_TRACK_INLINE std::array<char, 4> encode_mic_e_alt_feet(double alt_feet)
{
    double alt_meters = alt_feet * 0.3048;
    return encode_mic_e_alt(alt_meters);
}

APRS_TRACK_INLINE double round_number_to_nearest_tenth(double number)
{
    // Round number implementation for Mic-E
    // In reality just using std::round would be sufficient
    // But rounding to the nearest tenth make the encoding more
    // compatible with existing Mic-E implementations like LibFAP

    double factor = 10.0;
    return std::round(number * factor) / factor;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
//  common data extensions                                          //
//                                                                  //
// **************************************************************** //

template<std::output_iterator<char> OutputIt> OutputIt encode_altitude(double alt_feet, OutputIt out);
template<std::output_iterator<char> OutputIt> OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out);
std::array<char, 7> encode_course_speed(double course_degrees, double speed_knots);
std::array<char, 9> encode_altitude(double alt_feet);

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_altitude(double alt_feet, OutputIt out)
{
    std::array<char, 9> alt = encode_altitude(alt_feet);
    return std::copy(alt.begin(), alt.end(), out);
}

template<std::output_iterator<char> OutputIt>
APRS_TRACK_INLINE_NO_DISABLE OutputIt encode_course_speed(double course_degrees, double speed_knots, OutputIt out)
{
    std::array<char, 7> course_speed = encode_course_speed(course_degrees, speed_knots);
    return std::copy(course_speed.begin(), course_speed.end(), out);
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE std::array<char, 7> encode_course_speed(double course_degrees, double speed_knots)
{
    // 
    //  Data Format:
    // 
    //    Course / Speed
    //    --------------
    //      3    1   3
    //
    //  Example:
    //
    //    088/036 - a course 88 degrees, traveling at 36 knots
    //    000/000 - the course is unknown
    //       /    - the course is unknown
    //
    //  Course is expressed in degrees 001 to 360, clockwise from due north
    //

    int course_degrees_int = static_cast<int>(std::round(course_degrees));
    int speed_knots_int = static_cast<int>(std::round(speed_knots));

    std::array<char, 7> course_speed;

    format_n_digits_string(course_degrees_int, 3, course_speed.begin());

    course_speed[3] = '/';

    format_n_digits_string(speed_knots_int, 3, course_speed.begin() + 4);

    return course_speed;
}

APRS_TRACK_INLINE std::array<char, 9> encode_altitude(double alt_feet)
{
    //
    //  Data Format:
    //
    //    /A=aaaaaa
    //    ----------
    //     3   6
    //
    //  Example:
    //
    //    /A=001234 - an altitude of 1234 feet
    //    /A=123456 - the altitude is 123456 feet
    //
    //  Altitude is expressed in feet above sea level
    //

    int alt_feet_int = static_cast<int>(std::round(alt_feet));

    std::array<char, 9> altitude = { '/','A','=' };

    format_n_digits_string(alt_feet_int, 6, altitude.begin() + 3);

    return altitude;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_DETAIL_NAMESPACE_END

APRS_TRACK_NAMESPACE_END

// **************************************************************** //
//                                                                  //
// smart beaconing                                                  //
//                                                                  //
// **************************************************************** //

APRS_TRACK_NAMESPACE_BEGIN

APRS_TRACK_DETAIL_NAMESPACE_BEGIN

bool smart_beaconing_test(int speed, int prev_course, int course, int low_speed, int high_speed, int slow_rate, int fast_rate, int turn_time, int turn_angle, int turn_slope, int last_update);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE bool smart_beaconing_test(int speed, int prev_course, int course, int low_speed, int high_speed, int slow_rate, int fast_rate, int turn_time, int turn_angle, int turn_slope, int last_update)
{
    //
    // Smart Beaconing Algorithm (TM)
    // The Smart Beaconing Algorithm (TM) is a trademark of Tony Arnerich and Steve Bragg.
    //
    // This algorithm dynamically adjusts position reporting intervals based on speed
    // and course changes, reducing unnecessary transmissions while ensuring timely
    // updates during maneuvers.
    //
    // Parameters:
    //
    //   speed       - current speed (unit independent, e.g., 50 mph or 80 km/h)
    //   course      - current heading in degrees (0-360)
    //   low_speed   - speed threshold below which slow_rate is used
    //                 when the speed is lower than "low_speed", the tracker will update (transmit packet) at the slow_rate
    //   high_speed  - speed threshold above which fast_rate is used
    //                 when the speed is higher than "high_speed", the tracker will update (transmit packet) at the fast_rate
    //   slow_rate   - transmission interval (seconds) at or below low_speed
    //   fast_rate   - transmission interval (seconds) at or above high_speed
    //   turn_time   - minimum seconds between transmissions when cornering
    //                 the minimum number of seconds the tracker must turn before it will transmit a packet
    //   turn_angle  - minimum heading change (degrees) to trigger a corner transmission
    //   turn_slope  - speed-adjusted turn threshold factor; the effective turn
    //                 threshold is: turn_angle + (turn_slope / speed)
    //   last_update - seconds since last update (transmission)
    //
    // Returns:
    //
    //   true - tracker should transmit a position packet
    //   false - no transmission needed
    //
    // References:
    //
    //   https://www.w8wjb.com/qth/QTHHelp/English.lproj/adv-smartbeaconing.html
    //   https://thelifeofkenneth.com/files/thesis_aprs_finnegan_final.pdf
    //   https://n3ujj.com/manuals/SmartBeaconing.pdf
    //   https://github.com/wb2osz/direwolf/blob/master/src/beacon.c
    //

    int interval = 0;

    // Calculate course delta with wraparound
    int course_delta = std::abs(prev_course - course);
    if (course_delta > 180)
    {
        course_delta = 360 - course_delta; // Handle angle wraparound
    }

    if (speed < low_speed)
    {
        interval = slow_rate;
    }
    else
    {
        if (speed > high_speed)
        {
            interval = fast_rate;
        }
        else
        {
            interval = fast_rate * (high_speed / speed);
        }

        int turn_threshold = turn_angle + (turn_slope / speed);

        if (course_delta > turn_threshold)
        {
            interval = turn_time;
        }
    }

    bool result = (last_update >= interval);

    APRS_TRACK_SMART_BEACONING_DEBUG(result, speed, prev_course, course, low_speed, high_speed, slow_rate, fast_rate, turn_time, turn_angle, turn_slope, last_update, interval);

    return result;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// conversions                                                      //
//                                                                  //
// **************************************************************** //

double meters_to_feet(double meters);
double mps_to_knots(double mps);
double knots_to_mps(double knots);
double mph_to_mps(double mph);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE double meters_to_feet(double meters)
{
    return meters * 3.28084;
}

APRS_TRACK_INLINE double mps_to_knots(double mps)
{
    return mps * 1.9438444924406;
}

APRS_TRACK_INLINE double knots_to_mps(double knots)
{
    return knots / 1.9438444924406;
}

APRS_TRACK_INLINE double mph_to_mps(double mph)
{
    return mph * 0.44704;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_DETAIL_NAMESPACE_END

APRS_TRACK_NAMESPACE_END
