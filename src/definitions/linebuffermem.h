
using namespace CoreIR;

//Assumes common has been loaded
void LoadDefinition_LinebufferMem(Context* c) {
  Generator* lbmem = c->getGenerator("commonlib.LinebufferMem");
  lbmem->setGeneratorDefFromFun([](Context* c, Values args, ModuleDef* def) {
    uint width = args.at("width")->get<int>();
    uint depth = args.at("depth")->get<int>();
    ASSERT(width==16,"NYI Non 16 bit width");
    ASSERT(depth<=1024,"NYI using mutliple memories");
    def->addInstance("cgramem","cgralib.Mem",{{"width",Const::make(c,width)},{"depth",Const::make(c,1024)}},{{"mode",Const::make(c,"linebuffer")},{"fifo_depth",Const::make(c,depth)}});
    
    def->connect("self.wdata","cgramem.wdata");
    def->connect("self.wen","cgramem.wen");
    def->connect("self.rdata","cgramem.rdata");
    def->connect("self.valid","cgramem.valid");

  });

  Generator* smax = c->getGenerator("commonlib.smax");
  smax->setGeneratorDefFromFun([](Context* c, Values args, ModuleDef* def) {
    uint width = args.at("width")->get<int>();
    ASSERT(width==16,"NYI non 16");
    def->addInstance("cgramax","cgralib.PE",{{"op_kind",Const::make(c,"combined")}},{{"alu_op",Const::make(c,"max")}});
    def->connect("self.in0","cgramax.data.in.0");
    def->connect("self.in1","cgramax.data.in.1");
    def->connect("self.out","cgramax.data.out");

  });

}
