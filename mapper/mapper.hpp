#ifndef MAPPER_HPP_
#define MAPPER_HPP_

#include "coreir.h"
#include  <unordered_set>

using namespace std;

using namespace CoreIR;

WirePath toIOPath(WirePath wp) {
  if (!(wp.first=="self")) return wp;
  
  vector<string> sels = wp.second;
  string iname;
  if (sels[0] == "in") {
    iname = "ioin";
    sels[0] = "out";
  }
  else if (sels[0] == "out") {
    iname = "ioout";
    sels[0] = "in0";
  }
  else {
    cout << "Cannot map first select: " << wp.first<<"."<<sels[0]<<endl;
    assert(false);
  }
  return {iname,sels};
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

  Module* pe = c->getNamespace("cgra")->getModule("PE_16");
  Module* IOin = c->getNamespace("cgra")->getModule("IOIn_16");
  Module* IOout = c->getNamespace("cgra")->getModule("IOOut_16");

  ModuleDef* mappedDef = mapped->newModuleDef();
  mappedDef->addInstance("ioin",IOin);
  mappedDef->addInstance("ioout",IOout);

  ModuleDef* mdef = m->getDef();
  for (auto instmap : mdef->getInstances()) {
    Instance* inst = instmap.second;
    string node = inst->getInstRef()->getName();
    if (node=="add2_16") {
      Args config = Args({{"op",c->str2Arg("add")},{"constvalue",c->int2Arg(0)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,config);
    }
    else if (node=="mult2_16") {
      Args config = Args({{"op",c->str2Arg("mult")},{"constvalue",c->int2Arg(0)}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,config);
    }
    else if (node=="const_16") {
      Arg* constarg = inst->getConfigValue("value");
      Args config = Args({{"op",c->str2Arg("const")},{"constvalue",constarg}});
      Instance* i = mappedDef->addInstance(inst);
      i->replace(pe,config);
    }
    else { c->die(); }
  }

  for (auto con : mdef->getConnections() ) {
    WirePath pathA = toIOPath(con.first->getPath());
    WirePath pathB = toIOPath(con.second->getPath());
    mappedDef->wire(pathA,pathB);
  }
  mapped->addDef(mappedDef);
  return mapped;
}




#endif //MAPPER_HPP_
