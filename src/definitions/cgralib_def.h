
#include "../headers/config.h"

using namespace CoreIR;

//Assumes common has been loaded
void load_mem_ext(Context* c) {
  //Specialized extensions
  Generator* lbmem = c->getGenerator("memory.rowbuffer");
  lbmem->setGeneratorDefFromFun([](Context* c, Values args, ModuleDef* def) {
    uint width = args.at("width")->get<int>();
    uint depth = args.at("depth")->get<int>();
    ASSERT(width==16,"NYI Non 16 bit width");
    ASSERT(depth<=1024,"NYI using mutliple memories");
    Values rbGenargs({{"width",Const::make(c,width)},{"total_depth",Const::make(c,1024)}});
    def->addInstance("cgramem","cgralib.Mem",
      rbGenargs,
      {{"mode",Const::make(c,"linebuffer")},{"depth",Const::make(c,depth)}});
    def->addInstance("c1","corebit.const",{{"value",Const::make(c,true)}});
    def->connect("self.wdata","cgramem.wdata");
    def->connect("self.wen","cgramem.wen");
    def->connect("self.rdata","cgramem.rdata");
    def->connect("self.valid","cgramem.valid");
    def->connect("c1.out","cgramem.cg_en");
    def->connect("c1.out","cgramem.ren");

  });

}

void load_commonlib_ext(Context* c) {
  Generator* smax = c->getGenerator("commonlib.smax");
  smax->setGeneratorDefFromFun([](Context* c, Values args, ModuleDef* def) {
    uint width = args.at("width")->get<int>();
    ASSERT(width==16,"NYI non 16");
    Values PEArgs({
      {"alu_op",Const::make(c,op_size,OP_GTE_MAX)},
      {"alu_op_debug",Const::make(c,"max")},
      {"flag_sel",Const::make(c,flag_sel_size,F_PRED)},
      {"signed",Const::make(c,1,1)}
    });
    def->addInstance("cgramax","cgralib.PE",{{"op_kind",Const::make(c,"combined")}},PEArgs);
    def->connect("self.in0","cgramax.data.in.0");
    def->connect("self.in1","cgramax.data.in.1");
    def->connect("self.out","cgramax.data.out");

  });
}

void load_opsubstitution(Context* c) {
  //CoreIR ops
  //op substituions (coreir prims in terms of other coreir prims)
  
  //Changing all the comparison's to <u|s><le|ge>
  vector<string> signs({"u","s"});
  vector<std::pair<string,string>> ops({
    {"gt","le"},
    {"lt","ge"}
  });
  for (auto sign : signs) {
    for (auto op : ops) {
      string from = "coreir." + string(sign)+op.first;
      string to = "coreir." + string(sign)+op.second;
      Module* mod = c->getGenerator(from)->getModule({{"width",Const::make(c,16)}});
      ModuleDef* def = mod->newModuleDef();
      def->addInstance("comp",to);
      def->addInstance("not","corebit.not");
      def->connect("self.in0","comp.in0");
      def->connect("self.in1","comp.in1");
      def->connect("comp.out","not.in");
      def->connect("not.out","self.out");
      mod->setDef(def);
    }
  }

  //coreir.neg should be  0 - in
  c->getGenerator("coreir.neg")->setGeneratorDefFromFun([](Context* c, Values args, ModuleDef* def) {
    def->addInstance("sub","coreir.sub");
    def->addInstance("c0","coreir.const",Values(),{{"value",Const::make(c,16,0)}});
    def->connect("self.in","sub.in1");
    def->connect("c0.out","sub.in0");
    def->connect("sub.out","self.out");
  });

}

