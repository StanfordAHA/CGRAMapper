#include "coreir.h"

#include "coreir-lib/cgralib.h"

#include "passes/verifycanmap.h"
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

  
  c->runPasses({"rungenerators","verifyfullyconnected-noclkrst","removebulkconnections","flattentypes"});
  
  //load last verification
  c->addPass(new MapperPasses::VerifyCanMap);
  c->runPasses({"verifycanmap"});

  //DO any normal optimizations

  //Pre-Technolog Mapping steps

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
