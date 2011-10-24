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

#include "mmapper.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "syscall.h"

using namespace std;

namespace pfi{
namespace system{
namespace mmapper{

int mmapper::open(const string& filename){
  int r;
  close();

  NO_INTR(fd, ::open(filename.c_str(), O_RDWR));
  if (FAILED(fd)) return -1;

  struct stat st_buf;
  NO_INTR(r, fstat(fd, &st_buf));
  if (FAILED(fd)) { close(); return -1; }
  length = st_buf.st_size;

  int prot = PROT_WRITE | PROT_READ;
  void *p = NULL;
  NO_INTR(p, mmap(NULL, length, prot, MAP_SHARED, fd, 0));
  if (p == MAP_FAILED) { close(); return -1; }
  ptr = (char*)p;

  return 0;
}

int mmapper::close(){
  if (fd >= 0) {
    ::close(fd);
    fd = -1;
  }
  if (ptr != NULL) {
    munmap(ptr, length);
    ptr = NULL;
  }
  return 0;
}

}
}
}