void load_corebit2lut(Context* c) {
#define B0 170
#define B1 (12*17)
#define B2 (15*16)
  
  {
    //unary
    Module* mod = c->getModule("corebit.not");
    ModuleDef* def = mod->newModuleDef();
    //Add the Lut
    def->addInstance("lut","commonlib.lutN",Values(),{{"init",Const::make(c,8,~B0)}});
    def->addInstance("c0","corebit.const",{{"value",Const::make(c,false)}});
    def->connect("self.in","lut.in.0");
    def->connect("c0.out","lut.in.1");
    def->connect("c0.out","lut.in.2");
    def->connect("lut.out","self.out");
    mod->setDef(def);
  }
  vector<std::pair<string,uint>> binops({{"and",B0&B1},{"or",B0|B1},{"xor",B0^B1}});
  for (auto op : binops) {
    Value* lutval = Const::make(c,8,op.second);
    Module* mod = c->getModule("corebit."+op.first);
    ModuleDef* def = mod->newModuleDef();
    //Add the Lut
    def->addInstance("lut","commonlib.lutN",Values(),{{"init",lutval}});
    def->addInstance("c0","corebit.const",{{"value",Const::make(c,false)}});
    def->connect("self.in0","lut.in.0");
    def->connect("self.in1","lut.in.1");
    def->connect("c0.out","lut.in.2");
    def->connect("lut.out","self.out");
    mod->setDef(def);
  }
  {
    //mux
    Module* mod = c->getModule("corebit.mux");
    ModuleDef* def = mod->newModuleDef();
    //Add the Lut
    def->addInstance("lut","commonlib.lutN",Values(),{{"init",Const::make(c,8,(B2&B1)|((~B2)&B0))}});
    def->connect("self.in0","lut.in.0");
    def->connect("self.in1","lut.in.1");
    def->connect("self.sel","lut.in.2");
    def->connect("lut.out","self.out");
    mod->setDef(def);
  }

#undef B0
#undef B1
#undef B2
}
void load_cgramapping(Context* c) {
  //commonlib.lut def
  {
    Module* mod = c->getGenerator("commonlib.lutN")->getModule({{"N",Const::make(c,3)}});
    ModuleDef* def = mod->newModuleDef();
    Values bitPEArgs({{"lut_value",mod->getArg("init")}});
    def->addInstance(+"lut","cgralib.PE",{{"op_kind",Const::make(c,"bit")}},bitPEArgs);
    
    def->connect("self.in","lut.bit.in");
    def->connect("lut.bit.out","self.out");
    mod->setDef(def);
  }
  {
    //unary op width)->width
    std::vector<std::tuple<string,uint,uint>> unops = {
        std::make_tuple("not",OP_INV,0),
    };
    for (auto op : unops) {
      string opstr = std::get<0>(op);
      uint alu_op = std::get<1>(op);
      uint is_signed = std::get<2>(op);
      Module* mod = c->getGenerator("coreir."+opstr)->getModule({{"width",Const::make(c,16)}});
      ModuleDef* def = mod->newModuleDef();
      Values dataPEArgs({
        {"alu_op",Const::make(c,op_size,alu_op)},
        {"alu_op_debug",Const::make(c,opstr)},
        {"signed",Const::make(c,1,is_signed)}});
      def->addInstance("binop","cgralib.PE",{{"op_kind",Const::make(c,"alu")}},dataPEArgs);
    
      def->connect("self.in","binop.data.in.0");
      def->connect("self.out","binop.data.out");
      mod->setDef(def);
    }
  }
  {
    //binary op (width,width)->width
    std::vector<std::tuple<string,uint,uint>> binops({
      std::make_tuple("add",OP_ADD,0),
      std::make_tuple("sub",OP_SUB,0),
      std::make_tuple("mul",OP_MULT_0,0),
      std::make_tuple("or",OP_OR,0),
      std::make_tuple("and",OP_AND,0),
      std::make_tuple("xor",OP_XOR,0),
      std::make_tuple("ashr",OP_RSHIFT,0),
      std::make_tuple("shl",OP_LSHIFT,0),
    });
    for (auto op : binops) {
      string opstr = std::get<0>(op);
      uint alu_op = std::get<1>(op);
      uint is_signed = std::get<2>(op);
      Module* mod = c->getGenerator("coreir."+opstr)->getModule({{"width",Const::make(c,16)}});
      ModuleDef* def = mod->newModuleDef();
      Values dataPEArgs({
        {"alu_op",Const::make(c,op_size,alu_op)},
        {"alu_op_debug",Const::make(c,opstr)},
        {"signed",Const::make(c,1,is_signed)}});
      def->addInstance("binop","cgralib.PE",{{"op_kind",Const::make(c,"alu")}},dataPEArgs);
    
      def->connect("self.in0","binop.data.in.0");
      def->connect("self.in1","binop.data.in.1");
      def->connect("self.out","binop.data.out");
      mod->setDef(def);
    }
  }
  //Mux
  {
    Module* mod = c->getGenerator("coreir.mux")->getModule({{"width",Const::make(c,16)}});
    ModuleDef* def = mod->newModuleDef();
    Values PEArgs({
      {"alu_op",Const::make(c,op_size,OP_SEL)},
      {"alu_op_debug",Const::make(c,"mux")},
      {"flag_sel",Const::make(c,flag_sel_size,F_PRED)},
      {"signed",Const::make(c,1,0)}
    });
    def->addInstance("mux","cgralib.PE",{{"op_kind",Const::make(c,"combined")}},PEArgs);
    def->connect("self.in0","mux.data.in.0");
    def->connect("self.in1","mux.data.in.1");
    def->connect("self.sel","mux.bit.in.0");
    def->connect("mux.data.out","self.out");
    mod->setDef(def);
  }
  {
    //comp op (width,width)->bit
    std::vector<std::tuple<string,uint,uint,uint>> compops({
      std::make_tuple("eq",OP_SUB,F_EQ,0),
      std::make_tuple("neq",OP_SUB,F_NE,0),
      std::make_tuple("sge",OP_GTE_MAX,F_PRED,1),
      std::make_tuple("uge",OP_GTE_MAX,F_PRED,0),
      std::make_tuple("sle",OP_LTE_MIN,F_PRED,1),
      std::make_tuple("ule",OP_LTE_MIN,F_PRED,0),
    });
    for (auto op : compops) {
      string opstr = std::get<0>(op);
      uint alu_op = std::get<1>(op);
      uint flag_sel = std::get<2>(op);
      uint is_signed = std::get<3>(op);
      Module* mod = c->getGenerator("coreir."+opstr)->getModule({{"width",Const::make(c,16)}});
      ModuleDef* def = mod->newModuleDef();
      Values PEArgs({
        {"alu_op",Const::make(c,op_size,alu_op)},
        {"alu_op_debug",Const::make(c,opstr)},
        {"flag_sel",Const::make(c,flag_sel_size,flag_sel)},
        {"signed",Const::make(c,1,is_signed)}
      });
      def->addInstance("compop","cgralib.PE",{{"op_kind",Const::make(c,"combined")}},PEArgs);
    
      def->connect("self.in0","compop.data.in.0");
      def->connect("self.in1","compop.data.in.1");
      def->connect("self.out","compop.bit.out");
      mod->setDef(def);
    }
  }
  
  //term
  {
    Module* mod = c->getGenerator("coreir.term")->getModule({{"width",Const::make(c,16)}});
    ModuleDef* def = mod->newModuleDef();
    mod->setDef(def); //TODO is this even valid?
  }

  //bitterm
  {
    Module* mod = c->getModule("corebit.term");
    ModuleDef* def = mod->newModuleDef();
    mod->setDef(def); //TODO is this even valid?
  }


}

void LoadDefinition_cgralib(Context* c) {

  load_mem_ext(c);
  load_commonlib_ext(c);
  load_opsubstitution(c);
  load_corebit2lut(c);
  load_cgramapping(c);

}

