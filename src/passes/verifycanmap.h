
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
  
  auto mref = inst->getModuleRef();
  string mrefname = mref->getRefName();
  ASSERT(mrefname != "coreir.slice","NYI Slice");
  ASSERT(mrefname != "coreir.concat","NYI Concat");
  
  bool isLeaf = false;
  if (mrefname == "commonlib.LinebufferMem" ) {
    isLeaf = true;
  }
  else if (mrefname == "commonlib.smax") {
    isLeaf = true;
  }
  else if (mref->getNamespace() == c->getNamespace("coreir")) {
    isLeaf = true;
  }
  else if (mref->getNamespace() == c->getNamespace("corebit")) {
    isLeaf = true;
  }
  else {
    ASSERT(mref->hasDef(),"NYI, Do not know how to map: " + mrefname);
  }

  //If it is a leaf then verify that there are no selects on the ports
  if (!isLeaf) return false;
  for (auto wsel : inst->getSelects()) {
    ASSERT(wsel.second->getSelects().size()==0,"NYI: subselecting from a primitive");
  }
  return false;
}


