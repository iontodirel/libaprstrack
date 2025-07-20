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

APRS_TRACK_NAMESPACE_BEGIN

#ifndef APRS_TRACK_DEFINE_CUSTOM_TYPES

using string_t = std::basic_string<char>;

#endif // APRS_TRACK_DEFINE_CUSTOM_TYPES

APRS_TRACK_NAMESPACE_END

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
    { t.speed } -> std::convertible_to<double>;
};

template<typename T>
concept has_track = requires(T t)
{
    { t.track } -> std::convertible_to<double>;
};

template <typename T>
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

string_t encode_position_packet_no_timestamp_no_message(const tracker& t, const data& d);
string_t encode_position_packet_no_timestamp(const tracker& t, const data& d);
string_t encode_position_packet_with_timestamp_dhm_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_timestamp_dhm(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_hms(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_dhm(const tracker& t, const data& d);
string_t encode_position_packet_compressed_no_timestamp_no_message(const tracker& t, const data& d);
string_t encode_position_packet_compressed_no_timestamp(const tracker& t, const data& d);
string_t encode_mic_e_packet_no_message(const tracker& t, const data& d);
string_t encode_mic_e_packet(const tracker& t, const data& d);

bool smart_beaconing_test(int speed, int course, int low_speed, int high_speed, int slow_interval_seconds, int fast_interval_seconds, int min_turn_degrees, int turn_interval_seconds, int turn_slope, int last_update_seconds);

double meters_to_feet(double meters);
double meters_s_to_knots(double meters_s);

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

template <typename T>
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
    const string_t& from() const;
    const string_t& to() const;
    const string_t& path() const;

    void ambiguity(int a);
    int ambiguity() const;

    void messaging(bool m);
    bool messaging() const;

    void mic_e_status(enum mic_e_status s);
    enum mic_e_status mic_e_status() const;

    void low_speed(int value);
    int low_speed() const;
    void high_speed(int value);
    int high_speed() const;
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

    template <typename CharType, typename Traits>
    void message(const std::basic_string_view<CharType, Traits>& m);

    template <typename CharType>
    void message(const CharType* m);

    template <typename CharType>
    void message(const CharType* m, size_t count);

    template <std::ranges::input_range InputRange>
    void message(InputRange&& input_range);

    template <std::input_iterator InputIterator>
    void message(InputIterator begin, InputIterator end);

    template <std::output_iterator<unsigned char> OutputIterator>
    void message(OutputIterator output);

    string_t message() const;
    std::u8string u8message() const;

    template <class Rep, class Period>
    void interval(std::chrono::duration<Rep, Period> interval);

    void interval_seconds(int interval);

    template <Position T>
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

    string_t packet_string_no_message(packet_type p) const;

    string_t packet_string(packet_type p) const;

    std::u8string u8packet_string(packet_type p) const;

    template <std::output_iterator<unsigned char> OutputIterator>
    void packet(packet_type p, OutputIterator output) const;

    template <std::ranges::output_range<unsigned char> OutputRange>
    void packet(packet_type p, OutputRange&& output_range) const;

    bool smart_beaconing_test();

private:
    APRS_TRACK_DETAIL_NAMESPACE_REFERENCE data data_;
    std::vector<APRS_TRACK_DETAIL_NAMESPACE_REFERENCE data> data_list_;
    string_t from_;
    string_t to_;
    string_t path_;
    enum algorithm algorithm_ = algorithm::none;
    std::vector<unsigned char> message_data_;
    size_t message_data_length_ = 0;
    char symbol_code_ = '>';
    char symbol_table_ = '/';
    unsigned int interval_seconds_ = 30;
    unsigned int last_update_seconds = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time;
    int low_speed_ = 5;
    int high_speed_ = 60;
    int slow_rate_ = 60;
    int fast_rate_ = 30;
    int turn_time_ = 15;
    int turn_angle_ = 15;
    int turn_slope_ = 255;
    int ambiguity_ = 0;
    bool aprs_messaging_ = false;
    bool updated_ = false;
    enum mic_e_status mic_e_status_ = mic_e_status::in_service;
};

string_t to_string(mic_e_status status);

string_t to_string(packet_type type);

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

APRS_TRACK_INLINE string_t to_string(mic_e_status status)
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

APRS_TRACK_INLINE string_t to_string(packet_type type)
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
    from_ = f.data();
}

APRS_TRACK_INLINE const string_t& tracker::from() const
{
    return from_;
}

APRS_TRACK_INLINE void tracker::to(std::string_view t)
{
    to_ = t.data();
}

APRS_TRACK_INLINE const string_t& tracker::to() const
{
    return to_;
}

APRS_TRACK_INLINE void tracker::path(std::string_view p)
{
    path_ = p.data();
}

