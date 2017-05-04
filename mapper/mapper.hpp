#ifndef MAPPER_HPP_
#define MAPPER_HPP_

#include "coreir.h"
#include  <unordered_set>

using namespace std;

using namespace CoreIR;

SelectPath toIOPath(SelectPath sels) {
  if (!(sels[0]=="self")) return sels;
  
  string iname;
  if (sels[1] == "in") {
    sels[0] = "ioin";
    sels[1] = "out";
  }
  else if (sels[1] == "out") {
    sels[0] = "ioout";
    sels[1] = "in0";
  }
  else {
    cout << "Cannot map first select: " << sels[0] <<"."<<sels[1]<<endl;
    assert(false);
  }
  return sels;
}


Module* mapper(Context* c, Module* m, bool* err) {
  if (!m->hasDef()) {
    Error e;
    e.message("Module " + m->getName() + " has no definition to map!");
    e.fatal();
    c->error(e);
  } 

  //Create new module that has no ports
  Module* mapped = c->getGlobal()->newModuleDecl(m->getName() + "_mapped",c->Any());

  Generator* PE = c->getNamespace("cgra")->getGenerator("PE");
  Generator* IO = c->getNamespace("cgra")->getGenerator("IO");
  Generator* Reg = c->getNamespace("cgra")->getGenerator("Reg");
  Generator* Const = c->getNamespace("cgra")->getGenerator("Const");
  Generator* Mem = c->getNamespace("cgra")->getGenerator("Mem");

  ModuleDef* mappedDef = mapped->newModuleDef();
  mappedDef->addInstance("ioin",IOin);
  mappedDef->addInstance("ioout",IOout);

  ModuleDef* mdef = m->getDef();
  for (auto instmap : mdef->getInstances()) {
    Instance* inst = instmap.second;
    string node = inst->getModuleRef()->getName();
    if (node=="add2_16") {
      Args configargs = Args({{"op",c->argString("add")},{"constvalue",c->argInt(0)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,configargs);
    }
    else if (node=="mult2_16") {
      Args configargs = Args({{"op",c->argString("mult")},{"constvalue",c->argInt(0)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,configargs);
    }
    else if (node=="const_16") {
      Arg* constarg = inst->getConfigArg("value");
      Args configargs = Args({{"op",c->argString("const")},{"constvalue",constarg}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,configargs);
    }
    else { c->die(); }
  }

  for (auto con : mdef->getConnections() ) {
    SelectPath pathA = toIOPath(con.first->getSelectPath());
    SelectPath pathB = toIOPath(con.second->getSelectPath());
    mappedDef->wire(pathA,pathB);
  }
  mapped->setDef(mappedDef);
  return mapped;
}




#endif //MAPPER_HPP_
