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

using namespace pfi::data::string;

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_are_empty)
{  const char* p="";
  EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument);
}

TEST(ustring_utf_8_decode_test, utf_8_single_byte_character_is_out_of_range)
{
  for (int c = 0x80; c < 0xBF; c++) {
    std::string str(1u, static_cast<char>(c));
    const char* p=str.c_str();
    EXPECT_THROW({chars_to_uchar(p, p + str.size());}, std::invalid_argument)
      << std::hex << c;
  }
  for (int c = 0xFE; c <= 0xFF; c++) {
    std::string str(1u, static_cast<char>(c));
    const char* p=str.c_str();
    EXPECT_THROW({chars_to_uchar(p, p + str.size());}, std::invalid_argument)
      << std::hex << c;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_5_byte_sequence)
{
  { // min 5-byte sequence
    const char str[] = {0xF8, 0x80, 0x80, 0x80, 0x80, 0x00};
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
  { // max 5-byte sequence
    const char str[] = {0xFB, 0xBF, 0xBF, 0xBF, 0xBF, 0x00};
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_6_byte_sequence)
{
  { // min 6-byte sequence
    const char str[] = {0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00};
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
  { // max 6-byte sequence
    const char str[] = {0xFD, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0x00};
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_end_with_imcomplete_byte_sequence)
{
  {
    const char str[] = {0xE3, 0x00}; // 2 bytes are missing
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
  {
    const char str[] = {0xE3, 0x80, 0x00}; // 1 byte is missing
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_start_byte_not_enough_continuation_bytes)
{
  {
    const char str[] = {0xE3, 0x80, // 1 byte is missing here
                        0xE3, 0x80, 0x81, 0x00};
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_an_overlong_encoding)
{
  { // Example of an overlong ASCII character('/')
    const char str[3][5] = {
      {0xC0, 0xAF, 0x00},
      {0xE0, 0x80, 0xAF, 0x00},
      {0xF0, 0x80, 0x80, 0xAF, 0x00},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
    }
  }
  { // Maximum overlong sequences
    const char str[3][5] = {
      {0xC1, 0xBF, 0x00},
      {0xE0, 0x9F, 0xBF, 0x00},
      {0xF0, 0x8F, 0xBF, 0xBF, 0x00},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
    }
  }
  { // Minimum overlong sequences (NUL character)
    const char str[3][5] = {
      {0xC0, 0x80, 0x00},
      {0xE0, 0x80, 0x80, 0x00},
      {0xF0, 0x80, 0x80, 0x80, 0x00},
    };
    for (size_t i = 0; i < 3; i++) {
      const char* p=str[i];
      EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
    }
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_an_invalid_4_byte_sequence)
{
  {
    const char str[] = {0xF4, 0x8F, 0x8F, 0x8F, 0x00}; // U+10FFFF
    const char* p=str;
    EXPECT_NO_THROW(chars_to_uchar(p, p + strlen(p))) << str;
  }
  {
    const char str[] = {0xF4, 0x90, 0x80, 0x80, 0x00}; // U+110000
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}

TEST(ustring_utf_8_decode_test, utf_8_byte_sequences_have_a_surrogate)
{
  {
    const char str[] = {0xED, 0xA0, 0x80, 0x00}; // U+D800
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
  {
    const char str[] = {0xED, 0xBF, 0xBF, 0x00}; // U+DFFF
    const char* p=str;
    EXPECT_THROW({chars_to_uchar(p, p + strlen(p));}, std::invalid_argument) << str;
  }
}
