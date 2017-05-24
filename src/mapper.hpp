#ifndef MAPPER_HPP_
#define MAPPER_HPP_

#include "coreir.h"
#include "coreir-lib/stdlib.h"
#include  <unordered_set>

using namespace std;

using namespace CoreIR;

SelectPath toIOPath(SelectPath sels,unordered_set<string> consts) {
  cout << "mapping path: " << SelectPath2Str(sels) << endl;
  if (sels[0]=="self") {
    string iname;
    if (sels[1] == "in") {
      sels[0] = "ioin";
      sels[1] = "out";
    }
    else if (sels[1] == "out") {
      sels[0] = "ioout";
      sels[1] = "in";
    }
    else {
      cout << "Cannot map first select: " << sels[0] <<"."<<sels[1]<<endl;
      assert(false);
    }
  }
  else if (consts.count(sels[0])>0) { 
    ;
  }
  else {
    sels.insert(sels.begin()+1,"data");
  }
  cout << "mapped path: " << SelectPath2Str(sels) << endl;
  return sels;
}

//Add
//in.0
//out

//PE:
//data.in.0
//data.out




Module* mapper(Context* c, Module* m, bool* err) {
  if (!m->hasDef()) {
    Error e;
    e.message("Module " + m->getName() + " has no definition to map!");
    e.fatal();
    c->error(e);
  } 

  Namespace* stdlib = c->getNamespace("stdlib");

  //Create new module that has no ports
  Module* mapped = c->getGlobal()->newModuleDecl(m->getName() + "_mapped",c->Any());

  Generator* PE = c->getNamespace("cgralib")->getGenerator("PE");
  Generator* IO = c->getNamespace("cgralib")->getGenerator("IO");
  //Generator* Reg = c->getNamespace("cgralib")->getGenerator("Reg");
  Generator* Const = c->getNamespace("cgralib")->getGenerator("Const");
  //Generator* Mem = c->getNamespace("cgralib")->getGenerator("Mem");

  Args aWidth({{"width",c->argInt(16)}});
  ModuleDef* mappedDef = mapped->newModuleDef();
  
  mappedDef->addInstance("ioin",IO,aWidth,{{"mode",c->argString("i")}});
  mappedDef->addInstance("ioout",IO,aWidth,{{"mode",c->argString("o")}});

  ModuleDef* mdef = m->getDef();
  
  //Set to hold const inst names for now
  unordered_set<string> consts;
  
  for (auto instmap : mdef->getInstances()) {
    Instance* inst = instmap.second;
    Generator* node = inst->getGeneratorRef();
    if (node == stdlib->getGenerator("add")) {
      Args configargs = Args({{"op",c->argString("add")}});
      Args genargs = Args({{"width",c->argInt(16)},{"numin",c->argInt(2)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(PE,genargs,configargs);
      cout << i->toString() << endl;
      cout << i->getModuleRef()->getName() << endl;
    }
    else if (node == stdlib->getGenerator("mul")) {
      Args configargs = Args({{"op",c->argString("mul")}});
      Args genargs = Args({{"width",c->argInt(16)},{"numin",c->argInt(2)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(PE,genargs,configargs);
    }
    else if (node == stdlib->getGenerator("const")) {
      Args configargs = inst->getConfigArgs();
      Args genargs = Args({{"width",c->argInt(16)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(Const,genargs,configargs);
      consts.insert(instmap.first)
    }
    else { 
      cout << "NYI for " << node->getNamespace()->getName() << "." << node->getName();
      c->die(); 
    }
  }

  for (auto con : mdef->getConnections() ) {
    SelectPath pathA = toIOPath(con.first->getSelectPath(),consts);
    SelectPath pathB = toIOPath(con.second->getSelectPath(),consts);
    cout << "connecting: " << SelectPath2Str(pathA) << " to " << SelectPath2Str(pathB) << endl;
    mappedDef->connect(pathA,pathB);
  }
  mapped->setDef(mappedDef);
  return mapped;
}




#endif //MAPPER_HPP_
