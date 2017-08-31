
using namespace CoreIR;

namespace MapperPasses {
class OpSubstitution : public InstanceVisitorPass {
  public :
    static std::string ID;
    OpSubstitution() : InstanceVisitorPass(ID,"Does substituions like LT -> GTE + Not") {}
    void setVisitorInfo() override;
};

}

//TODO also replace Static SHifts with Dynamic SHifts
namespace {
//Replaces UGT with ULTE + bitnot
bool UGTReplacement(Instance* gt) {
  Context* c = gt->getContext();
  ModuleDef* def = gt->getContainer();
  string iname = gt->getInstname();
  //Add the ule + not instances
  Instance* ule = def->addInstance(iname+"_ule","coreir.ule");
  Instance* bitnot = def->addInstance(iname + "_not","coreir.bitnot");
  def->connect(ule->sel("out"),bitnot->sel("in"));
  
  //Isolate the instance
  Instance* pt = addPassthrough(gt,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),gt);
  def->connect(pt->sel("in")->sel("in0"),ule->sel("in0"));
  def->connect(pt->sel("in")->sel("in1"),ule->sel("in1"));
  def->connect(pt->sel("in")->sel("out"),bitnot->sel("out"));
  
  //Remove instance and inline passthrough
  def->removeInstance(gt);
  inlineInstance(pt);
  return true;
}

//Replaces ULT with UGTE + bitnot
bool ULTReplacement(Instance* gt) {
  Context* c = gt->getContext();
  ModuleDef* def = gt->getContainer();
  string iname = gt->getInstname();
  //Add the ule + not instances
  Instance* uge = def->addInstance(iname+"_uge","coreir.uge");
  Instance* bitnot = def->addInstance(iname + "_not","coreir.bitnot");
  def->connect(uge->sel("out"),bitnot->sel("in"));
  
  //Isolate the instance
  Instance* pt = addPassthrough(gt,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),gt);
  def->connect(pt->sel("in")->sel("in0"),uge->sel("in0"));
  def->connect(pt->sel("in")->sel("in1"),uge->sel("in1"));
  def->connect(pt->sel("in")->sel("out"),bitnot->sel("out"));
  
  //Remove instance and inline passthrough
  def->removeInstance(gt);
  inlineInstance(pt);
  return true;
}

//Replaces neg with 0 - val
bool NegReplacement(Instance* neg) {
  Context* c = neg->getContext();
  ModuleDef* def = neg->getContainer();
  string iname = neg->getInstname();
  //Add the ule + not instances
  Instance* sub = def->addInstance(iname+"_sub","coreir.sub");
  Instance* zero = def->addInstance(iname + "_not","coreir.const",{},{{"value",c->argInt(0)}});
  def->connect(sub->sel("in0"),zero->sel("out"));
  
  //Isolate the instance
  Instance* pt = addPassthrough(neg,"_pt"+c->getUnique());
  def->disconnect(pt->sel("in"),neg);
  def->connect(pt->sel("in")->sel("in"),sub->sel("in1"));
  def->connect(pt->sel("in")->sel("out"),sub->sel("out"));
  
  //Remove instance and inline passthrough
  def->removeInstance(neg);
  inlineInstance(pt);
  return true;
}


}


std::string MapperPasses::OpSubstitution::ID = "opsubstitution";
void MapperPasses::OpSubstitution::setVisitorInfo() {
  Context* c = this->getContext();
  addVisitorFunction(c->getInstantiable("coreir.ugt"),UGTReplacement);
  addVisitorFunction(c->getInstantiable("coreir.ult"),ULTReplacement);
  addVisitorFunction(c->getInstantiable("coreir.neg"),NegReplacement);

}
