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

#include <iostream>
#include <sstream>
#include <complex>

#include "../math/random.h"
#include "../math/vector.h"
#include "../math/algebra_tester.h"
#include "ppm.h"

using namespace std;
using namespace pfi::math::vector::component_by_name;
using namespace pfi::visualization::color;
using namespace pfi::visualization::color::linear_algebra;
using namespace pfi::visualization::ppm;


pfi::math::random::random<pfi::math::random::mersenne_twister> Rand;
const int N=10;


TEST(ppm,test){
  EXPECT_EQ(6*7,42);
}

template<class color>
rgb<color> rand_rgb(){
  return rgb<color>((color)Rand.next_int(256),
                    (color)Rand.next_int(256),
                    (color)Rand.next_int(256));
}


bool near_double(const double &x, const double &y){
  return abs(x-y)<1e-6;
}

bool near_rgb(const rgb<double> &x, const rgb<double> &y){
  return near_double(x.r, y.r) && near_double(x.g, y.g) && near_double(x.b, y.b);
}



TEST_VECTOR_SPACE(rgb,1000,double,rgb<double>,Rand.next_double,rand_rgb<double>,1.0,rgb<double>(0,0,0),near_double,near_rgb)
TEST(rgb,human_check){
  rgb<double> roses(192,0,0), violets(64,0,255);
  cerr << "(" << roses << ") (" << violets << ") (" << (0.8 * roses + 0.2 * violets) << ")" << endl;
}

template<class color>
ppm<color> rand_ppm(image_coordinate_t w=Rand.next_int(50,200),
                    image_coordinate_t h=Rand.next_int(50,200),
                    rgb<color> bc=rgb<color> 
                    (Rand.next_int(256),Rand.next_int(256),Rand.next_int(256))){

  ppm<color> p(w,h,bc);
  for(int y=0;y<p.height();++y){
    for(int x=0;x<p.width();++x){
      p(x,y)=rand_rgb<color>();
    }
  }
  return p;
}

TEST(ppm_test,constructor_consistency){
  for(int w=10;w<1000;w*=2){
    for(int h=10;h<1000;h*=2){
      EXPECT_EQ(rand_ppm<uint8_t>(w,h).width(),w);
      EXPECT_EQ(rand_ppm<uint8_t>(w,h).height(),h);
    }
  }
}

TEST(ppm,bmp_inside_detection){
  for(int n=0;n<N;++n){
    ppm<uint8_t> p(rand_ppm<uint8_t>());
    for(int y=0;y<p.height();++y){
      for(int x=0;x<p.width();++x){
        EXPECT_TRUE(p.in(x,y));
        EXPECT_TRUE(!p.in(x+p.width(),y));
        EXPECT_TRUE(!p.in(x-p.width(),y));
        EXPECT_TRUE(!p.in(x,y+p.height()));
        EXPECT_TRUE(!p.in(x,y-p.height()));
      }
    }
  }
}

TEST(ppm,out_of_range_access_safety){
  for(int n=0;n<N;++n){
    ppm<uint8_t> p=rand_ppm<uint8_t>(), p_orig=p;
    for(int t=0;t<100;++t){
      int x,y; 
      *((uint32_t*)&x)=Rand.next_int();
      *((uint32_t*)&y)=Rand.next_int();
      EXPECT_TRUE(p(x,y)==p.bgcolor);
      p(x,y)=rand_rgb<uint8_t>();
    }

    for(int y=0;y<p.height();++y){
      for(int x=0;x<p.width();++x){
        EXPECT_EQ(p_orig(x,y), p(x,y));
      }
    }
  }
}

TEST(ppm,IO){
  for(int n=0;n<N;++n){
    ppm<uint8_t> p_orig=rand_ppm<uint8_t>();
    p_orig.write("./test.ppm");
    ppm<uint8_t> p2("./test.ppm");
    p2.write("./test2.ppm");
    ppm<uint8_t> p3=rand_ppm<uint8_t>();
    p3.read("./test2.ppm");
    ostringstream oss;
    p3.write(oss);
    istringstream iss(oss.str());
    ppm<uint8_t> p(iss);

    for(int y=0;y<p.height();++y){
      for(int x=0;x<p.width();++x){
        EXPECT_TRUE(p(x,y)==p_orig(x,y));
      }
    }
  }
}

TEST(ppm,access_by_complex_and_vector){
  for(int n=0;n<N;++n){
    ppm<uint8_t> p=rand_ppm<uint8_t>();
    for(int i=0;i<1000;++i){
      double
        x=Rand.next_double(-10,p.width()+10),
        y=Rand.next_double(-10,p.height()+10);
      complex<double> z(x,y);
      vector2<double> pt(x,y);
      EXPECT_TRUE(p(x,y)==p(z));
      EXPECT_TRUE(p(x,y)==p(pt));
    }
  }
}
