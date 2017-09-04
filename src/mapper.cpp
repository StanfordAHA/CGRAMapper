#include "coreir.h"

#include "coreir-lib/cgralib.h"
#include "coreir-lib/commonlib.h"

#include "passes/verifycanmap.h"
#include "passes/opsubstitution.h"
#include "passes/bitop2lut.h"

#include "definitions/linebuffermem.h"
#include "passes/techmapping.h"
#include "passes/verifytechmapping.h"
//#include "passes/constregduplication.h"


#include "coreir-passes/analysis/coreirjson.h"

#include <fstream>

using namespace CoreIR;

typedef struct {
  vector<SelectPath> IO16;
  vector<SelectPath> IO16in;
  vector<SelectPath> IO1;
  vector<SelectPath> IO1in;
} IOpaths;

void getAllIOPaths(Wireable* w, IOpaths& paths) {
  Type* t = w->getType();
  if (auto at = dyn_cast<ArrayType>(t)) {
    if (at->getLen()==16 && isa<BitType>(at->getElemType())) {
      paths.IO16.push_back(w->getSelectPath());
    }
    else if (at->getLen() == 16 && isa<BitInType>(at->getElemType())) {
      paths.IO16in.push_back(w->getSelectPath());
    }
    else {
      for (auto sw : w->getSelects()) {
        getAllIOPaths(sw.second,paths);
      }
    }
  }
  else if (isa<BitType>(t)) {
    paths.IO1.push_back(w->getSelectPath());
  }
  else if (isa<BitInType>(t)) {
    paths.IO1in.push_back(w->getSelectPath());
  }
  else {
    for (auto sw : w->getSelects()) {
      getAllIOPaths(sw.second,paths);
    }
  }
  
}

void addIOs(Module* top) {
  Context* c = top->getContext();
  ModuleDef* mdef = top->getDef();

  Args aWidth({{"width",c->argInt(16)}});
  IOpaths iopaths;
  getAllIOPaths(mdef->getInterface(), iopaths);
  Instance* pt = addPassthrough(mdef->getInterface(),"_self");
  for (auto path : iopaths.IO16) {
    string ioname = "io16in"+c->getUnique();
    mdef->addInstance(ioname,"cgralib.IO",aWidth,{{"mode",c->argString("i")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"out"},path);
  }
  for (auto path : iopaths.IO16in) {
    string ioname = "io16"+c->getUnique();
    mdef->addInstance(ioname,"cgralib.IO",aWidth,{{"mode",c->argString("o")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"in"},path);
  }
  for (auto path : iopaths.IO1) {
    string ioname = "io1in"+c->getUnique();
    mdef->addInstance(ioname,"cgralib.bitIO",{{"mode",c->argString("i")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"out"},path);
  }
  for (auto path : iopaths.IO1in) {
    string ioname = "io1"+c->getUnique();
    mdef->addInstance(ioname,"cgralib.bitIO",{{"mode",c->argString("o")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"in"},path);
  }
  mdef->disconnect(mdef->getInterface());
  inlineInstance(pt);
}


int main(int argc, char *argv[]){
  Context* c = newContext();
  
  CoreIRLoadLibrary_cgralib(c);
  CoreIRLoadLibrary_commonlib(c);

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
  Module* top = nullptr;
  if (!loadFromFile(c,premap,&top)) {
    c->die();
  }
  ASSERT(top,"Could not load top:");

  //SLight hack. Add a default width for all of coreir
  for (auto cgenmap : c->getNamespace("coreir")->getGenerators()) {
    if (cgenmap.second->getGenParams().count("width")) {
      cgenmap.second->addDefaultGenArgs({{"width",c->argInt(16)}});
    }
  }
  c->getGenerator("commonlib.lutN")->addDefaultGenArgs({{"N",c->argInt(3)}});

  c->getPassManager()->setVerbosity(true);

  c->runPasses({"rungenerators","verifyfullyconnected-noclkrst","removebulkconnections","flattentypes"},{"global","commonlib"});
 

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
  addIOs(top);
  c->addPass(new MapperPasses::VerifyTechMapping);
  c->runPasses({"techmapping","verifytechmapping"});
  

  ////Fold constants and registers into PEs
  //c->addPass(new MapperPasses::ConstDuplication);
  //c->runPasses({"constduplication"});



  //Flatten
  c->runPasses({"flatten"});
  c->getPassManager()->printLog();
  cout << "Trying to save" << endl;
  c->runPasses({"coreirjson"});
  auto jpass = static_cast<Passes::CoreIRJson*>(c->getPassManager()->getAnalysisPass("coreirjson"));
  //Create file here.
  std::ofstream file(postmap);
  jpass->writeToStream(file,top->getRefName());
 
  
  //if (saveToFile(m->getNamespace(),postmap,m)) {
  //  c->die();
  //}

  deleteContext(c);
  return 0;
}
