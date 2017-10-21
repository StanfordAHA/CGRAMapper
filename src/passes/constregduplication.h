
using namespace CoreIR;

//TODO this pass and the constRegFolding could probably just be done as one single pass
namespace MapperPasses {
class ConstRegDuplication : public InstanceVisitorPass {
  public :
    static std::string ID;
    ConstRegDuplication() : InstanceVisitorPass(ID,"Duplicates constants and registers") {}
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
bool RegDup(Instance* reg) {
  Context* c = reg->getContext();
  ModuleDef* def = reg->getContainer();
  string iname = reg->getInstname();

  ASSERT(reg->getGenArgs().at("en")->get<ArgBool>()==false,"NYI regs with en");
  ASSERT(reg->getGenArgs().at("clr")->get<ArgBool>()==false,"NYI regs with clr");

  Wireable* regout = reg->sel("out");
  ASSERT(regout->getSelects().size()==0,"NYI, reg with selects");
  uint fanout = regout->getConnectedWireables().size();
  if (fanout==1) return false;
  
  //Keep track of connected to PEs
  vector<Wireable*> connected;
  for (auto w : regout->getConnectedWireables()) {
    //Verify that each thing it is connected to is either PE,bitPE or dataPE. If it is not, then stop
    Wireable* topW = w->getTopParent();
    if (isa<Interface>(topW)) continue;
    Instance* conInst = cast<Instance>(topW);
    auto iref = conInst->getInstantiableRef();
    if ( iref != c->getInstantiable("cgralib.Mem")
      && iref != c->getInstantiable("cgralib.dataMem")
      && iref != c->getInstantiable("cgralib.bitMem")
    ) continue;

    connected.push_back(w);
  }
  for (auto w : connected) {
    Instance* newReg = def->addInstance(reg,iname+"_dup"+c->getUnique());
    def->disconnect(regout,w);
    def->connect(newReg->sel("out"),w);
  }
  
  //Keep only if connected to a reg or IO
  if (fanout == connected.size()) {
    def->removeInstance(reg);
  }

  return connected.size()>0;
}

}

std::string MapperPasses::ConstRegDuplication::ID = "constregduplication";
void MapperPasses::ConstRegDuplication::setVisitorInfo() {
  Context* c = this->getContext();
  addVisitorFunction(c->getInstantiable("coreir.const"),ConstDup);
  addVisitorFunction(c->getInstantiable("coreir.reg"),ConstDup);

}
