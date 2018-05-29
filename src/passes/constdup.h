
using namespace CoreIR;

//TODO this pass and the constRegFolding could probably just be done as one single pass
namespace MapperPasses {
class ConstDuplication : public InstanceVisitorPass {
  public :
    static std::string ID;
    ConstDuplication() : InstanceVisitorPass(ID,"duplicate all constants") {}
    void setVisitorInfo() override;
};

}

namespace {

bool ConstDup(Instance* cnst) {
  Module* modRef = cnst->getModuleRef();

  auto connSet = cnst->sel("out")->getConnectedWireables();
  if (connSet.size() < 1) {
    return false;
  }
  vector<Wireable*> conns(connSet.begin(),connSet.end());

  ModuleDef* def = cnst->getContainer();
  for (int i=1; i< conns.size(); ++i) {
    Wireable* conn = conns[i];
    cout << "replacing connection to : " << conn->toString() << endl;
    Instance* newconst = def->addInstance(cnst->getInstname() + to_string(i),modRef,cnst->getModArgs());
    def->connect(newconst->sel("out"),conn);
    def->disconnect(cnst->sel("out"),conn);
  }
  return true;
}

}

std::string MapperPasses::ConstDuplication::ID = "constduplication";
void MapperPasses::ConstDuplication::setVisitorInfo() {
  Context* c = this->getContext();
  if (c->hasModule("corebit.const")) {
    addVisitorFunction(c->getModule("corebit.const"),ConstDup);
  }

}
