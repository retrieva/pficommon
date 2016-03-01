// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <gtest/gtest.h>

#include "./ustring.h"

#include <sstream>
#include <cstdio>

using namespace pfi::data::string;

std::string FormatByteSequence(const char* str)
{
  if (!str) {
    return "";
  }
  std::ostringstream oss;
  for (const char* p = str; *p != '\0'; p++) {
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "\\x%02X", (0xFF & *p));
    oss << buffer;
  }
  return oss.str();
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_are_empty)
{
  const char* p="";
  EXPECT_THROW({
      exception_fallback fb;
      chars_to_uchar(p, p + strlen(p), fb);
    }, std::invalid_argument);
}

TEST(ustring_utf_8_decode_test, utf_8_single_byte_character_is_out_of_range)
{
  for (int c = 0x80; c < 0xBF; c++) {
    std::string str(1u, static_cast<char>(c));
    const char* p=str.c_str();
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + str.size(), fb);
      }, std::invalid_argument) << FormatByteSequence(str.c_str());
  }
  for (int c = 0xFE; c <= 0xFF; c++) {
    std::string str(1u, static_cast<char>(c));
    const char* p=str.c_str();
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + str.size(), fb);
      }, std::invalid_argument) << FormatByteSequence(str.c_str());
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_5_byte_sequence)
{
  { // min 5-byte sequence
    const char str[] = {'\xF8', '\x80', '\x80', '\x80', '\x80', '\x00'};
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
  { // max 5-byte sequence
    const char str[] = {'\xFB', '\xBF', '\xBF', '\xBF', '\xBF', '\x00'};
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_6_byte_sequence)
{
  { // min 6-byte sequence
    const char str[] = {'\xFC', '\x80', '\x80', '\x80', '\x80', '\x00'};
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
  { // max 6-byte sequence
    const char str[] = {'\xFD', '\xBF', '\xBF', '\xBF', '\xBF', '\x00'};
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_end_with_imcomplete_byte_sequence)
{
  {
    const char str[] = {'\xE3', '\x00'}; // 2 bytes are missing
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
  {
    const char str[] = {'\xE3', '\x80', '\x00'}; // 1 byte is missing
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_start_byte_not_enough_continuation_bytes)
{
  {
    const char str[] = {'\xE3', '\x80', // 1 byte is missing here
                        '\xE3', '\x80', '\x81', '\x00'};
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_an_overlong_encoding)
{
  { // Example of an overlong ASCII character('/')
    const char str[3][5] = {
      {'\xC0', '\xAF', '\x00'},
      {'\xE0', '\x80', '\xAF', '\x00'},
      {'\xF0', '\x80', '\x80', '\xAF', '\x00'},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({
          exception_fallback fb;
          chars_to_uchar(p, p + strlen(p), fb);
        }, std::invalid_argument) << FormatByteSequence(str[i]);
    }
  }
  { // Minimum overlong sequences (NUL character)
    const char str[3][5] = {
      {'\xC0', '\x80', '\x00'},
      {'\xE0', '\x80', '\x80', '\x00'},
      {'\xF0', '\x80', '\x80', '\x80', '\x00'},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({
          exception_fallback fb;
          chars_to_uchar(p, p + strlen(p), fb);
        }, std::invalid_argument) << FormatByteSequence(str[i]);
    }
  }
  { // Maximum overlong sequences
    const char str[3][5] = {
      {'\xC1', '\xBF', '\x00'},
      {'\xE0', '\x9F', '\xBF', '\x00'},
      {'\xF0', '\x8F', '\xBF', '\xBF', '\x00'},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({
          exception_fallback fb;
          chars_to_uchar(p, p + strlen(p), fb);
        }, std::invalid_argument) << FormatByteSequence(str[i]);
    }
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_an_invalid_4_byte_sequence)
{
  {
    const char str[] = {'\xF4', '\x8F', '\x8F', '\x8F', '\x00'}; // U+10FFFF
    const char* p=str;
    EXPECT_NO_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }) << FormatByteSequence(str);
  }
  {
    const char str[] = {'\xF4', '\x90', '\x80', '\x80', '\x00'}; // U+110000
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_surrogate)
{
  {
    const char str[] = {'\xED', '\xA0', '\x80', '\x00'}; // U+D800
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
  {
    const char str[] = {'\xED', '\xBF', '\xBF', '\x00'}; // U+DFFF
    const char* p=str;
    EXPECT_THROW({
        exception_fallback fb;
        chars_to_uchar(p, p + strlen(p), fb);
      }, std::invalid_argument) << FormatByteSequence(str);
  }
}
