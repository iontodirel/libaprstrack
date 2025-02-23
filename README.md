# libaprstrack

C++ header only standalone APRS encoding library for APRS tracking.

This libary is designed for implementing APRS trackers, and contains an implementation of the Smart Beaconing (TM) algorithm.

The library is cross platform, and can run on Windows, Linux, and OSX. It also has been tested on the ESP32 and other embedded platform. 

Automatic Packet Reporting System (APRS) (TM) is a system for real-time digital data communication over ham radio.

See more info at http://www.aprs.org/ and https://groups.io/g/APRS.

SmartBeaconing (TM) is a beaconing algorithm invented by Tony Arnerich KD7TA and Steve Bragg KA9MVA.

APRS is a registered trademark of Bob Bruninga, WB4APR.

## Features

- Comprehensive APRS packet encoding support suitable for APRS tracking:
  - Mic-E position packets with configurable Mic-E Status and optional altitude and course/speed
  - Position packets with/without timestamps (DHM/HMS formats) 
  - Compressed position packets
  - UTC and local timestamp support
  - Position ambiguity control
  - Message/comment field support
- Binary and string support, including UTF-8 support.
- Supports the Smart Beaconing (TM) algorithm, with full configurability.
- Cross platform and Cross Toolchain
  - Runs on Windows, Linux, OSX, ESP32, Pico and Teensy platforms
  - Tested with MSVC, GCC, and Clang toolsets.
- Header only library, no dependencies.
  - Requires C++ 20 and a small subset of the C++ Standard Library.
  - No package manager required!
- Comprehensive tests and examples
  - All functions are comprehensively tested with real world packets and data.
  - The tests provide an important insight into how the APRS protocol works.
  - The tracker is fully tested with data from OSM and Valhalla.
  - Ready-to-use test data sets for APRS encoder verification.
- Modular achitecture
  - No coupling!
  - Low level functions can be (re)used standalone or outside the library. The entire encoding core library can be extracted as-is with two copy-pastes.
  - All the code is testable
  - Packet and data encoding can be used without the tracker class
  - Minimal OO design

## Goals

- Make it easy to implement APRS trackers. The library is designed to be simple and easy to use.
- No coupling, no dependencies, easy to reuse by the community outside the library and its intended use.
  - The APRS encoding is implemented as a set of easily reusable functions, which can be used standalone or outside the library.
  - Individual functions can be copied and used without the rest of the library trivially.
  - Easy to port to other languages or platforms.
  - Clean separation of concerns and between encoding logic and tracking functionality.
- Comprehensive tests and examples to help other implementers test or implement APRS encoders.
  - Democratize APRS through easy to follow tests and examples.
  - Ready-to-reuse test data sets.
- Create a modern C++20 APRS tracker implementation
- Cross Plaform: Windows, Linux, OSX, ESP32, Pico, Teensy

## Examples

### Updating the position and encoding a packet

``` cpp
tracker t;
t.from("N0CALL");
t.mic_e_status(mic_e_status::en_route);
t.path("WIDE1-1");
t.symbol_table('/');
t.symbol_code('>');

struct s
{
    double lat   = 0.0;
    double lon   = 0.0;
    double speed = 0.0;
    double track = 0.0;
    double alt   = 0.0;
};

s data { 51.6145, -0.0485, 3.601, 297.0, 33.00 };

t.position(data);

std::string packet = t.packet_string(packet_type::mic_e);

assert(packet == "N0CALL>UQ3VXW,WIDE1-1:`vZwlh}>/\"48}");
```

### Smart beaconing

An external GNSS client is used in this example to supply position information to the tracker library, there is no coupling between the *gpsd_client* and the library.

``` cpp
// using an GNSS client to supply the position info to the library
gpsd_client gpsd("192.168.1.11", 8888);
gpsd.open();

aprs::track::tracker t;

t.message("hello, world!");
t.algorithm(aprs::track::algorithm::smart_beaconing);
t.message("Hello, world!");
t.symbol_code('I');
t.symbol_table('/');
t.from("N0CALL");
t.to("APRS");
t.path("WIDE1-1,WIDE2-2");
t.interval(std::chrono::seconds(5));
t.ambiguity(0);
t.mic_e_status(aprs::track::mic_e_status::en_route);
t.messaging(false);

t.low_speed(5);
t.high_speed(60);
t.slow_rate(60);
t.fast_rate(10);
t.turn_angle(28);
t.turn_time(30);
t.turn_slope(26);

while (true)
{
    // using an GNSS client to supply the position info to the library
    gnss_info info;
    gpsd.try_get_gps_info(info, gnss_include_info::all, 1000);

    t.position(info);
    t.update();

    if (t.updated())
    {
        std::string packet = t.packet_string(aprs::track::packet_type::mic_e);

        fmt::println("{}", packet);
    }
}
```

### Encoding other packet types

Position packet with UTC DMS timestamp:

``` cpp
std::string packet = t.packet_string(packet_type::position_with_timestamp_utc);
assert(packet == "N0CALL>APRS,WIDE1-1:/181613z3945.07N/07505.12W_");
```

Compressed position packet:

``` cpp
std::string packet = t.packet_string(packet_type::position_compressed);
assert(packet == "N0CALL>APRS,WIDE1-1:!/A2hQ5`8vp!!Y");
```

Other examples are available in the tests.

### Binary and UTF-8 support

The string functions are provided for convenience, the library can be used directly with binary data.

The library is string encoding agnostic, and can work with UTF-8 or any other encoding when encoding packets.

``` cpp
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
std::vector<unsigned char> message_bytes = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 };
t.message(message_bytes);

