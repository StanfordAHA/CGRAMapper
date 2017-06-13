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

  bool debug = false;
  string premap;
  string postmap;
  if (argc >= 3 && argc <=4) {
    premap = argv[1];
    postmap = argv[2];
    if (argc==4) debug = true;
  }
  else {
    cout << "usage: mapper premapped.json mapped.json <debug>" << endl;
    return 1;
  }

  bool err = false;
  cout << "Loading " << premap << endl;
  Module* m = loadModule(c,premap,&err);
  if(err){c->die();}
  
  cout << "Trying to map" << endl;
  mapper(c,m,&err);
  if(err){
    cout << "failed mapping!" << endl;
    c->die();
  }

  cout << "Trying to save" << endl;
  saveModule(m,postmap,&err);
  if (err) c->die();

  deleteContext(c);

  if (debug) {
    cout << "Trying to Load" << endl;
    c = newContext();
    CoreIRLoadLibrary_stdlib(c);
    CoreIRLoadLibrary_cgralib(c);
    m = loadModule(c,argv[2],&err);
    if (err) c->die();
    m->print();
    deleteContext(c);
  }
  return 0;
}