APRS_TRACK_INLINE const string_t& tracker::path() const
{
    return path_;
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

APRS_TRACK_INLINE void tracker::low_speed(int value)
{
    low_speed_ = value;
}

APRS_TRACK_INLINE int tracker::low_speed() const
{
    return low_speed_;
}

APRS_TRACK_INLINE void tracker::high_speed(int value)
{
    high_speed_ = value;
}

APRS_TRACK_INLINE int tracker::high_speed() const
{
    return high_speed_;
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

template <typename CharType, typename Traits>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const std::basic_string_view<CharType, Traits>& m)
{
    message_data_length_ = m.size();

    const unsigned char* data = reinterpret_cast<const unsigned char*>(m.data());
    size_t size = m.size() * sizeof(CharType);
    message_data_.assign(data, data + size);
}

template <typename CharType>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const CharType* m)
{
    message(std::basic_string_view<CharType>(m));
}

template <typename CharType>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(const CharType* m, size_t count)
{
    message(std::basic_string_view<CharType>(m, count));
}

template <std::ranges::input_range InputRange>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(InputRange&& input_range)
{
    message(std::ranges::begin(input_range), std::ranges::end(input_range));
}

template <std::input_iterator InputIterator>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(InputIterator begin, InputIterator end)
{
    message_data_length_ = std::distance(begin, end);
    message_data_.assign(begin, end);
}

template <std::output_iterator<unsigned char> OutputIterator>
APRS_TRACK_INLINE_NO_DISABLE void tracker::message(OutputIterator output)
{
    std::copy(message_data_.begin(), message_data_.end(), output);
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t tracker::message() const
{
    const char* data = reinterpret_cast<const char*>(message_data_.data());
    return string_t(data, message_data_length_);
}

APRS_TRACK_INLINE std::u8string tracker::u8message() const
{
    const char8_t* data = reinterpret_cast<const char8_t*>(message_data_.data());
    return std::u8string(data, message_data_length_);
}

#endif

template <class Rep, class Period>
APRS_TRACK_INLINE_NO_DISABLE void tracker::interval(std::chrono::duration<Rep, Period> interval)
{
    interval_seconds_ = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(interval).count());
}

template <Position T>
APRS_TRACK_INLINE_NO_DISABLE void tracker::position(const T& p)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = p.lat;
    data_.lon = p.lon;

    if constexpr (has_speed<T>)
    {
        data_.speed_knots = meters_s_to_knots(p.speed);
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

APRS_TRACK_INLINE void tracker::interval_seconds(int interval)
{
    interval_seconds_ = interval;
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
    data_.speed_knots = meters_s_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon, double speed_mps, double track_degrees, double alt)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = lat;
    data_.lon = lon;
    data_.speed_knots = meters_s_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
    data_.alt_feet = alt;
}

APRS_TRACK_INLINE void tracker::position(double lat, double lon, double speed_mps, double track_degrees, double alt, int day, int hour, int minute, int second)
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    data_.lat = lat;
    data_.lon = lon;
    data_.speed_knots = meters_s_to_knots(speed_mps);
    data_.track_degrees = track_degrees;
    data_.alt_feet = alt;
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

    data_.speed_knots = meters_s_to_knots(speed_mps);
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