// get packet bytes, by passing in an output iterator
std::vector<unsigned char> packet_bytes;
t.packet(packet_type::position, std::back_inserter(packet_bytes));

std::string packet_string_from_bytes(packet_bytes.begin(), packet_bytes.end());
assert(packet_string_from_bytes == "N0CALL>APRS,WIDE1-1:!3945.07N/07505.12W_Hello World");
```

### Encoding a mic-e packet:

The library is modular with no internal coupling. Lower level functions in the library can also be used standalone for convenience or utility:

``` cpp
std::string packet = aprs::track::detail::encode_mic_e_packet_no_message("N0CALL", "WIDE1-1", 35.449666666667, 140.2685, mic_e_status::custom6, 257, 5.999, '/', '>', 0, 9.84);
assert(packet == "N0CALL>35CVY8,WIDE1-1:`D,'l^U>/\"3u}");
```

## Testing

The ***tests*** directory contain the tests. `tests/tests.cpp` contains a comprehensive sets of tests, written using the Google Test framework.

Use the tests as examples of how to use the library. The tests also provide a comprehensive insight into how the APRS protocol works, and will help other implementers test or implement APRS solutions.

The ***assets*** directory contains route information generated from OSM and Valhalla, and is used to simulate and test tracking.

## Development

The test project can be opened in Visual Studio or VSCode. And it will work out of the box if the dependencies are installed.

Install the CMake and C++ extensions in VSCode, or the Native Desktop workload inside Visual Studio (*if using Visual Studio*).

On Linux systems, install the dependencies listed in `install_dependencies.sh`, which include a compiler, the CMake build system, and a native build system like make. Example for Debian systems: `apt-get install -y gcc g++ gcc-13 g++-13 clang make cmake ninja-build`.

### Dependencies

This library uses C++ 20, C++ 20 language features, and a small part of the C++ Standard Library. It has no other dependencies.

The library is cross platform, and can run on Windows, Linux, and OSX. The library is supported and tested with MSVC, GCC and Clang toolsets.


### Embedded platforms

#### ESP32

The library has been tested successfully on the ESP32 platform.

#### Raspberry Pi Pico 2

The library has been tested successfully on the Pico 1 and 2 platforms.

Please make sure to set the C++ language standard to C++ 20 in your CMake project:

``` cmake
set(CMAKE_CXX_STANDARD 20)
```

There seems to be a problem on the Pico 2 platform where `round` is not part of the `std` namespace.

This is a bug in the toolchain, and can be fixed by adding this portion of code before the including the header:

``` cpp
namespace std
{
    using ::round;
}
```

#### Teensy

The library has been tested successfully on the Teensy platform.

The Teensy toolchain has to first be configured to use C++ 20. This can be done by updating the `teensy\hardware\avr\1.59.0\boards.txt` file.

Change `gnu++17` to `gnu++20` for your board. This would typically look like this:

``` text
teensy41.build.flags.cpp=-std=gnu++20 -fno-exceptions -fpermissive -fno-rtti -fno-threadsafe-statics -felide-constructors -Wno-error=narrowing
```

If you are getting compilation errors after this change, you likely did not change the language standard in all of the places.

In addition, there seems to be some issues with the C libraries when targeting C++ 20, the compiler could not find implementations for few C functions.

I had to fix it by adding the following code before including the header in the sketch:

``` cpp
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

void _exit(int status) { while (1) {} }
int _close(int file) { return -1; }
int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _read(int file, char *ptr, int len) { return 0; }
int _write(int file, char *ptr, int len) { return len; }
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }
int _sbrk(int incr) { return -1; }
```

These functions should not be needed on an embedded platform anyway.

Lastly, I had to save the libaprstrack.hpp file in the sketch folder in plain ASCII, it seems that this toolchain does not support UTF-8 source files with a BOM.

#### Arduino

At the moment the library is not supported on the Arduino platform.

The Arduino platform has limited C++ 20 support. Depending on the board used, the toolchain might be outdated (2014) and limited to C++ 11. There isn't always C++ standard library availability, and AndroidSTL does not work with the Arduino IDE 2.0.

### Integration with CMake

As this is a header only library, you can simple download the header and use it without anything else.

You can do this in CMake with the following command:

`file(DOWNLOAD
    https://raw.githubusercontent.com/iontodirel/libaprstrack/refs/heads/main/libaprstrack.hpp
    ${CMAKE_SOURCE_DIR}/external/libaprstrack.hpp)`

Include the header in your source file:

`#include "external/libaprstrack.hpp"`

### Compiling in a TU

The library can be compiled in a translation unit (TU) to reduce compile time. This is done by including the header file in a TU, and then using the `#include` directive to include the header file in other source files.

Use `APRS_TRACK_INLINE` and `APRS_TRACK_PUBLIC_FORWARD_DECLARATIONS_ONLY` to control the compilation of the library in a TU.

An example is provided with the tests.

# License

MIT License, Copyright (C) 2025 Ion Todirel

Some of the test data and assets used by tests was obtained from APRS-IS. I do not hold any copyright or license claim over this data, the data is in the public domain.
