
using namespace CoreIR;

namespace MapperPasses {
class VerifyCanMap : public InstancePass {
  public :
    static string ID;
    VerifyCanMap() : InstancePass(ID,"Errors if it cannot map",true) {}
    bool runOnInstance(Instance* inst);
};

}

std::string MapperPasses::VerifyCanMap::ID = "verifycanmap";
bool MapperPasses::VerifyCanMap::runOnInstance(Instance* inst) {
  Context* c = this->getContext();
  
  auto iref = inst->getInstantiableRef();
  
  //TODO write this better
  bool isLeaf = false;
  if (iref == c->getInstantiable("commonlib.LinebufferMem") ) {
    isLeaf = true;
  }
  else if (iref == c->getInstantiable("commonlib.smax") ) {
    isLeaf = true;
  }
  else if( iref == c->getInstantiable("coreir.reg")) {
    Args genargs = inst->getGenArgs();
    ASSERT(genargs["en"]->get<bool>()==false,"NYI registers with en");
    ASSERT(genargs["clr"]->get<bool>()==false,"NYI registers with en");
    isLeaf = true;
  }
  else if (iref->getNamespace() == c->getNamespace("coreir")) {
    ASSERT(iref != c->getInstantiable("coreir.slice"),"NYI Slice");
    ASSERT(iref != c->getInstantiable("coreir.concat"),"NYI Concat");
    isLeaf = true;
  }
  else {
    ASSERT(isa<Module>(iref),"Do not know how to map: " + iref->getRefName());
    ASSERT(cast<Module>(iref)->hasDef(),"DO not know how to map: " + iref->getRefName());
  }

  //If it is a leaf then verify that there are no selects on the ports
  if (!isLeaf) return false;
  for (auto wsel : inst->getSelects()) {
    ASSERT(wsel.second->getSelects().size()==0,"NYI: subselecting from a primitive");
  }
  return false;
}


