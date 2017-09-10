
using namespace CoreIR;

namespace MapperPasses {
class VerifyCanMap : public InstancePass {
  public :
    static string ID;
    VerifyCanMap() : InstancePass(ID,"Errors if it cannot map",true) {}
    bool runOnInstance(Instance* inst);
};

}

namespace {
//Checks if Type is Bit, BitIn, Clk,Rst, Array(16,Bit,BitIn)
void checkType(Context* c,Type* t) {
  if (
    isa<BitType>(t) ||
    isa<BitInType>(t) ||
    t == c->Named("coreir.clk") ||
    t == c->Named("coreir.clkIn") ||
    t == c->Named("coreir.rst") ||
    t == c->Named("coreir.rstIn")
  ) return;
  if (auto at = dyn_cast<ArrayType>(t)) {
    ASSERT(at->getLen()==16,"NYI, Arrays have to be BitIn[16] or Bit[16]. Type=" + t->toString());
    ASSERT(isa<BitType>(at->getElemType()) || isa<BitInType>(at->getElemType()),"Need to flatten types: " + t->toString());
  }
  else {
    ASSERT(0,"Bad Type for mapping! " + t->toString());
  }
}
}


std::string MapperPasses::VerifyCanMap::ID = "verifycanmap";
bool MapperPasses::VerifyCanMap::runOnInstance(Instance* inst) {
  Context* c = this->getContext();
  for (auto rmap : cast<RecordType>(inst->getType())->getRecord()) {
    Type* t = rmap.second;
    checkType(c,t);
  }

  
  auto iref = inst->getInstantiableRef();
  
  if (iref == c->getInstantiable("commonlib.LinebufferMem") ){
    return false;
  }
  if (iref == c->getInstantiable("coreir.reg")) {
    Args genargs = inst->getGenArgs();
    ASSERT(genargs["en"]->get<ArgBool>()==false,"NYI registers with en");
    ASSERT(genargs["clr"]->get<ArgBool>()==false,"NYI registers with en");
  }

  if (iref->getNamespace() == c->getNamespace("coreir")) return false;
  ASSERT(isa<Module>(iref),"Do not know how to map: " + iref->getRefName());
  ASSERT(cast<Module>(iref)->hasDef(),"DO not know how to map: " + iref->getRefName());
  return false;
}