APRS_TRACK_INLINE string_t tracker::packet_string_no_message(packet_type p) const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    string_t packet;

    switch (p)
    {
        case aprs::track::packet_type::mic_e:
            packet = encode_mic_e_packet_no_message(*this, data_);
            break;
        case aprs::track::packet_type::position:
            packet = encode_position_packet_no_timestamp_no_message(*this, data_);
            break;
        case aprs::track::packet_type::position_compressed:
            packet = encode_position_packet_compressed_no_timestamp_no_message(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp:
            packet = encode_position_packet_with_timestamp_dhm_no_message(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp_utc:
            packet = encode_position_packet_with_utc_timestamp_dhm_no_message(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp_utc_hms:
            packet = encode_position_packet_with_utc_timestamp_hms_no_message(*this, data_);
            break;
        default:
            break;
    }

    return packet;
}

APRS_TRACK_INLINE string_t tracker::packet_string(packet_type p) const
{
APRS_TRACK_DETAIL_NAMESPACE_USE

    string_t packet;

    switch (p)
    {
        case aprs::track::packet_type::mic_e:
            packet = encode_mic_e_packet(*this, data_);
            break;
        case aprs::track::packet_type::position:
            packet = encode_position_packet_no_timestamp(*this, data_);
            break;
        case aprs::track::packet_type::position_compressed:
            packet = encode_position_packet_compressed_no_timestamp(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp:
            packet = encode_position_packet_with_timestamp_dhm(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp_utc:
            packet = encode_position_packet_with_utc_timestamp_dhm(*this, data_);
            break;
        case aprs::track::packet_type::position_with_timestamp_utc_hms:
            packet = encode_position_packet_with_utc_timestamp_hms(*this, data_);
            break;
        default:
            break;
    }

    return packet;
}

APRS_TRACK_INLINE std::u8string tracker::u8packet_string(packet_type p) const
{
    string_t packet = packet_string(p);
    std::u8string u8packet(packet.begin(), packet.end());
    return u8packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

template <std::output_iterator<unsigned char> OutputIterator>
APRS_TRACK_INLINE_NO_DISABLE void tracker::packet(packet_type p, OutputIterator output) const
{
    string_t packet = packet_string_no_message(p);

    OutputIterator current = std::copy(reinterpret_cast<const unsigned char*>(packet.data()), reinterpret_cast<const unsigned char*>(packet.data() + packet.size()), output);

    std::copy(message_data_.begin(), message_data_.end(), current);
}

template <std::ranges::output_range<unsigned char> OutputRange>
APRS_TRACK_INLINE_NO_DISABLE void tracker::packet(packet_type p, OutputRange&& output_range) const
{
    packet(p, std::ranges::begin(output_range));
}

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE bool tracker::smart_beaconing_test()
{
    int speed = static_cast<int>(data_.speed_knots.value_or(0));
    int course = static_cast<int>(data_.track_degrees.value_or(0));

    bool result = APRS_TRACK_DETAIL_NAMESPACE_REFERENCE smart_beaconing_test(speed, course, low_speed_, high_speed_, slow_rate_,
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

string_t encode_position_packet_no_timestamp_no_message(const tracker& t, const data& d);
string_t encode_position_packet_no_timestamp(const tracker& t, const data& d);
string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

string_t encode_position_packet_with_timestamp_dhm_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_timestamp_dhm(const tracker& t, const data& d);
string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);
char packet_type_with_timestamp(bool m);

string_t encode_position_packet_with_utc_timestamp_hms_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_hms(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

string_t encode_position_packet_with_utc_timestamp_dhm_no_message(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_dhm(const tracker& t, const data& d);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

string_t encode_position_packet_compressed_no_timestamp_no_message(const tracker& t, const data& d);
string_t encode_position_packet_compressed_no_timestamp(const tracker& t, const data& d);
string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type);
string_t encode_compressed_course_speed(double course_degrees, double speed_knots);
string_t encode_compressed_altitude(double altitude_feet);

string_t encode_mic_e_packet_no_message(const tracker& t, const data& d);
string_t encode_mic_e_packet(const tracker& t, const data& d);
string_t encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity);
string_t encode_mic_e_alt_feet(double alt_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp_no_message(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_no_timestamp_no_message(t.from().c_str(), t.to().c_str(), t.path().c_str(), t.messaging(), d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity()));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        packet.append(encode_course_speed(d.track_degrees.value(), d.speed_knots.value()));
    }

    if (d.alt_feet.has_value())
    {
        packet.append(encode_altitude(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_no_timestamp_no_message(t, d));

    packet.append(t.message());

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm_no_message(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_timestamp_dhm_no_message(t.from().c_str(), t.to().c_str(), t.path().c_str(), packet_type_with_timestamp(t.messaging()), d.day, d.hour, d.minute, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity()));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        packet.append(encode_course_speed(d.track_degrees.value(), d.speed_knots.value()));
    }

    if (d.alt_feet.has_value())
    {
        packet.append(encode_altitude(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_timestamp_dhm_no_message(t, d));

    packet.append(t.message());

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms_no_message(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_hms_no_message(t.from().c_str(), t.to().c_str(), t.path().c_str(), t.messaging(), d.hour, d.minute, d.second, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity()));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        packet.append(encode_course_speed(d.track_degrees.value(), d.speed_knots.value()));
    }

    if (d.alt_feet.has_value())
    {
        packet.append(encode_altitude(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_hms_no_message(t, d));

    packet.append(t.message());

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm_no_message(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_dhm_no_message(t.from().c_str(), t.to().c_str(), t.path().c_str(), t.messaging(), d.day, d.hour, d.minute, d.lat, d.lon, t.symbol_table(), t.symbol_code(), t.ambiguity()));

    if (d.speed_knots.has_value() && d.track_degrees.has_value())
    {
        packet.append(encode_course_speed(d.track_degrees.value(), d.speed_knots.value()));
    }

    if (d.alt_feet.has_value())
    {
        packet.append(encode_altitude(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_dhm_no_message(t, d));

    packet.append(t.message());

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp_no_message(const tracker& t, const data& d)
{
    string_t packet;

    char compression_type = 0b00111000 + 33; // current, RMC, compressed

    packet.append(encode_position_packet_compressed_no_timestamp_no_message(t.from().c_str(), t.to().c_str(), t.path().c_str(), t.messaging(), d.lat, d.lon, t.symbol_table(), t.symbol_code(), d.track_degrees.value_or(0), d.speed_knots.value_or(0), compression_type));

    if (d.alt_feet.has_value())
    {
        packet.append(encode_altitude(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_position_packet_compressed_no_timestamp_no_message(t, d));

    packet.append(t.message());

    return packet;
}

APRS_TRACK_INLINE string_t encode_mic_e_packet_no_message(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_mic_e_packet_no_message(t.from().c_str(), t.path().c_str(), d.lat, d.lon, t.mic_e_status(), d.track_degrees.value_or(0), d.speed_knots.value_or(0), t.symbol_table(), t.symbol_code(), t.ambiguity()));

    if (d.alt_feet.has_value())
    {
        packet.append(encode_mic_e_alt_feet(d.alt_feet.value()));
    }

    return packet;
}

APRS_TRACK_INLINE string_t encode_mic_e_packet(const tracker& t, const data& d)
{
    string_t packet;

    packet.append(encode_mic_e_packet_no_message(t, d));

    packet.append(t.message());

    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

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

#include <string> // for std::string.
#include <string_view> // for std::string_view.
#include <tuple> // for std::tuple and std::tie.
#include <cmath> // for mathematical functions like std::abs, std::modf, and std::round.
#include <cstdio> // for std::snprintf.
#include <cassert>

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
    string_t lat;
    string_t lon;
};

std::tuple<int, int, double> dd_to_dms(double dd);
std::tuple<int, double> dd_to_ddm(double dd);
position_ddm dd_to_ddm(double lat, double lon);
string_t format_number_to_string(double number, int width, int precision);
string_t format_number_to_string(double number, int precision);
string_t format_n_digits_string(int number, int digits);
string_t format_two_digits_string(int number);
position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity);
position_ddm_string to_ddm_short_string(const position_ddm& p);
void add_position_ambiguity(string_t& position, int ambiguity);
string_t encode_compressed_lon(double lon);
string_t encode_compressed_lat(double lat);
string_t encode_compressed_lat_lon(double lat, double lon);

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

APRS_TRACK_INLINE string_t format_number_to_string(double number, int width, int precision)
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

    string_t pretty_number_str;

    if (precision == 0)
    {
        double i;
        double f = std::modf(number, &i);
        (void)f;

        char buffer[32];

        if (width > 0)
        {
            std::snprintf(buffer, sizeof(buffer), "%0*d", width, static_cast<int>(i));
        }
        else
        {
            std::snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(i));
        }

        pretty_number_str = buffer;
    }
    else
    {
        char buffer[32];

        if (width > 0)
        {
            std::snprintf(buffer, sizeof(buffer), "%0*.*f", width + 1, precision, number);
        }
        else
        {
            std::snprintf(buffer, sizeof(buffer), "%.*f", precision, number);
        }

        pretty_number_str = buffer;
    }

    return pretty_number_str;
}

APRS_TRACK_INLINE string_t format_number_to_string(double number, int precision)
{
    return format_number_to_string(number, 0, precision);
}

APRS_TRACK_INLINE string_t format_n_digits_string(int number, int width)
{
    if (width <= 0)
    {
        //return std::to_string(number);
        return string_t(std::to_string(number).c_str());
    }

    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%0*d", width, number);
    return string_t(buffer);
}

APRS_TRACK_INLINE string_t format_two_digits_string(int number)
{
    return format_n_digits_string(number, 2);
}

APRS_TRACK_INLINE position_ddm_string to_ddm_short_string(const position_ddm& p, int ambiguity)
{
    position_ddm_string s;
    s.lat = format_number_to_string(p.lat_d, 2, 0);
    s.lat.append(format_number_to_string(p.lat_m, 4, 2));
    s.lat.append(1, p.lat);
    add_position_ambiguity(s.lat, ambiguity);
    s.lon = format_number_to_string(p.lon_d, 3, 0);
    s.lon.append(format_number_to_string(p.lon_m, 4, 2));
    s.lon.append(1, p.lon);
    return s;
}

APRS_TRACK_INLINE position_ddm_string to_ddm_short_string(const position_ddm& p)
{
    return to_ddm_short_string(p, 0);
}

APRS_TRACK_INLINE void add_position_ambiguity(string_t& position, int ambiguity)
{
    if (!(ambiguity > 0))
    {
        return;
    }

    for (size_t i = position.size() - 2, count = 0; i > 0 && count < static_cast<size_t>(ambiguity); i--)
    {
        if (position[i] == '.')
        {
            continue;
        }
        position[i] = ' ';
        count++;
    }
}

APRS_TRACK_INLINE string_t encode_compressed_lon(double lon)
{
    string_t result;

    long num = static_cast<long>(std::round(190463 * (180 + lon)));

    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));

    return result;
}

APRS_TRACK_INLINE string_t encode_compressed_lat(double lat)
{
    string_t result;

    long num = static_cast<long>(std::round(380926 * (90 - lat)));

    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));
    num /= 91;
    result.insert(result.begin(), static_cast<char>((num % 91) + 33));

    return result;
}

APRS_TRACK_INLINE string_t encode_compressed_lat_lon(double lat, double lon)
{
    string_t result;

    result.append(encode_compressed_lat(lat));
    result.append(encode_compressed_lon(lon));

    return result;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// timestamp                                                        //
//                                                                  //
// **************************************************************** //

string_t encode_timestamp_dhm(int day, int hour, int min);
string_t encode_utc_timestamp_dhm(int day, int hour, int min);
string_t encode_utc_timestamp_hms(int hour, int min, int sec);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_timestamp_dhm(int day, int hour, int min)
{
    string_t dhm;

    dhm.append(format_two_digits_string(day));
    dhm.append(format_two_digits_string(hour));
    dhm.append(format_two_digits_string(min));

    dhm.append("/");

    return dhm;
}

APRS_TRACK_INLINE string_t encode_utc_timestamp_dhm(int day, int hour, int min)
{
    string_t dhm;

    dhm.append(format_two_digits_string(day));
    dhm.append(format_two_digits_string(hour));
    dhm.append(format_two_digits_string(min));

    dhm.append("z");

    return dhm;
}

APRS_TRACK_INLINE string_t encode_utc_timestamp_hms(int hour, int min, int sec)
{
    string_t dhm;

    dhm.append(format_two_digits_string(hour));
    dhm.append(format_two_digits_string(min));
    dhm.append(format_two_digits_string(sec));

    dhm.append("h");

    return dhm;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// header                                                           //
//                                                                  //
// **************************************************************** //

string_t encode_header(std::string_view from, std::string_view to, std::string_view path);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_header(std::string_view from, std::string_view to, std::string_view path)
{
    // Encode the header of a packet:
    //
    // Example:
    //
    //   N0CALL>APRS,TCPIP*,qAC,N0CALL:
    //   ~~~~~~ ~~~~ ~~~~~~~~~~~~~~~~~
    //   from  >to  ,path             :

    string_t packet;

    packet.append(from.data());
    packet.append(1, '>');
    packet.append(to.data());

    if (path.empty() == false)
    {
        packet.append(1, ',');
        packet.append(path.data());
    }

    packet.append(1, ':');

    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position no timestamp                                            //
//                                                                  //
// **************************************************************** //

string_t encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees);
string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet);
string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_data_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
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

    string_t data;

    data.append(1, type);

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    data.append(ddm_str.lat);

    data.append(1, symbol_table);

    data.append(ddm_str.lon);

    data.append(1, symbol_code);

    return data;
}

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, ambiguity));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees)
{
    string_t packet;

    packet.append(encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_course_speed(track_degrees, speed_knots));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_no_timestamp_no_message(from, to, path, messaging, lat, lon, symbol_table, symbol_code, ambiguity, speed_knots, track_degrees));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with timestamp dhm                                      //
//                                                                  //
// **************************************************************** //

string_t encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees);
string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet);
string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_data_with_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
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

    string_t data;

    data.append(1, type);

    data.append(encode_timestamp_dhm(day, hour, min));

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    data.append(ddm_str.lat);

    data.append(1, symbol_table);

    data.append(ddm_str.lon);

    data.append(1, symbol_code);

    return data;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_with_timestamp_dhm(packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees)
{
    string_t packet;

    packet.append(encode_position_packet_with_timestamp_dhm_no_message(from, to, path, packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_course_speed(track_degrees, speed_knots));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_with_timestamp_dhm_no_message(from, to, path, packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_with_timestamp_dhm_no_message(from, to, path, packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_course_speed(track_degrees, speed_knots));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with timestamp hms                                      //
//                                                                  //
// **************************************************************** //

string_t encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet);
string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_data_with_utc_timestamp_hms(char type, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
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

    string_t data;

    data.append(1, type);

    data.append(encode_utc_timestamp_hms(hour, min, sec));

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    data.append(ddm_str.lat);

    data.append(1, symbol_table);

    data.append(ddm_str.lon);

    data.append(1, symbol_code);

    return data;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_with_utc_timestamp_hms(packet_type_with_timestamp(messaging), hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity));
    
    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees)
{
    string_t packet;
    
    packet.append(encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity));
    
    packet.append(encode_course_speed(track_degrees, speed_knots));
    
    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity));
    
    packet.append(encode_altitude(alt_feet));
    
    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_hms_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int hour, int min, int sec, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet)
{
    string_t packet;
   
    packet.append(encode_position_packet_with_utc_timestamp_hms_no_message(from, to, path, messaging, hour, min, sec, lat, lon, symbol_table, symbol_code, ambiguity));
    
    packet.append(encode_course_speed(track_degrees, speed_knots));
    
    packet.append(encode_altitude(alt_feet));
    
    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position with utc timestamp                                      //
//                                                                  //
// **************************************************************** //

string_t encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet);
string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet);
string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double altitude_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_data_with_utc_timestamp_dhm(char type, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
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

    string_t data;

    data.append(1, type);

    data.append(encode_utc_timestamp_dhm(day, hour, min));

    position_ddm ddm = dd_to_ddm(lat, lon);
    position_ddm_string ddm_str = to_ddm_short_string(ddm, ambiguity);

    data.append(ddm_str.lat);

    data.append(1, symbol_table);

    data.append(ddm_str.lon);

    data.append(1, symbol_code);

    return data;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_with_utc_timestamp_dhm(packet_type_with_timestamp(messaging), day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));
    
    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees)
{
    string_t packet;
  
    packet.append(encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));
    
    packet.append(encode_course_speed(track_degrees, speed_knots));
    
    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_with_utc_timestamp_dhm_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, int day, int hour, int min, double lat, double lon, char symbol_table, char symbol_code, int ambiguity, double speed_knots, double track_degrees, double alt_feet)
{
    string_t packet;

    packet.append(encode_position_packet_with_utc_timestamp_dhm_no_message(from, to, path, messaging, day, hour, min, lat, lon, symbol_table, symbol_code, ambiguity));
    
    packet.append(encode_course_speed(track_degrees, speed_knots));
    
    packet.append(encode_altitude(alt_feet));
    
    return packet;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// position compression                                             //
//                                                                  //
// **************************************************************** //

string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code);
string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type);
string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type);
string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type);
string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type);
string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type);
string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type);
string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, double alt_feet);
string_t encode_compressed_course_speed(double course_degrees, double speed_knots);
string_t encode_compressed_altitude(double altitude_feet);
int compression_type_to_int(compression_type type);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code)
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

    string_t data;

    data.append(1, type);

    data.append(1, symbol_table);

    data.append(encode_compressed_lat(lat));
    data.append(encode_compressed_lon(lon));

    data.append(1, symbol_code);

    return data;
}

