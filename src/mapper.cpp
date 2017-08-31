#include "coreir.h"

#include "coreir-lib/cgralib.h"
#include "coreir-lib/common.h"

#include "passes/verifycanmap.h"
#include "passes/opsubstitution.h"
#include "passes/bitop2lut.h"

#include "definitions/linebuffermem.h"
#include "passes/techmapping.h"
#include "passes/verifytechmapping.h"
#include "passes/constduplication.h"


#include "coreir-passes/analysis/coreirjson.h"

#include <fstream>

using namespace CoreIR;


int main(int argc, char *argv[]){
  Context* c = newContext();
  
  CoreIRLoadLibrary_cgralib(c);
  CoreIRLoadLibrary_common(c);

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

  //SLight hack. Add a default width for all of coreir
  for (auto cgenmap : c->getNamespace("coreir")->getGenerators()) {
    if (cgenmap.second->getGenParams().count("width")) {
      cgenmap.second->addDefaultGenArgs({{"width",c->argInt(16)}});
    }
  }
  c->getGenerator("common.lutN")->addDefaultGenArgs({{"N",c->argInt(3)}});

  c->getPassManager()->setVerbosity(true);

  c->runPasses({"rungenerators","verifyfullyconnected-noclkrst","removebulkconnections","flattentypes"});
  
  //load last verification
  c->addPass(new MapperPasses::VerifyCanMap);
  c->runPasses({"verifycanmap"});

  //DO any normal optimizations

  //Pre-Technolog Mapping steps
  c->addPass(new MapperPasses::OpSubstitution);
  c->addPass(new MapperPasses::BitOp2Lut);
  c->runPasses({"opsubstitution","bitop2lut"});

  //Tech mapping
  //Link in LBMem def
  LoadDefinition_LinebufferMem(c);
  c->addPass(new MapperPasses::TechMapping);
  c->addPass(new MapperPasses::VerifyTechMapping);
  c->runPasses({"techmapping","verifytechmapping"});
  


  //Fold constants and registers into PEs
  c->addPass(new MapperPasses::ConstDuplication);
  c->runPasses({"constduplication"});

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
