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

#ifndef INCLUDE_GUARD_PFI_LANG_SHARED_PTR_H_
#define INCLUDE_GUARD_PFI_LANG_SHARED_PTR_H_

#include <algorithm> // std::swap

#include "safe_bool.h"

namespace pfi{
namespace concurrent{
namespace threading_model{

class single_thread{
public:
  class lock{
  public:
    lock(const single_thread &){}
  };
};

} // threading_model
} // concurrent
} // pfi

namespace pfi{
namespace lang{

template <class TM>
class ref_obj : private TM {
public:
  ref_obj() :cnt(0) {}
  
  void inc(){
    typename TM::lock lk(*this);
    cnt++;
  }
  int dec(){
    typename TM::lock lk(*this);
    return --cnt;
  }
  
  int cur() const{
    return cnt;
  }
  
private:
  int cnt;
};

// shared_pt is not thread-safe.
// if you need to shared_ptr in multi-thread,
// you must specify suitable threading model

template <class T, class TM = pfi::concurrent::threading_model::single_thread>
class shared_ptr : public safe_bool<shared_ptr<T> > {
  template <class Y, class Z> friend class shared_ptr;

public:
  shared_ptr() :ref(NULL), p(NULL) {}

  template <class Y>
  explicit shared_ptr(Y *p) :ref(new ref_obj<TM>()), p(p){
    inc();
  }

  shared_ptr(const shared_ptr &p)
    : safe_bool<shared_ptr<T> >(*this) // avoid warning for g++<=4.1.x
    , ref(p.ref), p(p.p){
    inc();
  }

  template <class Y>
  shared_ptr(const shared_ptr<Y, TM> &p) :ref(p.ref), p(p.p){
    inc();
  }

  ~shared_ptr(){
    dec();
  }

  shared_ptr &operator=(const shared_ptr &r){
    shared_ptr s(r);
    swap(s);
    return *this;
  }

  template <class Y>
  shared_ptr<T> &operator=(const shared_ptr<Y, TM> &r){
    shared_ptr s(r);
    swap(s);
    return *this;
  }

  void reset(){
    set_ptr(NULL,NULL);
  }

  template<class Y>
  void reset(Y *r){
    set_ptr(new ref_obj<TM>(),r);
  }

  T &operator*() const { return *get(); }
  T *operator->() const { return get(); }
  T *get() const { return p; }

  bool unique() const {
    return use_count()==1;
  }

  int use_count() const {
    if (ref==NULL) return 0;
    return ref->cur();
  }

  bool bool_test() const {
    return p!=NULL;
  }

  void swap(shared_ptr &r){
    std::swap(ref,r.ref);
    std::swap(p,r.p);
  }

private:

  void set_ptr(ref_obj<TM> *a, T* b){
    dec();
    ref=a;
    p=b;
    inc();
  }

  void inc(){
    if (ref) ref->inc();
  }
  void dec(){
    if (ref){
      if (ref->dec()==0){
	delete ref;
	if (p) delete p;
	ref=NULL;
	p=NULL;
      }
    }
  }

  ref_obj<TM> *ref;
  T *p;
};

template <class T, class U>
bool operator==(const shared_ptr<T> &a, const shared_ptr<U> &b)
{
  return reinterpret_cast<U*>(a.get())==b.get();
}

template <class T, class U>
bool operator!=(const shared_ptr<T> &a, const shared_ptr<U> &b)
{
  return reinterpret_cast<U*>(a.get())!=b.get();
}

template <class T, class U>
bool operator<(const shared_ptr<T> &a, const shared_ptr<U> &b)
{
  return reinterpret_cast<U*>(a.get())<b.get();
}

template <class T>
void swap(shared_ptr<T> &a, shared_ptr<T> &b)
{
  a.swap(b);
}

} // lang
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_LANG_SHARED_PTR_H_
