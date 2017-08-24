#include "coreir.h"
#include "mapper.hpp"
#include "coreir-lib/cgralib.h"
#include "coreir-passes/analysis/coreirjson.h"
#include <fstream>

using namespace CoreIR;


int main(int argc, char *argv[]){
  Context* c = newContext();
  
  CoreIRLoadLibrary_cgralib(c);

  string premap;
  string postmap;
  if (argc == 3) {
    premap = argv[1];
    postmap = argv[2];
  }
  else {
    cout << "usage: mapper premapped.json mapped.json" << endl;
    return 1;
  }

  cout << "Loading " << premap << endl;
  Module* m = nullptr;
  if (!loadFromFile(c,premap,&m)) {
    c->die();
  }
  ASSERT(m,"Could not load top:");


  bool err = false;
  cout << "Trying to map" << endl;
  mapper(c,m,&err);
  if(err){
    cout << "failed mapping!" << endl;
    c->die();
  }
  c->getPassManager()->printLog();
  cout << "Trying to save" << endl;
  c->runPasses({"coreirjson"});
  auto jpass = static_cast<Passes::CoreIRJson*>(c->getPassManager()->getAnalysisPass("coreirjson"));
  //Create file here.
  std::ofstream file(postmap);
  jpass->writeToStream(file,m->getNamespace()->getName() + "." +m->getName());
 
  
  //if (saveToFile(m->getNamespace(),postmap,m)) {
  //  c->die();
  //}

  deleteContext(c);
  return 0;
}
