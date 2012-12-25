#include "/usr/local/share/pficommon/gen_php.h"
#include "/usr/local/share/pficommon/gen_hs.h"
#include "/usr/local/share/pficommon/gen_null.h"

#define RPC_PROC_BKUP RPC_PROC
#undef RPC_PROC
#define RPC_PROC REFLECT_RPC_PROC

#define RPC_GEN_BKUP RPC_GEN
#undef RPC_GEN
#define RPC_GEN REFLECT_RPC_GEN

#include "<<<SIGFILE>>>"

#undef RPC_GEN
#define RPC_GEN RPC_GEN_BKUP
#undef RPC_GEN_BKUP

#undef RPC_PROC
#define RPC_PROC RPC_PROC_BKUP
#undef RPC_PROC_BKUP

#include <fstream>
using namespace std;

struct info{
  const char *id;
  const char *suffix;
  void (*f)(vector<reflect_base*> &, const string &);
}infos[]={
  {"php",     "php",  &php_generator::generate},
  {"haskell", "hs",   &haskell_generator::generate},
  {"ocaml",   "ml",   &null_generator::generate},
  {"java",    "java", &null_generator::generate},
  {"ruby",    "rb",   &null_generator::generate},
  {"python",  "py",   &null_generator::generate},
  {"perl",    "pl",   &null_generator::generate},
};

int main()
{
  string base("<<<BASENAME>>>");
  string lang("<<<LANG>>>");

  size_t lang_num=sizeof(infos)/sizeof(infos[0]);

  for (size_t i=0;i<lang_num;i++){
    if (infos[i].id==lang){
      string outname(base);
      string new_suf("."+string(infos[i].suffix));

      static const char *header_sufs[]={
        ".hpp",
        ".hxx",
        ".h",
        "",
      };

      for (size_t j=0;j<sizeof(header_sufs)/sizeof(header_sufs[0]);j++){
        string org_suf(header_sufs[j]);

        if (org_suf==""){
          //outname+=new_suf;
          break;
        }

        string::size_type p=outname.find(org_suf);
        if (p!=string::npos){
          outname.replace(p, org_suf.length(), ""/*new_suf*/);
          break;
        }
      }

      try{
        vector<reflect_base*> refs;
        <<<ADDREFLECTS>>>
        infos[i].f(refs, outname);
      }
      catch(exception& e){
        cerr<<"generate error: "<<e.what()<<endl;
        unlink(outname.c_str());
      }

      return 0;
    }
  }

  cerr<<"language "<<lang<<" is not supported"<<endl;
  return 0;
}
