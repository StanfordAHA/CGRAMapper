
using namespace CoreIR;

namespace MapperPasses {
class VerifyTechMapping : public InstanceGraphPass {
  public :
    static std::string ID;
    VerifyTechMapping() : InstanceGraphPass(ID,"Verifies all external links are cgra") {}
    bool runOnInstanceGraphNode(InstanceGraphNode& node) override;
    void setAnalysisInfo() override {
      this->onlyTop = true;
    }

};
}

std::string MapperPasses::VerifyTechMapping::ID = "verifytechmapping";
bool MapperPasses::VerifyTechMapping::runOnInstanceGraphNode(InstanceGraphNode& node) {
  Context* c = this->getContext();
  Module* m = node.getModule();
  //This needs to be either:
  //  coreir.const
  //  coreir.reg
  //  cgralib.*
  //  Or a module with a definition
  if (m->getNamespace() == c->getNamespace("cgralib")) return false;
  if (m->getRefName() == "coreir.const") return false;
  if (m->getRefName() == "coreir.reg") return false;
  if (m->getRefName() == "corebit.const") return false;
  if (m->getRefName() == "corebit.reg") return false;
  if (!m->hasDef()) {
    m->print();
  }
  ASSERT(m->hasDef(),"NYI mapping primitive " +m->getRefName() + ". Needs to be a module with def!");
  return false;
}

