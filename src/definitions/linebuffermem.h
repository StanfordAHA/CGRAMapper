
using namespace CoreIR;

//Assumes common has been loaded
void LoadDefinition_LinebufferMem(Context* c) {
  Generator* lbmem = c->getGenerator("commonlib.LinebufferMem");
  lbmem->setGeneratorDefFromFun([](ModuleDef* def,Context* c, Type* t, Args args) {
    uint width = args.at("width")->get<ArgInt>();
    uint depth = args.at("depth")->get<ArgInt>();
    ASSERT(width==16,"NYI Non 16 bit width");
    ASSERT(depth<=1024,"NYI using mutliple memories");
    def->addInstance("cgramem","cgralib.Mem",{{"width",c->argInt(width)},{"depth",c->argInt(1024)}},{{"mode",c->argString("linebuffer")},{"fifo_depth",c->argInt(depth)}});
    
    def->connect("self.wdata","cgramem.wdata");
    def->connect("self.wen","cgramem.wen");
    def->connect("self.rdata","cgramem.rdata");
    def->connect("self.valid","cgramem.valid");

  });
}