APRS_TRACK_INLINE string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type)
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

    string_t data;

    data.append(1, type);

    data.append(1, symbol_table);

    data.append(encode_compressed_lat(lat));
    data.append(encode_compressed_lon(lon));

    data.append(1, symbol_code);

    data.append(1, static_cast<char>(compression_type));

    return data;
}

APRS_TRACK_INLINE string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type)
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

    string_t data;

    data.append(1, type);

    data.append(1, symbol_table);

    data.append(encode_compressed_lat(lat));
    data.append(encode_compressed_lon(lon));

    data.append(1, symbol_code);

    data.append(encode_compressed_course_speed(course_degrees, speed_knots));

    data.append(1, static_cast<char>(compression_type));

    return data;
}

APRS_TRACK_INLINE string_t encode_position_data_compressed_no_timestamp(char type, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type)
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

    string_t data;

    data.append(1, type);

    data.append(1, symbol_table);

    data.append(encode_compressed_lat(lat));
    data.append(encode_compressed_lon(lon));

    data.append(1, symbol_code);

    data.append(encode_compressed_altitude(alt_feet));

    data.append(1, static_cast<char>(compression_type));

    return data;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, unsigned char compression_type)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, compression_type));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type)
{
    string_t packet;

    packet.append(encode_header(from, to, path));
    
    packet.append(encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, course_degrees, speed_knots, compression_type));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double alt_feet, unsigned char compression_type)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, alt_feet, compression_type));

    return packet;
}

