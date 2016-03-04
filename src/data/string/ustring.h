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

#ifndef INCLUDE_GUARD_PFI_DATA_STRING_USTRING_H_
#define INCLUDE_GUARD_PFI_DATA_STRING_USTRING_H_

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <stdint.h>
#include <stdexcept>

namespace pfi {
namespace data {
namespace string {

typedef uint32_t uchar;

class ustring : public std::basic_string<uchar> {
public:
  ustring(){}
  ustring(const ustring &s, size_type pos = 0, size_type n = npos)
    : std::basic_string<uchar>(s, pos, n) {}
  ustring(const uchar *p)
    : std::basic_string<uchar>(p) {}
  ustring(const uchar *p, size_type n)
    : std::basic_string<uchar>(p, n) {}
  ustring(size_type n, uchar c)
    : std::basic_string<uchar>(n, c) {}

  template <class InputIterator>
  ustring(InputIterator first, InputIterator last)
    : std::basic_string<uchar>(first, last) {}

  ustring(const std::basic_string<uchar> &str)
    : std::basic_string<uchar>(str) {}
};

// string <-> ustring conversion
ustring string_to_ustring(const char* p);
ustring string_to_ustring(const std::string& s);
std::string ustring_to_string(const ustring& us);

// get first character of string
uchar string_to_uchar(const char* p);
uchar string_to_uchar(const std::string& s);
std::string uchar_to_string(uchar uc);

class fallback_base {
public:
  virtual ~fallback_base() {
  }

  virtual uchar fallback(const std::string& bytes,
                         const char* hint) = 0;
};

class exception_fallback : public fallback_base {
public:
  virtual ~exception_fallback() {
  }

  virtual uchar fallback(const std::string& bytes,
                         const char* hint) {
    if (bytes.empty()) {
      return 0x0000;
    }
    throw std::invalid_argument(hint);
  }
};

class replacement_fallback : public fallback_base {
public:
  replacement_fallback(uchar replace) : replace(replace) {
  }

  virtual ~replacement_fallback() {
  }

  virtual uchar fallback(const std::string& bytes,
                         const char* hint) {
    if (bytes.empty()) {
      return 0x0000;
    }
    return replace;
  }

private:
  uchar replace;
};

namespace detail {
template <class InputIterator1, class InputIterator2>
uchar chars_to_uchar_impl(InputIterator1& in, InputIterator2 end,
                          fallback_base& fb)
{
  const InputIterator1 begin = in;
  if (in == end) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 byte sequences are empty");
  }

  if (((*in) & 0x80) == 0) // U+0000 to U+007F
    return *in++;

  const unsigned c = *in++ & 0xFF;
  // It is an invalid byte also when c is 0xC0 or 0xC1.
  // But it could only be used for an overlong encoding of ASCII characters,
  // so it will be checked together after.
  if (c < 0xC0 || c > 0xFD) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 first byte of character is out of range. "
                       "It must not be in range of [0x80, 0xBF] or [0xFE, 0xFF]");
  }

  static const uchar head_masks[] = { 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
  static const uchar tail_masks[] = { 0x1F, 0x0F, 0x07, 0x03, 0x01 };
  static const uchar flag_bits[] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

  uchar ret = 0;
  int nbytes = 0;

  for (size_t i = 0; i < sizeof(head_masks)/sizeof(head_masks[0]); ++i)
    if ((c & head_masks[i]) == flag_bits[i]) {
      ret = c & tail_masks[i];
      nbytes = i + 2;
      break;
    }

  for (int i = 1; i < nbytes; ++i) {
    if (in == end) {
      return fb.fallback(std::string(begin, in),
                         "Invalid UTF-8: UTF-8 byte sequences end with incomplete byte sequence");
    }
    if ((*in & 0xC0) != 0x80) {
      return fb.fallback(std::string(begin, in),
                         "Invalid UTF-8: UTF-8 byte sequences have a start byte not followed by enough continuation bytes");
    }
    ret <<= 6;
    ret |= *in++ & 0x3F;
  }

  if (nbytes >= 5) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 byte sequences have a 5-byte or 6-byte sequence");
  }

  static const uchar mins[] = { 0, 0, 0x80, 0x800, 0x10000 };
  static const uchar maxs[] = { 0, 0, 0x7FF, 0xFFFF, 0x1FFFFF };

  if (ret < mins[nbytes] || ret > maxs[nbytes]) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 byte sequences have an overlong encoding");
  }

  if (ret > 0x10FFFF) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 byte sequences have an invalid 4-byte sequence. "
                       "It decodes to a value greater than U+10FFFF");
  }

  if (0xD800 <= ret && ret <= 0xDFFF) {
    return fb.fallback(std::string(begin, in),
                       "Invalid UTF-8: UTF-8 byte sequences have a surrogate. "
                       "It must not be in range of [0xD800, 0xDFFF]");
  }

  return ret;
}

struct dummy_end_iterator {};

template <class Iterator>
bool operator==(Iterator, dummy_end_iterator)
{
    return false;
}
}

#ifdef __GNUG__
template <class InputIterator>
uchar chars_to_uchar(InputIterator&) __attribute__((deprecated));
#endif

// char[] -> uchar conversion
template <class InputIterator>
uchar chars_to_uchar(InputIterator& in)
{
  replacement_fallback fb(0xFFFD);
  return detail::chars_to_uchar_impl(in, detail::dummy_end_iterator(), fb);
}

template <class InputIterator>
uchar chars_to_uchar(InputIterator& in, InputIterator end)
{
  replacement_fallback fb(0xFFFD);
  return detail::chars_to_uchar_impl(in, end, fb);
}

template <class InputIterator>
uchar chars_to_uchar(InputIterator& in, InputIterator end, fallback_base& fb)
{
  return detail::chars_to_uchar_impl(in, end, fb);
}

// uchar -> char[] conversion
template <class OutputIterator>
void uchar_to_chars(uchar c, OutputIterator &out){
  char b;
  if(c <= 0x007f) {
    b = c;
    *out++ = b;
  } else if (0x0080 <= c && c <= 0x07ff) {
    b = c >> 6;
    *out++ = (b | 0xc0);
    b = c & 0x3f;
    *out++ = b | 0x80;
  } else if (0x0800 <= c && c <= 0xffff) {
    b = c >> 12;
    *out++ = (b | 0xe0);
    b = (c >> 6) & 0x3f;
    *out++ = b | 0x80;
    b = c & 0x3f;
    *out++ = b | 0x80;
  } else if (0x10000 <= c && c <= 0x10ffff) {
    b = c >> 18;
    *out++ = (b | 0xf0);
    b = (c >> 12) & 0x3f;
    *out++ = b | 0x80;
    b = (c >> 6) & 0x3f;
    *out++ = b | 0x80;
    b = c & 0x3f;
    *out++ = b | 0x80;
  }
}

// utility
bool is_basic_latin(uchar uc);
bool is_alnum(uchar uc);
bool is_hiragana(uchar uc);
bool is_katakana(uchar uc);
bool is_kanji(uchar uc);
bool is_hankaku(uchar uc);

std::string sanitize_utf8(const std::string& s);
ustring hankaku_to_zenkaku(const ustring& us);
ustring zenkaku_latin_to_basic_latin(const ustring& us);
ustring basic_latin_to_zenkaku_latin(const ustring& us);
uchar zenkaku_latin_to_basic_latin(uchar uc);
uchar basic_latin_to_zenkaku_latin(uchar uc);

std::ostream& operator<<(std::ostream& out, const ustring &str);
std::istream& operator>>(std::istream& in , ustring &str);

} // string
} // data
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_DATA_STRING_USTRING_H_
