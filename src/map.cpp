#include "coreir.h"
#include "coreir-pass/passes.h"

#include "mapper.hpp"
#include "coreir-lib/cgralib.h"
#include "coreir-lib/stdlib.h"

using namespace CoreIR;


int main(int argc, char *argv[]){
  Context* c = newContext();
  
  CoreIRLoadLibrary_stdlib(c);
  CoreIRLoadLibrary_cgralib(c);

  if(argc!=3){
    cout << "usage: mapper premapped.json mapped.json" << endl;
    return 1;
  }

  bool err = false;
  cout << "Loading " << argv[1] << endl;
  Module* m = loadModule(c,argv[1],&err);
  if(err){c->die();}
  
  cout << "Trying to map" << endl;
  mapper(c,m,&err);
  if(err){
    cout << "failed mapping!" << endl;
    c->die();
  }

  cout << "Trying to save" << endl;
  saveModule(m,argv[2],&err);
  if (err) c->die();

  deleteContext(c);

  cout << "Trying to Load" << endl;
  c = newContext();
  CoreIRLoadLibrary_stdlib(c);
  CoreIRLoadLibrary_cgralib(c);
  m = loadModule(c,argv[2],&err);
  if (err) c->die();
  return 0;
}