APRS_TRACK_INLINE string_t encode_position_packet_compressed_no_timestamp_no_message(std::string_view from, std::string_view to, std::string_view path, bool messaging, double lat, double lon, char symbol_table, char symbol_code, double course_degrees, double speed_knots, unsigned char compression_type, double alt_feet)
{
    string_t packet;

    packet.append(encode_header(from, to, path));

    packet.append(encode_position_data_compressed_no_timestamp(packet_type_without_timestamp(messaging), lat, lon, symbol_table, symbol_code, course_degrees, speed_knots, compression_type));

    packet.append(encode_altitude(alt_feet));

    return packet;
}

APRS_TRACK_INLINE string_t encode_compressed_course_speed(double course_degrees, double speed_knots)
{
    string_t course_speed(2, '\0');

    // course degrees is expressed in degrees 0 to 359, clockwise from due north
    // if the value exceeds 359, it is wrapped around to 0
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

APRS_TRACK_INLINE string_t encode_compressed_altitude(double altitude_feet)
{
    int cs = static_cast<int>(std::round(std::log(altitude_feet) / std::log(1.002)));

    int c = cs / 91;
    int s = cs % 91;

    string_t out(2, '\0');

    out[0] = static_cast<char>(c + 33);
    out[1] = static_cast<char>(s + 33);

    return out;
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

string_t encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code);
string_t encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity);
string_t encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, double alt_feet);
void add_mic_e_position_ambiguity(string_t& destination_address, int ambiguity);
void encode_mic_e_status(int a, int b, int c, bool custom, string_t& destination_address);
void encode_mic_e_lat_direction(char direction, string_t& destination_address);
void encode_mic_lon_offset(bool offset, string_t& destination_address);
void encode_mic_lon_direction(char direction, string_t& destination_address);
std::tuple<int, int, int, bool> encode_mic_e_status(mic_e_status status);
void encode_mic_e_status(mic_e_status status, string_t& destination_address);
string_t encode_mic_e_lat(double lat);
string_t encode_mic_e_lat(double lat, mic_e_status status);
string_t encode_mic_e_lat(double lat, double lon, mic_e_status status, int ambiguity);
char encode_mic_e_lon_degrees(int lon_d);
char encode_mic_e_lon_minutes(int lon_m);
char encode_mic_e_lon_hundred_minutes(int lon_h);
string_t encode_mic_e_lon(double lon);
string_t encode_mic_e_course_speed(double course_degrees, double speed_knots);
string_t encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots);
string_t encode_mic_e_alt(double alt_meters);
string_t encode_mic_e_alt_feet(double alt_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE double round_number(double number)
{
    double factor = 10.0;
    return std::round(number * factor) / factor;
}

APRS_TRACK_INLINE string_t encode_mic_e_data(char type, double lat, double lon, double course_degrees, double speed_knots, char symbol_table, char symbol_code)
{
    (void)lat;

    string_t data;

    data.append(1, type);

    data.append(encode_mic_e_lon(lon));
    data.append(encode_mic_e_course_speed(course_degrees, speed_knots));

    data.append(1, symbol_code);
    data.append(1, symbol_table);

    return data;
}

// support type... first character
// 

APRS_TRACK_INLINE string_t encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity)
{
    string_t packet;

    string_t lat_str = encode_mic_e_lat(lat, lon, status, ambiguity);

    packet.append(encode_header(from, lat_str.data(), path));

    packet.append(encode_mic_e_data('`', lat, lon, course_degrees, speed_knots, symbol_table, symbol_code));

    return packet;
}

