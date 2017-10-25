
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
  i->print();
  //This needs to be either:
  //  coreir.const
  //  coreir.reg
  //  cgralib.*
  //  Or a module with a definition
  if (c->hasInstantiable("coreir.const") && i == c->getInstantiable("coreir.const")) return false;
  if (c->hasInstantiable("coreir.reg") && i == c->getInstantiable("coreir.reg")) return false;
  if (c->hasInstantiable("coreir.dff") && i == c->getInstantiable("corebit.dff")) return false;
  if (i->getNamespace() == c->getNamespace("cgralib")) return false;
  ASSERT(isa<Module>(i),"NYI mapping " +i->toString() + ". Needs to be a module with def!");
  ASSERT(cast<Module>(i)->hasDef(),"NYI mapping " +i->toString() + ". Needs to be a module with def!");
  return false;
}

