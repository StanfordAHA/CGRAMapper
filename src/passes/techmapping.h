
using namespace CoreIR;

namespace MapperPasses {
class TechMapping : public InstanceVisitorPass {
  public :
    static std::string ID;
    TechMapping() : InstanceVisitorPass(ID,"Does substituions like LT -> GTE + Not") {}
    void setVisitorInfo() override;
};

}

namespace {
//Replaces all LUTs
bool lutReplacement(Instance* inst) {
  Context* c = inst->getContext();
  ModuleDef* def = inst->getContainer();
  string iname = inst->getInstname();
  Args bitPEArgs({{"LUT_init",inst->getConfigArgs()["init"]}});
  Instance* bitPE = def->addInstance(iname+"_bitPE","cgralib.BitPE",Args(),bitPEArgs);
  
  //Isolate the instance
  Instance* pt = addPassthrough(inst,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),inst);
  //Some of these connections might be lost while inlining passthrough
  //But this is what we want because some inputs are unconnected
  def->connect(pt->sel({"in","in","0"}),bitPE->sel({"bit","in","0"}));
  def->connect(pt->sel({"in","in","1"}),bitPE->sel({"bit","in","1"}));
  def->connect(pt->sel({"in","in","2"}),bitPE->sel({"bit","in","2"}));
  def->connect(pt->sel({"in","out"}),bitPE->sel({"bit","out"}));
  
  //Remove instance and inline passthrough
  def->removeInstance(inst);
  inlineInstance(pt);
  return true;
}

//Replaces all Data (Add,Sub,RSHIFT,LSHIFT,Mult,OR,AND,XOR)
bool binaryOpReplacement(Instance* inst) {
  Context* c = inst->getContext();
  ModuleDef* def = inst->getContainer();
  string iname = inst->getInstname();
  //For now just use the coreir lib name as the op
  string opstr = inst->getInstantiableRef()->getName();
  Args dataPEArgs({{"op",c->argString(opstr)}});
  Instance* dataPE = def->addInstance(iname+"_PE","cgralib.DataPE",Args(),dataPEArgs);
  
  //Isolate the instance
  Instance* pt = addPassthrough(inst,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),inst);
  //Some of these connections might be lost while inlining passthrough
  //But this is what we want because some inputs are unconnected
  def->connect(pt->sel({"in","in0"}),dataPE->sel({"data","in","0"}));
  def->connect(pt->sel({"in","in1"}),dataPE->sel({"data","in","1"}));
  def->connect(pt->sel({"in","out"}),dataPE->sel({"data","out"}));
  
  //Remove instance and inline passthrough
  def->removeInstance(inst);
  inlineInstance(pt);
  return true;
}

//Replaces  (GTE,LTE) with PE
bool compOpReplacement(Instance* inst) {
  Context* c = inst->getContext();
  ModuleDef* def = inst->getContainer();
  string iname = inst->getInstname();
  //For now just use the coreir lib name as the op
  string opstr = inst->getInstantiableRef()->getName();
  Args PEArgs({{"op",c->argString(opstr)}});
  Instance* PE = def->addInstance(iname+"_PE","cgralib.PE",Args(),PEArgs);
  
  //Isolate the instance
  Instance* pt = addPassthrough(inst,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),inst);
  //Some of these connections might be lost while inlining passthrough
  //But this is what we want because some inputs are unconnected
  def->connect(pt->sel({"in","in0"}),PE->sel({"data","in","0"}));
  def->connect(pt->sel({"in","in1"}),PE->sel({"data","in","1"}));
  def->connect(pt->sel({"in","out"}),PE->sel({"bit","out"}));
  
  //Remove instance and inline passthrough
  def->removeInstance(inst);
  inlineInstance(pt);
  return true;
}

//Replaces mux with PE
bool muxOpReplacement(Instance* inst) {
  Context* c = inst->getContext();
  ModuleDef* def = inst->getContainer();
  string iname = inst->getInstname();
  //For now just use the coreir lib name as the op
  string opstr = inst->getInstantiableRef()->getName();
  Args PEArgs({{"op",c->argString("mux")}});
  Instance* PE = def->addInstance(iname+"_PE","cgralib.PE",Args(),PEArgs);
  
  //Isolate the instance
  Instance* pt = addPassthrough(inst,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),inst);
  //Some of these connections might be lost while inlining passthrough
  //But this is what we want because some inputs are unconnected
  def->connect(pt->sel({"in","in0"}),PE->sel({"data","in","0"}));
  def->connect(pt->sel({"in","in1"}),PE->sel({"data","in","1"}));
  def->connect(pt->sel({"in","sel"}),PE->sel({"bit","in","0"}));
  def->connect(pt->sel({"in","out"}),PE->sel({"data","out"}));
  
  //Remove instance and inline passthrough
  def->removeInstance(inst);
  inlineInstance(pt);
  return true;
}

//This will assume lbMem will have been linked with a cgra def
bool rungenAndReplace(Instance* inst) {
  Namespace* ns = inst->getInstantiableRef()->getNamespace();
  inst->runGenerator();
  Module* m = inst->getModuleRef();
  if (!ns->hasModule(m->getName())) {
    ns->addModule(m);
  }
  inlineInstance(inst);
  return true;
}

bool removeInstance(Instance* inst) {
  inst->getContainer()->removeInstance(inst);
  return true;
}

}


std::string MapperPasses::TechMapping::ID = "techmapping";

void MapperPasses::TechMapping::setVisitorInfo() {
  Context* c = this->getContext();
  addVisitorFunction(c->getInstantiable("commonlib.lutN"),lutReplacement);
  addVisitorFunction(c->getInstantiable("coreir.uge"),compOpReplacement);
  addVisitorFunction(c->getInstantiable("coreir.ule"),compOpReplacement);
  addVisitorFunction(c->getInstantiable("coreir.mux"),muxOpReplacement);
  addVisitorFunction(c->getInstantiable("coreir.term"),removeInstance);
  addVisitorFunction(c->getInstantiable("coreir.bitterm"),removeInstance);
  addVisitorFunction(c->getInstantiable("commonlib.LinebufferMem"),rungenAndReplace);
  addVisitorFunction(c->getInstantiable("commonlib.smax"),rungenAndReplace);
  
  //TODO what about dlshl
  for (auto str : {"add","sub","dshl","dashr","mul","or","and","xor"}) {
    addVisitorFunction(c->getInstantiable("coreir." + string(str)),binaryOpReplacement);
  }


}
