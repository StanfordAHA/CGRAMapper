
using namespace CoreIR;

//TODO this pass and the constRegFolding could probably just be done as one single pass
namespace MapperPasses {
class MemConst : public InstanceVisitorPass {
  public :
    static std::string ID;
    MemConst() : InstanceVisitorPass(ID,"replace mem wen const with lut") {}
    void setVisitorInfo() override;
};

}

namespace {


bool ConstReplace(Instance* cnst) {
  cout << "cnstreplace" << endl;
  cout << toString(cnst) << endl;
  Context* c = cnst->getContext();
  auto conns = cnst->sel("out")->getConnectedWireables();
  if (conns.size()==0) {
    return false;
  }
  ASSERT(conns.size()==1,"size: " + to_string(conns.size()));
  for (auto conn : conns) {
    if (auto conInst = dyn_cast<Instance>(conn->getTopParent())) {
      if (conInst->getModuleRef()->getRefName() != "cgralib.Mem" || conInst->getSelectPath().back()!="wen") {
        return false;
      }
    }
  }
  ModuleDef* def = cnst->getContainer();
  uint val = cnst->getModArgs().at("value")->get<bool>() ? 63 : 0;
  Values bitPEArgs({{"lut_value",Const::make(c,8,val)}});
  Instance* lut = def->addInstance(cnst->getInstname()+"_lutcnst","cgralib.PE",{{"op_kind",Const::make(c,"bit")}},bitPEArgs);
  for (auto conn : conns) {
    def->connect(lut->sel("bit")->sel("out"),conn);
  }
  def->removeInstance(cnst);
  return true;
}

}

std::string MapperPasses::MemConst::ID = "memconst";
void MapperPasses::MemConst::setVisitorInfo() {
  Context* c = this->getContext();
  if (c->hasModule("corebit.const")) {
    addVisitorFunction(c->getModule("corebit.const"),ConstReplace);
  }

}