APRS_TRACK_INLINE string_t encode_mic_e_packet_no_message(std::string_view from, std::string_view path, double lat, double lon, mic_e_status status, double course_degrees, double speed_knots, char symbol_table, char symbol_code, int ambiguity, double alt_feet)
{
    string_t packet;

    packet.append(encode_mic_e_packet_no_message(from, path, lat, lon, status, course_degrees, speed_knots, symbol_table, symbol_code, ambiguity));

    packet.append(encode_mic_e_alt_feet(alt_feet));

    return packet;
}

APRS_TRACK_INLINE void add_mic_e_position_ambiguity(string_t& destination_address, int ambiguity)
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

APRS_TRACK_INLINE void insert_mic_e_type(string_t& data, std::string_view type)
{
    assert(data.size() >= 9);
    data.insert(9, type.data());
}

APRS_TRACK_INLINE void append_mic_e_manufacturer(string_t& packet_or_data, std::string_view manufacturer_version)
{
    assert(manufacturer_version.size() >= 1);
    packet_or_data.append(manufacturer_version.data(), manufacturer_version.size());
}

APRS_TRACK_INLINE void encode_mic_e_status(int a, int b, int c, bool custom, string_t& destination_address)
{
    int message_bits[3] = {a, b, c};

    for (size_t i = 0; i < 3; i++)
    {
        // this handling is not necessary due to how the
        // lat ambiguity is applied later, but it is useful in testing
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

APRS_TRACK_INLINE void encode_mic_e_lat_direction(char direction, string_t& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but it is useful in testing
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

APRS_TRACK_INLINE void encode_mic_lon_offset(bool offset, string_t& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but it is useful in testing
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

APRS_TRACK_INLINE void encode_mic_lon_direction(char direction, string_t& destination_address)
{
    // this handling is not necessary due to how the
    // lat ambiguity is applied later, but it is useful in testing
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

APRS_TRACK_INLINE void encode_mic_e_status(mic_e_status status, string_t& destination_address)
{
    int a = 0;
    int b = 0;
    int c = 0;
    bool custom = false;

    std::tie(a, b, c, custom) = encode_mic_e_status(status);

    encode_mic_e_status(a, b, c, custom, destination_address);
}

APRS_TRACK_INLINE string_t encode_mic_e_lat(double lat)
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

    string_t lat_str(buffer);

    return lat_str;
}

APRS_TRACK_INLINE string_t encode_mic_e_lat(double lat, mic_e_status status)
{
    char direction = (lat >= 0.0) ? 'N' : 'S';

    string_t lat_str = encode_mic_e_lat(lat);

    encode_mic_e_status(status, lat_str);
    encode_mic_e_lat_direction(direction, lat_str);

    return lat_str;
}

APRS_TRACK_INLINE string_t encode_mic_e_lat(double lat, double lon, mic_e_status status, int ambiguity)
{
    string_t lat_str = encode_mic_e_lat(lat, status);

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

APRS_TRACK_INLINE string_t encode_mic_e_lon(double lon)
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

    string_t lon_str;

    double lon_abs = std::fabs(lon);

    int lon_d = static_cast<int>(lon_abs);

    double lon_m = (lon_abs - lon_d) * 60.0;
    double lon_m_f = 0.0;
    double lon_m_i = std::modf(lon_m, &lon_m_f) * 100.0;
    lon_m_i = round_number(lon_m_i);

    lon_str.append(1, encode_mic_e_lon_degrees(lon_d));
    lon_str.append(1, encode_mic_e_lon_minutes(static_cast<int>(lon_m)));
    lon_str.append(1, encode_mic_e_lon_hundred_minutes(static_cast<int>(lon_m_i)));

    return lon_str;
}

APRS_TRACK_INLINE string_t encode_mic_e_course_speed(double course_degrees, double speed_knots)
{
    string_t course_speed;

    int course = static_cast<int>(round_number(course_degrees));
    int speed = static_cast<int>(round_number(speed_knots));

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

    course_speed.append(1, static_cast<char>(sp));
    course_speed.append(1, static_cast<char>(dc));
    course_speed.append(1, static_cast<char>(se));

    return course_speed;
}

APRS_TRACK_INLINE string_t encode_mic_e_course_speed_alternate(double course_degrees, double speed_knots)
{
    string_t course_speed;

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

    course_speed.append(1, static_cast<char>(sp));
    course_speed.append(1, static_cast<char>(dc));
    course_speed.append(1, static_cast<char>(se));

    return course_speed;
}

APRS_TRACK_INLINE string_t encode_mic_e_alt(double alt_meters)
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

    string_t alt_str(4, '\0');

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

APRS_TRACK_INLINE string_t encode_mic_e_alt_feet(double alt_feet)
{
    double alt_meters = alt_feet * 0.3048;
    return encode_mic_e_alt(alt_meters);
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
//  common data extensions                                          //
//                                                                  //
// **************************************************************** //

string_t encode_course_speed(double course_degrees, double speed_knots);
string_t encode_altitude(double alt_feet);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE string_t encode_course_speed(double course_degrees, double speed_knots)
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

    string_t course_speed;
    course_speed.append(format_n_digits_string(course_degrees_int, 3));
    course_speed.append("/");
    course_speed.append(format_n_digits_string(speed_knots_int, 3));
    return course_speed;
}

APRS_TRACK_INLINE string_t encode_altitude(double alt_feet)
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

    string_t altitude;
    altitude.append("/A=");
    altitude.append(format_n_digits_string(alt_feet_int, 6));
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

bool smart_beaconing_test(int speed, int course, int low_speed, int high_speed, int slow_rate, int fast_rate, int turn_time, int turn_angle, int turn_slope, int last_update);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE bool smart_beaconing_test(int speed, int course, int low_speed, int high_speed, int slow_rate, int fast_rate, int turn_time, int turn_angle, int turn_slope, int last_update)
{
    //
    // Smart Beaconing Algorithm (TM)
    // The Smart Beaconing Algorithm (TM) is a trademark of Tony Arnerich and Steve Bragg.
    //
    // Parameters:
    //
    //   speed - speed, unit independent, ex: 50 mph
    //   course - course in degrees, ex: 200 degrees
    //   low_speed - when the speed is lower than "low_speed", the tracker will update (transmit packet) at the slow_interval_seconds
    //   high_speed - when the speed is higher than "high_speed", the tracker will update (transmit packet) at the fast_interval_seconds
    //   slow_rate - seconds, low speed transmission interval time
    //   fast_rate - seconds, high speed transmission interval time
    //   turn_time - the minimum number of degrees the tracker must turn before it will transmit a packet
    //   turn_angle - 
    //   turn_slope - the number of seconds since the last update
    //
    // Returns:
    //
    //   true - if the tracker was updated and should transmit a packet
    //   false - if the tracker was not updated, and no packet should be transmitted
    //
    // References:
    //
    //   https://www.w8wjb.com/qth/QTHHelp/English.lproj/adv-smartbeaconing.html
    //   https://thelifeofkenneth.com/files/thesis_aprs_finnegan_final.pdf
    //   https://n3ujj.com/manuals/SmartBeaconing.pdf
    //   https://github.com/wb2osz/direwolf/blob/master/src/beacon.c

    int interval = 0;

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

        if (course > turn_threshold)
        {
            interval = turn_time;
        }
    }

    if (last_update >= interval)
    {
        return true;
    }

    return false;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

// **************************************************************** //
//                                                                  //
// conversions                                                      //
//                                                                  //
// **************************************************************** //

double meters_to_feet(double meters);
double meters_s_to_knots(double meters_s);

#ifndef APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_INLINE double meters_to_feet(double meters)
{
    return meters * 3.28084;
}

APRS_TRACK_INLINE double meters_s_to_knots(double meters_s)
{
    return meters_s * 1.9438444924406;
}

#endif // APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY

APRS_TRACK_DETAIL_NAMESPACE_END

APRS_TRACK_NAMESPACE_END
