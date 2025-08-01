# **************************************************************** #
# libaprstrack - APRS tracking library                             # 
# Version 0.1.0                                                    #
# https://github.com/iontodirel/libaprstrack                       #
# Copyright (c) 2025 Ion Todirel                                   #
# **************************************************************** #
#
# CMakeLists.txt
# 
# MIT License
# 
# Copyright (c) 2025 Ion Todirel
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required (VERSION 3.28)

project ("aprstrack_tests")

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

if(MSVC)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

if(UNIX)
    add_compile_definitions(IS_LINUX_MAC)
endif()

find_package(OpenSSL REQUIRED)

if(OPENSSL_VERSION VERSION_LESS "1.1")
    add_definitions(-DOPENSSL_API_1_0)
else()
    add_definitions(-DOPENSSL_API_1_1)
endif()

include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG master)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(etl GIT_REPOSITORY https://github.com/ETLCPP/etl GIT_TAG 20.40.0)
FetchContent_MakeAvailable(etl)

set(BOOST_INCLUDE_LIBRARIES asio beast)
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_USE_STATIC_LIBS ON)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
  URL_MD5 d55d43218e81ca3d0fc14436b7665bf1
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(boost)

set(CIVETWEB_ENABLE_SSL OFF CACHE BOOL "Disable CivetWeb SSL" FORCE)
set(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "Disable CivetWeb building tests" FORCE)
set(CIVETWEB_ENABLE_WEBSOCKET ON CACHE BOOL "Enable CivetWeb websocket support" FORCE)
set(CIVETWEB_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  civetweb
  GIT_REPOSITORY https://github.com/civetweb/civetweb.git
  GIT_TAG v1.15
)
FetchContent_Populate(civetweb)
enable_testing()

add_library(civetweb STATIC
    ${civetweb_SOURCE_DIR}/src/civetweb.c
    ${civetweb_SOURCE_DIR}/src/CivetServer.cpp
)

target_link_libraries(civetweb PRIVATE ${OPENSSL_LIBRARIES})
target_include_directories(civetweb PUBLIC ${civetweb_SOURCE_DIR}/include ${civetweb_SOURCE_DIR})
target_compile_definitions(civetweb PRIVATE USE_WEBSOCKET)
set(INPUT_TEST_FILE ${CMAKE_SOURCE_DIR}/../assets/packets.json)

add_executable(aprstrack_tests "tests.cpp" "../aprstrack.hpp")
target_link_libraries(aprstrack_tests GTest::gtest_main gtest gtest_main nlohmann_json::nlohmann_json fmt::fmt Boost::asio Boost::beast)

add_definitions(-DINPUT_TEST_FILE="${INPUT_TEST_FILE}")

set_property(TARGET aprstrack_tests PROPERTY CXX_STANDARD 20)

if(MSVC)
    target_compile_options(aprstrack_tests PRIVATE /W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(aprstrack_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_executable (aprstrack_use_in_tu "use_in_tu.h" "use_in_tu.cpp" "use_in_other_tu.cpp" "compile_in_tu.cpp")
set_property(TARGET aprstrack_use_in_tu PROPERTY CXX_STANDARD 20)

add_executable (aprstrack_route_test "routing_test.cpp")
target_include_directories(aprstrack_route_test PRIVATE ${civetweb_SOURCE_DIR}/include ${civetweb_SOURCE_DIR})
target_link_libraries(aprstrack_route_test nlohmann_json::nlohmann_json fmt::fmt Boost::asio civetweb)
set_property(TARGET aprstrack_route_test PROPERTY CXX_STANDARD 20)

add_executable (generate_test_json "generate_test_json.cpp")
target_link_libraries(generate_test_json nlohmann_json::nlohmann_json fmt::fmt Boost::asio Boost::beast)
set_property(TARGET generate_test_json PROPERTY CXX_STANDARD 20)

add_executable(aprstrack_basic_periodic_test "basic_periodic_test.cpp" "../aprstrack.hpp")
target_link_libraries(aprstrack_basic_periodic_test GTest::gtest_main gtest gtest_main nlohmann_json::nlohmann_json fmt::fmt Boost::asio Boost::beast)
set_property(TARGET aprstrack_basic_periodic_test PROPERTY CXX_STANDARD 20)

add_executable(aprstrack_with_etl_string_test "with_etl_string.cpp" "../aprstrack.hpp")
set_property(TARGET aprstrack_with_etl_string_test PROPERTY CXX_STANDARD 20)
target_link_libraries(aprstrack_with_etl_string_test PRIVATE etl::etl GTest::gtest_main gtest gtest_main)

add_custom_target(run_generate_test_json
   COMMAND perl ${CMAKE_SOURCE_DIR}/../scripts/parse_packets.pl --input ${CMAKE_SOURCE_DIR}/../assets/mic_e_packets.txt ${CMAKE_SOURCE_DIR}/../assets/position_packets.txt --output ${CMAKE_SOURCE_DIR}/../assets/mic_e_packets.json ${CMAKE_SOURCE_DIR}/../assets/position_packets.json
   COMMAND generate_test_json ${CMAKE_SOURCE_DIR}/../assets/mic_e_packets.json ${CMAKE_SOURCE_DIR}/../assets/position_packets.json ${CMAKE_SOURCE_DIR}/../assets/packets.json
   DEPENDS generate_test_json
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   COMMENT "Running parse_packets.pl and generate_test_json with specified input files"
)

include(GoogleTest)

gtest_discover_tests(aprstrack_tests)
gtest_discover_tests(aprstrack_basic_periodic_test)
gtest_discover_tests(aprstrack_with_etl_string_test)
