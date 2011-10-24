#include <pficommon/lang/function.h>
#include <pficommon/data/serialization.h>
#include <pficommon/network/rpc.h>
#include <pficommon/network/rpc/reflect.h>

#include <iostream>

#define EXTRACT_RPC_PROC(name, tp)

#define EXTRACT_RPC_GEN(version, base, ...) \
  class base##_ext{			    \
  public:				    \
  base##_ext(){				    \
    std::cout<<(#base)<<std::endl;	    \
  }					    \
  } base##_ext_inst;

#define RPC_PROC_BKUP RPC_PROC
#undef RPC_PROC
#define RPC_PROC EXTRACT_RPC_PROC

#define RPC_GEN_BKUP RPC_GEN
#undef RPC_GEN
#define RPC_GEN EXTRACT_RPC_GEN

#include "<<<SIGFILE>>>"

#undef RPC_GEN
#define RPC_GEN RPC_GEN_BKUP
#undef RPC_GEN_BKUP

#undef RPC_PROC
#define RPC_PROC RPC_PROC_BKUP
#undef RPC_PROC_BKUP

int main()
{
  return 0;
}
