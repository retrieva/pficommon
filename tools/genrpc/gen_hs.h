#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <sstream>
#include <map>
using namespace std;

#include <pficommon/lang/function.h>
#include <pficommon/data/serialization.h>
#include <pficommon/network/rpc.h>
#include <pficommon/network/rpc/reflect.h>
using namespace pfi::lang;
using namespace pfi::data::serialization;
using namespace pfi::network;
using namespace pfi::network::rpc;

class haskell_generator{
public:
  haskell_generator(reflect_base &ref)
    : ref(ref)
    , anon(0){
  }

  static void generate(vector<reflect_base*> &ref, const string &os){
    haskell_generator gen(*ref[0]);
    gen.generate(os);
  }

private:
  void generate(const string &ofn){
    string module_name=ofn;
    if (module_name.length()>0) module_name[0]=toupper(module_name[0]);

    string fn=module_name+".hs";
    fn[0]=toupper(fn[0]);

    ofstream os(fn.c_str());

    map<string, type_rep*> types;

    map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > > &mems=ref.members();

    // collecting classes
    for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
         p!=mems.end(); p++){
      p->second.first->traverse(bind(&haskell_generator::add_type, this, &types, _1));
      for (int i=0; i<(int)p->second.second.size(); i++)
        p->second.second[i]->traverse(bind(&haskell_generator::add_type, this, &types, _1));
    }

    // output header
    os<<"module "<<module_name<<"("<<endl;

    for (map<string, type_rep*>::iterator p=types.begin();
         p!=types.end();p++){
      class_type *cls=dynamic_cast<class_type*>(p->second);
      if (cls==NULL) continue;
      string class_name=cls->get_name();
      if (class_name.length()>0) class_name[0]=toupper(class_name[0]);
      os<<"  "<<class_name<<"(..),"<<endl;
    }

    for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
         p!=mems.end(); p++){
      const string &method_name=p->first;
      os<<"  call_"<<method_name<<","<<endl;
    }

    os<<endl;
    os<<"  module PFIRPC,"<<endl;
    os<<") where"<<endl;
    os<<endl;

    os<<"import PFIRPC"<<endl;
    os<<"import qualified Data.Map as Map"<<endl;
    os<<"import Data.Map (Map)"<<endl;
    os<<endl;

    // defining classes
    for (map<string, type_rep*>::iterator p=types.begin();
         p!=types.end();p++){
      class_type *cls=dynamic_cast<class_type*>(p->second);
      if (cls==NULL) continue;

      string class_name=cls->get_name();
      if (class_name.length()>0) class_name[0]=toupper(class_name[0]);
      string mem_prefix=class_name;
      if (mem_prefix.length()>0) mem_prefix[0]=tolower(mem_prefix[0]);
      mem_prefix+="_";

      os<<"data "<<class_name<<" = "<<class_name<<endl;
      bool first_mem=true;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++, first_mem=false){
        os<<"  "<<(first_mem?"{":",")<<" "<<mem_prefix<<q->first<<" :: ";
        print_typename(os, q->second.get());
        os<<endl;
      }
      os<<"  } deriving (Eq, Show)"<<endl;
      os<<endl;

      os<<"instance RPCType "<<class_name<<" where"<<endl;
      os<<"  sendValue c v = do"<<endl;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++){
        os<<"    sendValue c ("<<mem_prefix<<q->first<<" v)"<<endl;
      }
      os<<"  recvValue c = do"<<endl;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++){
        os<<"    a_"<<q->first<<" <- recvValue c"<<endl;
      }
      os<<"    return $ "<<class_name;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++){
        os<<" a_"<<q->first;
      }
      os<<endl;
      os<<endl;
    }

    // generate rpc client

    for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
         p!=mems.end(); p++){
      const string &method_name=p->first;

      os<<"call_"<<method_name<<" :: RPCClient c => c";
      for (int i=0; i<(int)p->second.second.size(); i++){
        os<<" -> ";
        type_rep *arep=p->second.second[i].get();
        print_typename(os, arep);
      }
      os<<" -> IO (";
      print_typename(os, p->second.first.get());
      os<<")"<<endl;

      os<<"call_"<<method_name<<" c";
      for (int i=0; i<(int)p->second.second.size(); i++)
        os<<" a"<<i;
      os<<" ="<<endl;

      os<<"  call c \""<<method_name<<"\" 1 [";
      for (int i=0; i<(int)p->second.second.size(); i++){
        if (i!=0) os<<",";
        os<<" RPCValue a"<<i;
      }
      os<<" ]"<<endl;
      os<<endl;
    }
  }

  void add_type(map<string, type_rep*> *types, type_rep* type){
    if (class_type *cls=dynamic_cast<class_type*>(type)){
      string class_name=cls->get_name();
      if (class_name==""){
        ostringstream oss;
        oss<<"anonymous"<<(anon++);
        class_name=oss.str();
      }
      cls->set_name(class_name);
      (*types)[class_name]=cls;
    }
  }

  void print_typename(ostream &os, type_rep *rep){
    if (dynamic_cast<bool_type*>(rep))
      os<<"Bool";

    if (int_type *nt=dynamic_cast<int_type*>(rep))
      os<<"Int";

    if (dynamic_cast<float_type*>(rep))
      os<<"Double";
    
    if (dynamic_cast<char_type*>(rep))
      os<<"Char";

    if (dynamic_cast<string_type*>(rep))
      os<<"String";

    if (array_type *at=dynamic_cast<array_type*>(rep)){
      os<<"[";
      print_typename(os, at->elem_type().get());
      os<<"]";
    }

    if (map_type *mt=dynamic_cast<map_type*>(rep)){
      os<<"(Map ";
      print_typename(os, mt->key_type().get());
      os<<" ";
      print_typename(os, mt->value_type().get());
      os<<")";
    }
    
    if (class_type *ct=dynamic_cast<class_type*>(rep)){
      string name=ct->get_name();
      if (name.length()>0) name[0]=toupper(name[0]);
      os<<name;
    }

    runtime_error("unsupported type");
  }

  reflect_base &ref;
  int anon;
};
