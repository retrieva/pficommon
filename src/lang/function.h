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

#pragma once

#include "shared_ptr.h"
#include "safe_bool.h"

namespace pfi{
namespace lang{

template <class T>
class function{};

#define ARITY function0
#define TARG
#define AARG
#define BARG
#define FARG
#define RARG
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function1
#define TARG ,class T0
#define AARG T0
#define BARG , T0
#define FARG T0 t0
#define RARG t0
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function2
#define TARG , class T0, class T1
#define AARG T0, T1
#define BARG , T0, T1
#define FARG T0 t0, T1 t1
#define RARG t0, t1
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function3
#define TARG , class T0, class T1, class T2
#define AARG T0, T1, T2
#define BARG , T0, T1, T2
#define FARG T0 t0, T1 t1, T2 t2
#define RARG t0, t1, t2
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function4
#define TARG , class T0, class T1, class T2, class T3
#define AARG T0, T1, T2, T3
#define BARG , T0, T1, T2, T3
#define FARG T0 t0, T1 t1, T2 t2, T3 t3
#define RARG t0, t1, t2, t3
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function5
#define TARG , class T0, class T1, class T2, class T3, class T4
#define AARG T0, T1, T2, T3, T4
#define BARG , T0, T1, T2, T3, T4
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4
#define RARG t0, t1, t2, t3, t4
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function6
#define TARG , class T0, class T1, class T2, class T3, class T4, class T5
#define AARG T0, T1, T2, T3, T4, T5
#define BARG , T0, T1, T2, T3, T4, T5
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5
#define RARG t0, t1, t2, t3, t4, t5
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function7
#define TARG , class T0, class T1, class T2, class T3, class T4, class T5, class T6
#define AARG T0, T1, T2, T3, T4, T5, T6
#define BARG , T0, T1, T2, T3, T4, T5, T6
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6
#define RARG t0, t1, t2, t3, t4, t5, t6
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function8
#define TARG , class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7
#define AARG T0, T1, T2, T3, T4, T5, T6, T7
#define BARG , T0, T1, T2, T3, T4, T5, T6, T7
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7
#define RARG t0, t1, t2, t3, t4, t5, t6, t7
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function9
#define TARG , class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8
#define AARG T0, T1, T2, T3, T4, T5, T6, T7, T8
#define BARG , T0, T1, T2, T3, T4, T5, T6, T7, T8
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8
#define RARG t0, t1, t2, t3, t4, t5, t6, t7, t8
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

#define ARITY function10
#define TARG , class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9
#define AARG T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
#define BARG , T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
#define FARG T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9
#define RARG t0, t1, t2, t3, t4, t5, t6, t7, t8, t9
#include "function_def.h"
#undef RARG
#undef FARG
#undef BARG
#undef AARG
#undef TARG
#undef ARITY

} // lang
} // pfi
