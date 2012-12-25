#include <vector>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
using namespace std;

#include "../../src/lang/shared_ptr.h"
#include "../../src/system/file.h"
using namespace pfi::lang;
using namespace pfi::system::file;

#define TMPL_PATH "/usr/local/share/pficommon/"

bool replace(string &s, const string &pat, const string &aft)
{
  string::size_type pos=s.find(pat);
  if (pos==string::npos) return false;
  s.replace(pos, pat.length(), aft);
  return true;
}

string eval_cpp(const string &source_code, const string &compiler_options)
{
  string tname("genrpctmpXXXXXX");
  string gname("genrpcexeXXXXXX");
  {
    pfi::lang::shared_ptr<ostream> pofs(tmpstream(tname));
    ostream &ofs=*pofs.get();
    ofs<<source_code;
  }
  {
    pfi::lang::shared_ptr<ostream> pofs(tmpstream(gname));
  }
  int compile_stat=system(("g++ -xc++ "+tname+" "+compiler_options+" -lpficommon -o "+gname).c_str());
  unlink(tname.c_str());
  
  if (compile_stat!=0)
    throw runtime_error("compile failed");
  
  string ret;

  FILE *pf=popen(("./"+gname).c_str(), "r");

  for (int c; (c=fgetc(pf))!=EOF; ) ret+=(char)c;

  int gen_stat=pclose(pf);

  unlink(gname.c_str());
  if (gen_stat!=0)
    throw runtime_error("cannot generate file");

  return ret;
}

int main(int argc, char *argv[])
{
  if (argc<3){
    cout<<"usage: "<<argv[0]<<" --<language-name> <compiler-option> <rpc_signuture.h>"<<endl;
    return 0;
  }

  string signame(argv[1]);
  string langname("");
  for (int i=1; i<argc; i++){
    if (strncmp(argv[i], "--", 2)==0){
      langname=string(argv[i]+2);
    }
    else{
      signame=argv[i];
    }
  }

  if (langname==""){
    cerr<<"you must specify generate language name"<<endl;
    return 1;
  }

  string compiler_options;
  for (int i=2;i<argc-1;i++)
    compiler_options+=string(" ")+argv[i];

  string sign;
  {
    ostringstream oss;oss<<ifstream(signame.c_str()).rdbuf();
    sign=oss.str();
  }
  string tmpl;
  {
    ostringstream oss;oss<<ifstream(TMPL_PATH "genrpc_template.cpp").rdbuf();
    tmpl=oss.str();
  }
  string extr;
  {
    ostringstream oss;oss<<ifstream(TMPL_PATH "genrpc_extract_names.cpp").rdbuf();
    extr=oss.str();
  }

  vector<string> names;
  {
    replace(extr, "<<<SIGFILE>>>", signame);

    try{
      string names_str=eval_cpp(extr, compiler_options);
      istringstream iss(names_str);
      
      for (string name; iss>>name; )
        names.push_back(name);
    }
    catch(runtime_error& e){
      cout<<string("name extract failed: ")+e.what()<<endl;
      return 0;
    }
  }

  replace(tmpl, "<<<SIGFILE>>>", signame);
  replace(tmpl, "<<<BASENAME>>>", signame);
  replace(tmpl, "<<<LANG>>>", langname);

  ostringstream reflects;
  for (size_t i=0;i<names.size();i++){
    reflects<<names[i]<<"_reflect ref"<<i<<";"<<endl;
    reflects<<"refs.push_back(&ref"<<i<<");"<<endl;
  }
  replace(tmpl, "<<<ADDREFLECTS>>>", reflects.str());

  try{
    (void)eval_cpp(tmpl, compiler_options);
  }
  catch(runtime_error& e){
    cout<<"generate error: "<<e.what()<<endl;
    return 0;
  }

  return 0;
}
