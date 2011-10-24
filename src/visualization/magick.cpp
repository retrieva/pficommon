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

#include "magick.h"

#include <Magick++.h>

namespace pfi{
namespace visualization{
namespace ppm{

namespace detail{

class magick_image::impl{
public:
  impl(const char *fname, const std::string &fmt){
    if (fmt.size()>0)
      img.magick(fmt);
    img.read(fname);
  }

  impl(const void *dat, size_t size, const std::string &fmt){
    if (fmt.size()>0)
      img.magick(fmt);
    img.read(Magick::Blob(dat, size));
  }

  int rows() const{
    return img.rows();
  }

  int columns() const{
    return img.columns();
  }

  int max_color() const{
    return 255;
  }

  void pixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b) const{
    Magick::PixelPacket c=img.pixelColor(x, y);
    // Quantum Depth is 8 or 16
#if QuantumDepth == 16
    r=c.red>>8;
    g=c.green>>8;
    b=c.blue>>8;
#else
    r=c.red;
    g=c.green;
    b=c.blue;
#endif
  }

private:
  Magick::Image img;
};

magick_image::magick_image(const char *fname, const std::string &fmt)
  :pimpl(new impl(fname, fmt))
{
}

magick_image::magick_image(const void *dat, size_t size, const std::string &fmt)
  :pimpl(new impl(dat, size, fmt))
{
}

magick_image::~magick_image()
{
}

int magick_image::rows() const
{
  return pimpl->rows();
}

int magick_image::columns() const
{
  return pimpl->columns();
}

int magick_image::max_color() const
{
  return pimpl->max_color();
}

void magick_image::pixel(
  int x, int y,
  unsigned char &r, unsigned char &g, unsigned char &b) const
{
  pimpl->pixel(x, y, r, g, b);
}

} // detail

} // ppm
} // visualization
} // pfi
