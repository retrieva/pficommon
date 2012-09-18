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

#include "inserter.h"

#include <vector>

using namespace std;
using namespace pfi::lang;
using namespace pfi::text::xhtml;

namespace pfi{
namespace network{
namespace cgi{

tag_inserter::tag_inserter(const string &name, xhtml_builder &b)
  : name(name)
  , b(b)
{
  b.enter();
}

tag_inserter::~tag_inserter()
{
  pfi::lang::shared_ptr<tag_elem> elem(new tag_elem(name, attrs));
  vector<pfi::lang::shared_ptr<html_elem> > children=b.leave();
  for (int i=0;i<(int)children.size();i++)
    elem->add_child(children[i]);
  b.push(elem);
}

text_inserter::text_inserter(const string &text, xhtml_builder &b)
{
  b.push(pfi::lang::shared_ptr<html_elem>(new text_elem(text)));
}

prim_inserter::prim_inserter(const string &text, xhtml_builder &b)
{
  b.push(pfi::lang::shared_ptr<html_elem>(new prim_elem(text)));
}

} // cgi
} // network
} // pfi
