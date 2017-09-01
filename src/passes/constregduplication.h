
using namespace CoreIR;

namespace MapperPasses {
class ConstDuplication : public InstanceVisitorPass {
  public :
    static std::string ID;
    ConstDuplication() : InstanceVisitorPass(ID,"Does substituions like LT -> GTE + Not") {}
    void setVisitorInfo() override;
};

}

namespace {
//Any time const has fanout, duplicate it
bool ConstDup(Instance* con) {
  ModuleDef* def = con->getContainer();
  string iname = con->getInstname();

  Wireable* conout = con->sel("out");
  ASSERT(conout->getSelects().size()==0,"NYI, const with selects");
  uint fanout = conout->getConnectedWireables().size();
  if (fanout==1) return false;
  
  vector<Wireable*> connected;
  for (auto w : conout->getConnectedWireables()) {
    connected.push_back(w);
  }
  for (uint i=0; i<fanout-1; ++i) {
    Instance* newConst = def->addInstance(con,iname+"_dup"+to_string(i));
    def->disconnect(conout,connected[i]);
    def->connect(newConst->sel("out"),connected[i]);
  }
  return true;
}

////if reg has fanout, duplicate it (sometimes)
//bool RegDup(Instance* con) {
//  Context* c = con->getContext();
//  ModuleDef* def = con->getContainer();
//  string iname = con->getInstname();
//
//  Wireable* conout = con->sel("out");
//  ASSERT(conout->getSelects().size()==0,"NYI, const with selects");
//  uint fanout = conout->getConnectedWireables().size();
//  if (fanout==1) return false;
//  
//  vector<Wireable*> connected;
//  for (auto w : conout->getConnectedWireables()) {
//    //Verify that each thing it is connected to is either PE,bitPE or dataPE. If it is not, then stop
//    Wireable* topW = w->getTopParent();
//    if (isa<Interface>(topW)) return false;
//    Instance* conInst = cast<Instance>(topW);
//    auto iref = conInst->getInstantiableRef();
//    if ( iref != c->getInstantiable("cgralib.Mem")
//      && iref != c->getInstantiable("cgralib.dataMem")
//      && iref != c->getInstantiable("cgralib.bitMem")
//    ) return false;
//
//    connected.push_back(w);
//  }
//  for (uint i=1; i<fanout; ++i) {
//    Instance* newReg = def->addInstance(reg,iname+"_dup"+to_string(i));
//    def->disconnect(conout,connected[i]);
//    def->connect(newConst->sel("out"),connected[i]);
//  }
//  return true;
//}

}

std::string MapperPasses::ConstDuplication::ID = "constduplication";
void MapperPasses::ConstDuplication::setVisitorInfo() {
  Context* c = this->getContext();
  addVisitorFunction(c->getInstantiable("coreir.const"),ConstDup);

}
