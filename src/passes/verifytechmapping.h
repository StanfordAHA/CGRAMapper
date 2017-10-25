
using namespace CoreIR;

namespace MapperPasses {
class VerifyTechMapping : public InstanceGraphPass {
  public :
    static std::string ID;
    VerifyTechMapping() : InstanceGraphPass(ID,"Verifies all external links are cgra") {}
    bool runOnInstanceGraphNode(InstanceGraphNode& node) override;
};
}

std::string MapperPasses::VerifyTechMapping::ID = "verifytechmapping";
bool MapperPasses::VerifyTechMapping::runOnInstanceGraphNode(InstanceGraphNode& node) {
  Context* c = this->getContext();
  Instantiable* i = node.getInstantiable();
  Module* m = cast<Module>(i);
  //This needs to be either:
  //  coreir.const
  //  coreir.reg
  //  cgralib.*
  //  Or a module with a definition
  if (m->getNamespace() == c->getNamespace("cgralib")) return false;
  if (m->getRefName() == "coreir.const") return false;
  if (m->getRefName() == "coreir.reg") return false;
  if (m->getRefName() == "corebit.dff") return false;
  ASSERT(isa<Module>(i),"NYI mapping " +i->toString() + ". Needs to be a module with def!");
  ASSERT(cast<Module>(i)->hasDef(),"NYI mapping " +i->toString() + ". Needs to be a module with def!");
  return false;
}

