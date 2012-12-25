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

#ifndef INCLUDE_GUARD_PFI_VISUALIZATION_MAGICK_H_
#define INCLUDE_GUARD_PFI_VISUALIZATION_MAGICK_H_

#include <string>

#include "ppm.h"
#include "../lang/scoped_ptr.h"

/**
   can load from arbitraly image format to ppm.
*/

namespace pfi{
namespace visualization{
namespace ppm{

namespace detail{

// pimpl for ImageMagick
class magick_image{
private:
  magick_image(const magick_image&);
  magick_image& operator =(const magick_image&);

public:
  magick_image(const char *fname, const std::string &fmt);
  magick_image(const void *dat, size_t size, const std::string &fmt);

  // 2010/6/29 Nobuyuki Kubota <nobu@preferred.jp>
  // This destructor must be implemented in .cpp
  // to avoid pimpl+scoped_ptr problem.
  ~magick_image();

  int rows() const;
  int columns() const;
  int max_color() const;

  void pixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b) const;

private:
  class impl;
  pfi::lang::scoped_ptr<impl> pimpl;
};

} // deatil

template <class color>
class magick_ppm: public pfi::visualization::ppm::ppm<color>{
private:
  std::string format_tag;
public:
  magick_ppm(){}
  magick_ppm(int w,int h):pfi::visualization::ppm::ppm<color>(w,h),
                   format_tag("PPM"){}
  magick_ppm(int w,int h,pfi::visualization::color::rgb<color> c):
    pfi::visualization::ppm::ppm<color>(w,h,c),
    format_tag("PPM"){}
  magick_ppm(const std::string &filename, 
             magick_ppm<color> default_ppm=
             magick_ppm<color>(1,1,pfi::visualization::color::rgb<color>(0,0,0))){
    if(!read(filename)){
      *this=default_ppm;
      this->is_valid_ = false;
    }
  }
  /// load image from Blob
  magick_ppm(const void *data, const size_t size, const std::string &fmt="",
             magick_ppm<color> default_ppm=
             magick_ppm<color>(1,1,pfi::visualization::color::rgb<color>(0,0,0))){
    if(!read(data,size,fmt)){
      *this=default_ppm;
      this->is_valid_ = false;
    }
  }

  virtual ~magick_ppm(){}

  /// read from ImageMacick object.
  virtual bool read(const detail::magick_image &img){
    try {
      *this=magick_ppm(img.columns(),img.rows());
      for(pfi::visualization::ppm::image_coordinate_t y=0;y<this->height();++y){
        for(pfi::visualization::ppm::image_coordinate_t x=0;x<this->width();++x){
          unsigned char r, g, b;
          img.pixel(x, y, r, g, b);
          (*this)(x,y)=pfi::visualization::color::rgb<color>(r, g, b);
        }
      }
      this->set_max_color(img.max_color());
      return this->is_valid_ = true;
    } catch (...) {
      return this->is_valid_ = false;
    }
  }

  /// read from file
  virtual bool read(const std::string &filename, const std::string &format0=""){
    try {
      this->is_valid_ = true;

      // 2010/6/29 Nobuyuki Kubota <nobu@preferred.jp>
      // Due to the bug of gcc4.1.2, following two lines cannot be
      // written as a single line code:
      // return read(detail::magick_image(filename.c_str(), format0));
      // This will call the copy constructor of magick_image.
      detail::magick_image tmp(filename.c_str(), format0);
      return read(tmp);
    } catch (...) {
      return this->is_valid_ = false;
    }
  }

  /// read from blob
  virtual bool read(const void *data, const size_t size, const std::string &format0=""){
    try {
      this->is_valid_ = true;
      detail::magick_image tmp(data, size, format0);
      return read(tmp);
    } catch (...) {
      return this->is_valid_ = false;
    }
  }
  
  /// read from istream
  /// this method is not implemented
  virtual bool read(std::istream&){
    return false;
  }

  /// returns image format
  const std::string format() const{return format_tag;}
};

} // ppm
} // visualization
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_VISUALIZATION_MAGICK_H_
