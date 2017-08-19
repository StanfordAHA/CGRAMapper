#ifndef MAPPER_HPP_
#define MAPPER_HPP_

#include "coreir.h"
#include "coreir-passes/transform/matchandreplace.h"

using namespace std;

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

//This will edit the module
void mapper(Context* c, Module* m, bool* err) {
  if (!m->hasDef()) {
    Error e;
    e.message("Module " + m->getName() + " has no definition to map!");
    e.fatal();
    c->error(e);
  } 

  Namespace* coreir = c->getNamespace("coreir");


  Generator* PE = c->getNamespace("cgralib")->getGenerator("PE");
  Generator* IO = c->getNamespace("cgralib")->getGenerator("IO");
  Generator* Reg = c->getNamespace("cgralib")->getGenerator("Reg");
  Generator* Const = c->getNamespace("cgralib")->getGenerator("Const");
  //Generator* Mem = c->getNamespace("cgralib")->getGenerator("Mem");
  
  //PE replacement module:
  Args PEGenArgs = {{"width",c->argInt(16)},{"numin",c->argInt(2)}};
  Type* PEType = PE->getTypeGen()->getType(PEGenArgs);
  
  Args ConstGenArgs = {{"width",c->argInt(16)}};
  Type* ConstType = Const->getTypeGen()->getType(ConstGenArgs);
  
  Args RegGenArgs = {{"width",c->argInt(16)}};
  Type* RegType = Reg->getTypeGen()->getType(RegGenArgs);

  //Create all the search and replace patterns. 
  Namespace* patns = c->newNamespace("mapperpatterns");
  
   unordered_map<string,vector<string>> opmap({
    {"unary",{"not","neg"}},
    {"unaryReduce",{"andr","orr","xorr"}},
    {"binary",{
      "and","or","xor",
      "dshl","dlshr","dashr",
      "add","sub","mul",
      "udiv","urem",
      "sdiv","srem","smod"
    }},
    {"binaryReduce",{"eq",
      "slt","sgt","sle","sge",
      "ult","ugt","ule","uge"
    }},
    {"ternary",{"mux"}},
  });
 
  for (auto op : opmap["unary"]) {
    Module* patternOp = patns->newModuleDecl(op,PEType);
    ModuleDef* pdef = patternOp->newModuleDef();
      pdef->addInstance("inst",coreir->getGenerator(op),{{"width",c->argInt(16)}});
      pdef->connect("self.data.in.0","inst.in");
      pdef->connect("self.data.out","inst.out");
    patternOp->setDef(pdef);
  }
  for (auto op : opmap["unaryReduce"]) {
    Module* patternOp = patns->newModuleDecl(op,PEType);
    ModuleDef* pdef = patternOp->newModuleDef();
      pdef->addInstance("inst",coreir->getGenerator(op),{{"width",c->argInt(16)}});
      pdef->connect("self.data.in.0","inst.in");
      pdef->connect("self.bit.out","inst.out");
    patternOp->setDef(pdef);
  }
  for (auto op : opmap["binary"]) {
    Module* patternOp = patns->newModuleDecl(op,PEType);
    ModuleDef* pdef = patternOp->newModuleDef();
      pdef->addInstance("inst",coreir->getGenerator(op),{{"width",c->argInt(16)}});
      pdef->connect("self.data.in.0","inst.in0");
      pdef->connect("self.data.in.1","inst.in1");
      pdef->connect("self.data.out","inst.out");
    patternOp->setDef(pdef);
  }
  for (auto op : opmap["binaryReduce"]) {
    Module* patternOp = patns->newModuleDecl(op,PEType);
    ModuleDef* pdef = patternOp->newModuleDef();
      pdef->addInstance("inst",coreir->getGenerator(op),{{"width",c->argInt(16)}});
      pdef->connect("self.data.in.0","inst.in0");
      pdef->connect("self.data.in.1","inst.in1");
      pdef->connect("self.bit.out","inst.out");
    patternOp->setDef(pdef);
  }
  for (auto op : opmap["ternary"]) {
    Module* patternOp = patns->newModuleDecl(op,PEType);
    ModuleDef* pdef = patternOp->newModuleDef();
      pdef->addInstance("inst",coreir->getGenerator(op),{{"width",c->argInt(16)}});
      pdef->connect("self.data.in.0","inst.in0");
      pdef->connect("self.data.in.1","inst.in1");
      pdef->connect("self.bit.in.0","inst.sel");
      pdef->connect("self.data.out","inst.out");
    patternOp->setDef(pdef);
  }
  
  cout << "Added PE Passes!!" << endl;
  //Search pattern for Const TODO probably could do this usng a simpler method
  Module* patternConst = patns->newModuleDecl("const",ConstType);
  ModuleDef* pdef = patternConst->newModuleDef();
    pdef->addInstance("inst",coreir->getGenerator("const"),{{"width",c->argInt(16)}},{{"value",c->argInt(13)}});
    pdef->connect("self","inst"); //These are the same
  patternConst->setDef(pdef);
  cout << "Added Cons Passes!!" << endl;

  //Search pattern for Reg TODO probably could do this usng a simpler method
  Module* patternReg = patns->newModuleDecl("reg",RegType);
  pdef = patternReg->newModuleDef();
    Args regArgs({
      {"width",c->argInt(16)},
      {"en",c->argBool(false)},
      {"clr",c->argBool(false)},
      {"rst",c->argBool(false)}
    });
    pdef->addInstance("inst",coreir->getGenerator("reg"),regArgs,{{"init",c->argInt(0)}});
    pdef->connect("self.in","inst.in"); 
    pdef->connect("self.out","inst.out"); 
  patternReg->setDef(pdef);
  cout << "Added Reg Passes!!" << endl;

  Args aWidth({{"width",c->argInt(16)}});
  ModuleDef* mdef = m->getDef();
  IOpaths iopaths;
  getAllIOPaths(mdef->getInterface(), iopaths);
  Instance* pt = addPassthrough(mdef->getInterface(),"_self");
  for (auto path : iopaths.IO16) {
    string ioname = "io16in"+c->getUnique();
    mdef->addInstance(ioname,IO,aWidth,{{"mode",c->argString("i")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"out"},path);
  }
  for (auto path : iopaths.IO16in) {
    string ioname = "io16"+c->getUnique();
    mdef->addInstance(ioname,IO,aWidth,{{"mode",c->argString("o")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"in"},path);
  }
  for (auto path : iopaths.IO1) {
    string ioname = "io1in"+c->getUnique();
    mdef->addInstance(ioname,IO,{{"width",c->argInt(1)}},{{"mode",c->argString("i")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"out"},path);
  }
  for (auto path : iopaths.IO1in) {
    string ioname = "io1"+c->getUnique();
    mdef->addInstance(ioname,IO,{{"width",c->argInt(1)}},{{"mode",c->argString("o")}});
    path[0] = "in";
    path.insert(path.begin(),"_self");
    mdef->connect({ioname,"in"},path);
  }
  mdef->disconnect(mdef->getInterface());
  inlineInstance(pt);
  
  vector<string> toRun;
  for (auto tmap : opmap) {
    for (auto op : tmap.second) {
      Passes::MatchAndReplace::Opts opts;
      opts.configargs = {{"op",c->argString(op)}};
      opts.genargs = PEGenArgs;
      c->addPass(new Passes::MatchAndReplace(op,patns->getModule(op),PE,opts));
      toRun.push_back(op);
    }
  }
  
  Passes::MatchAndReplace::Opts copts;
  copts.genargs = ConstGenArgs;
  copts.getConfigArgs = [](const vector<Instance*>& matches) {
    return matches[0]->getConfigArgs();
  };
  c->addPass(new Passes::MatchAndReplace("const",patternConst,Const,copts));

  Passes::MatchAndReplace::Opts ropts;
  ropts.genargs = RegGenArgs;
  c->addPass(new Passes::MatchAndReplace("reg",patternReg,Reg,ropts));
  cout << "Added all passes!!" << endl;
  toRun.push_back("const");
  toRun.push_back("reg");
  c->runPasses(toRun,{"_G","mapperpatterns"});
}




#endif //MAPPER_HPP_
