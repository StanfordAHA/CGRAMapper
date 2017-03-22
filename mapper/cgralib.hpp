#ifndef CGRALIB_HPP_
#define CGRALIB_HPP_

#include "context.hpp"

using namespace CoreIR;

Namespace* getcgralib(Context* c) {
  
  Namespace* cgralib = c->newNamespace("cgra");
    
  Type* array16 = c->Array(16,c->BitOut());
  Type* PEType = c->Record({
    {"in0",c->Flip(array16)},
    {"in1",c->Flip(array16)},
    {"out",array16}
  });

  cgralib->newModuleDecl("PE_16",PEType,{{"op",ASTRING},{"constvalue",AINT}});
  cgralib->newModuleDecl("IOIn_16",c->Record({{"out",array16}}));
  cgralib->newModuleDecl("IOOut_16",c->Record({{"in0",c->Flip(array16)}}));
  return cgralib;
}

#endif //CGRALIB_HPP_
