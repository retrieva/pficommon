#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>
using namespace std;

#include <pficommon/network/rpc/reflect.h>
using namespace pfi::network::rpc;

class null_generator{
public:
  static void generate(vector<reflect_base*> &ref, const string &os){
    throw runtime_error("this language is not implemented currently");
  }
};
