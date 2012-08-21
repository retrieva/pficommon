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

#ifndef INCLUDE_GUARD_PFI_VISUALIZATION_PPM_H_
#define INCLUDE_GUARD_PFI_VISUALIZATION_PPM_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <stdint.h>

#include "color.h"
#include "../math/vector.h"


namespace pfi{
namespace visualization{
namespace ppm{

typedef int image_coordinate_t;

  /**
     full color ppm format loader/writer and image class.
     pixel data is 2d vector, complex or two image_coordinate_t.
   */
template <class quantum>
class ppm{

private:
  quantum my_max_color;  
  image_coordinate_t w, h;

  std::vector<color::rgb<quantum> > bmp;
  mutable pfi::visualization::color::rgb<quantum> out_of_range_quantum;

protected:
  bool is_valid_;
public:
  void set_is_valid(bool v) { is_valid_ = v; }
  bool is_valid() { return is_valid_;}
  
public:
  /// return color of out of range
  color::rgb<quantum> bgcolor; 

  image_coordinate_t width() const{return w;}

  image_coordinate_t height() const{return h;}
  
  bool in(const image_coordinate_t &x, const image_coordinate_t &y) const{ 
    return x>=0 && y>=0 && x<w && y<h;
  }
  template<class r>
  bool in(const pfi::math::vector::component_by_name::vector2<r> &p) const{
    return in((image_coordinate_t)p.x, (image_coordinate_t)p.y);
  }
  template<class r>
  bool in(const std::complex<r> &p) const{
    return in((image_coordinate_t)p.real(), (image_coordinate_t)p.imag());
  }

  const color::rgb<quantum> &operator()(image_coordinate_t x,image_coordinate_t y) const{
    if(!in(x,y)) {
      out_of_range_quantum = bgcolor;
      return out_of_range_quantum;
    }
    return bmp[y*w+x];
  }
  color::rgb<quantum> &operator()(image_coordinate_t x,image_coordinate_t y){
    if(!in(x,y)) {
      out_of_range_quantum = bgcolor;
      return out_of_range_quantum;
    }
    return bmp[y*w+x];
  }

  template<class r>
  const color::rgb<quantum> &operator()(const pfi::math::vector::component_by_name::vector2<r> &p) const{
    return operator()((image_coordinate_t)p.x,(image_coordinate_t)p.y);
  }
  template<class r>
  color::rgb<quantum> &operator()(const pfi::math::vector::component_by_name::vector2<r> &p){
    return operator()((image_coordinate_t)p.x,(image_coordinate_t)p.y);
  }
  template<class r>
  const color::rgb<quantum> &operator()(const std::complex<r> &p) const{
    return operator()((image_coordinate_t)p.real(),(image_coordinate_t)p.imag());
  }
  template<class r>
  color::rgb<quantum> &operator()(const std::complex<r> &p){
    return operator()((image_coordinate_t)p.real(),(image_coordinate_t)p.imag());
  }

  /// maximum value of color
  const quantum max_color() const{return my_max_color;}

  /// set maximum value of color
  void set_max_color(quantum mc) {my_max_color = mc;}

  /// set maximum value of color and rescale.
  void rescale_max_color(quantum mc) {
    double scale = mc / my_max_color;
    for(int i = 0; i < bmp.size(); ++i){
      bmp[i].r = quantum(bmp[i].r * scale);
      bmp[i].g = quantum(bmp[i].g * scale);
      bmp[i].b = quantum(bmp[i].b * scale);
    }
    my_max_color = mc;
  }

  ppm() : is_valid_(false) {}

  /// create w * h bitmap
  ppm(int w,int h):w(w), h(h), bmp(w*h), is_valid_(false) {}

  /// create w * h bitmap of specified color
  ppm(int w,int h,color::rgb<quantum> c):w(w), h(h), bmp(w*h,c), is_valid_(true), bgcolor(c){}

  /// load specified image file
  /// if it fails, image become to default
  ppm(const std::string &filename, 
      ppm<quantum> default_ppm=ppm<quantum>(1,1,color::rgb<quantum>(0,0,0))){
    is_valid_ = true;
    std::ifstream cin(filename.c_str());
    if(!cin || !read(cin)){
      *this=default_ppm;
      is_valid_ = false;
    }
  }

  /// load image from istream.
  /// same as above
  ppm(std::istream &cin, 
      ppm<quantum> default_ppm=ppm<quantum>(1,1,color::rgb<quantum>(0,0,0))){
    is_valid_ = true;
    if(!cin || !read(cin)){
      *this=default_ppm;
      is_valid_ = false;
    }
  }

  virtual ~ppm(){}

  /// load image from istream
  bool read(std::istream &cin){
    int header_count=0;
    std::string format_str; quantum c_m;
    while(header_count < 3){
      std::string line;
      if(!cin) return is_valid_ = false;
      getline(cin,line);
      if(line[0]=='#'){
        continue;
      }else{
        std::istringstream iss(line);
        switch(header_count++){
        case 0:
          iss>>format_str;
          break;
        case 1:
          iss >> w >> h;
          break;
        case 2:
          iss >> c_m;
          my_max_color = c_m;
          break;
        default:
#ifdef TEST
          EXPECT_TRUE(false) << "ppm header parsing algorithm wrong" << std::endl;
#endif
          break;
        }
      }
    }

    bmp.clear();
    for(int y=0;y<h;++y){
      for(int x=0;x<w;++x){
        quantum r,g,b;
        if(!cin) return is_valid_ = false;
        r=(quantum)cin.get();
        if(!cin) return is_valid_ = false;
        g=(quantum)cin.get();
        if(!cin) return is_valid_ = false;
        b=(quantum)cin.get();
        bmp.push_back(color::rgb<quantum>(r,g,b));
      }
    }
    this->set_max_color(255);
    return is_valid_ = true;
  }
  /// load image from file
  bool read(const std::string &filename){
    std::ifstream cin(filename.c_str());
    return read(cin);
  }

  /// write PPM binary ('P6' format) to ostream
  void write(std::ostream &cout){

    cout << "P6" << std::endl 
         << w << " " << h << std::endl 
         << my_max_color << std::endl;
    for(size_t i=0;i<bmp.size();++i){
      cout.put(__nc(bmp[i].r));
      cout.put(__nc(bmp[i].g));
      cout.put(__nc(bmp[i].b));
    }
  }

  /// write PPM binary to file.
  void write(const std::string &filename){
    std::ofstream cout(filename.c_str());
    write(cout);
  }
private:
  uint8_t __nc(quantum c){
    if(c<0) c=0;
    if(c>255) c=255;
    return uint8_t(c);
  }
};

}}}//namespaces
#endif // #ifndef INCLUDE_GUARD_PFI_VISUALIZATION_PPM_H_
