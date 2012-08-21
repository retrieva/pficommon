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

#define LIB_PATH "pfi_rpc.php"

typedef shared_ptr<type_rep> type_rep_p;
typedef pair<type_rep_p, vector<type_rep_p> > function_type;

class php_generator{
public:
  php_generator(vector<reflect_base*> &refs)
    : refs(refs)
    , anon(0){
  }

  static void generate(vector<reflect_base*> &refs, const string &os){
    php_generator gen(refs);
    gen.generate(os);
  }

private:
  void generate(const string &ofn){
    ofstream os((ofn+".php").c_str());

    map<string, type_rep*> types;

    // collecting classes

    for (size_t i=0; i<refs.size();i++){
      map<string, function_type> &mems=refs[i]->members();

      for (map<string, function_type>::iterator p=mems.begin();
           p!=mems.end(); p++){
        p->second.first->traverse(bind(&php_generator::add_type, this, &types, _1));
        for (int i=0; i<(int)p->second.second.size(); i++)
          p->second.second[i]->traverse(bind(&php_generator::add_type, this, &types, _1));
      }
    }

    // output header
    os<<"<?"<<endl;
    os<<"include_once(\"" LIB_PATH "\");"<<endl;

    int anon_mem_cnt=0;

    // defining classes
    for (map<string, type_rep*>::iterator p=types.begin();
         p!=types.end();p++){
      class_type *cls=dynamic_cast<class_type*>(p->second);
      if (cls==NULL) continue;

      os<<"class "<<cls->get_name()<<"{"<<endl;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++){
        if (q->first==""){
          ostringstream oss;
          oss<<"anon"<<(anon_mem_cnt++);
          q->first=oss.str();
        }
        os<<"  var $"<<q->first<<";"<<endl;
      }
      os<<endl;
      os<<"  public function typeinfo(){"<<endl;
      os<<"    return new class_type(\""<<cls->get_name()<<"\","<<endl;
      os<<"      array("<<endl;
      for (class_type::iterator q=cls->begin();
           q!=cls->end(); q++){
        os<<"        \""<<q->first<<"\" => ";
        print_typeinfo(os, q->second.get());
        os<<","<<endl;
      }
      os<<"      ));"<<endl;
      os<<"  }"<<endl;
      os<<"}"<<endl;
    }

    // generate rpc client
    for (size_t i=0; i<refs.size(); i++){
      reflect_base &ref=*refs[i];
      map<string, function_type> &mems=refs[i]->members();
      
      os<<"class "<<ref.get_name()<<"_client{"<<endl;
      
      os<<"  var $int_typeinfo;"<<endl;
      os<<"  var $string_typeinfo;"<<endl;
      
      for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
           p!=mems.end(); p++){
        const string &method_name=p->first;
        
        os<<"  var $"<<method_name<<"_ret_typeinfo;"<<endl;
        for (int i=0; i<(int)p->second.second.size(); i++){
          os<<"  var $"<<method_name<<"_a"<<i<<"_typeinfo;"<<endl;
        }
      }
      
      os<<"  function __construct($host, $port){"<<endl;
      os<<"    $this->int_typeinfo=new int_type(false, 4);"<<endl;
      os<<"    $this->string_typeinfo=new string_type();"<<endl;
      
      for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
           p!=mems.end(); p++){
        const string &method_name=p->first;
        
        os<<"    $this->"<<method_name<<"_ret_typeinfo=";
        print_typeinfo(os, p->second.first.get());
        os<<";"<<endl;
        for (int i=0; i<(int)p->second.second.size(); i++){
          type_rep *arep=p->second.second[i].get();
          os<<"    $this->"<<method_name<<"_a"<<i<<"_typeinfo=";
          print_typeinfo(os, arep);
          os<<";"<<endl;
        }
      }
      
      os<<"    $sock=fsockopen($host, $port);"<<endl;
      os<<"    if ($sock===FALSE)"<<endl;
      os<<"      throw new Exception(\"cannot connect to host\");"<<endl;
      os<<"    $this->buf=new buffer($sock);"<<endl;
      os<<"  }"<<endl;
      
      os<<"  function ping_pong(){"<<endl;
      os<<"    $this->string_typeinfo->write($this->buf, \"<<<PING>>>\");"<<endl;
      os<<"    $this->buf->flush();"<<endl;
      os<<"    $pong=$this->string_typeinfo->read($this->buf);"<<endl;
      os<<"    if ($pong!=\"<<<PONG>>>\")"<<endl;
      os<<"      throw new Exception(\"connection establish failed\");"<<endl;
      os<<"  }"<<endl;
      
      os<<"  function check_code(){"<<endl;
      os<<"    $code=$this->string_typeinfo->read($this->buf);"<<endl;
      os<<"    if ($code!=\"OK\")"<<endl;
      os<<"      throw new Exception(\"result code is not OK: $code\");"<<endl;
      os<<"  }"<<endl;
      
      for (map<string, pair<shared_ptr<type_rep>, vector<shared_ptr<type_rep> > > >::iterator p=mems.begin();
           p!=mems.end(); p++){
        const string &method_name=p->first;
        
        os<<"  function call_"<<method_name<<"(";
        for (int i=0; i<(int)p->second.second.size(); i++){
          if (i>0) os<<", ";
          os<<"$a"<<i;
        }
        os<<"){"<<endl;
        
        os<<"    $this->ping_pong();"<<endl;
        os<<"    $this->string_typeinfo->write($this->buf, \""<<method_name<<"\");"<<endl;
        os<<"    $this->int_typeinfo->write($this->buf, 1);"<<endl;
        for (int i=0; i<(int)p->second.second.size(); i++){
          os<<"    $this->"<<method_name<<"_a"<<i<<"_typeinfo->write($this->buf, $a"<<i<<");"<<endl;
        }
        os<<"    $this->buf->flush();"<<endl;
        os<<"    $this->check_code();"<<endl;
        
        os<<"    return $this->"<<method_name<<"_ret_typeinfo->read($this->buf);"<<endl;
        
        os<<"  }"<<endl;
      }
      os<<"}"<<endl;
    }

    // output footer
    os<<"?>"<<endl;
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

  void print_typeinfo(ostream &os, type_rep *rep){
    if (dynamic_cast<bool_type*>(rep))
      os<<"new bool_type()";

    if (int_type *nt=dynamic_cast<int_type*>(rep))
      os<<"new int_type("<<(nt->sign()?"true":"false")
        <<","<<nt->size()<<")";
    
    if (dynamic_cast<float_type*>(rep))
      os<<"new float_type()";
    
    if (dynamic_cast<char_type*>(rep))
      os<<"new char_type()";

    if (dynamic_cast<string_type*>(rep))
      os<<"new string_type()";

    if (array_type *at=dynamic_cast<array_type*>(rep)){
      os<<"new array_type(";
      print_typeinfo(os, at->elem_type().get());
      os<<")";
    }

    if (map_type *mt=dynamic_cast<map_type*>(rep)){
      os<<"new map_type(";
      print_typeinfo(os, mt->key_type().get());
      os<<",";
      print_typeinfo(os, mt->value_type().get());
      os<<")";
    }
    
    if (class_type *ct=dynamic_cast<class_type*>(rep))
      os<<ct->get_name()<<"::typeinfo()";
  }

  vector<reflect_base*> &refs;
  int anon;
};
