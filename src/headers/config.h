#ifndef CONFIG_H_
#define CONFIG_H_


#define PE_FLAG_SEL_LIST\
  _X(F_EQ,0)\
  _X(F_NE,1)\
  _X(F_CS,2)\
  _X(F_CC,3)\
  _X(F_MI,4)\
  _X(F_PL,5)\
  _X(F_VS,6)\
  _X(F_VC,7)\
  _X(F_HI,8)\
  _X(F_LS,9)\
  _X(F_GE,10)\
  _X(F_LT,11)\
  _X(F_GT,12)\
  _X(F_LE,13)\
  _X(F_LUT,14)\
  _X(F_PRED,15)

const char* PE_flag_sel_str[] = {
  #define _X(name, idx) #name,
  PE_FLAG_SEL_LIST
  #undef _X
};

enum PE_flag_sel {
  #define _X(name, idx) name = idx,
  PE_FLAG_SEL_LIST
  #undef _X
}; 

namespace {
  int op_size = 6;
  int flag_sel_size = 4;
}

enum PE_op {
  OP_ADD=0,
  OP_SUB=1,
  OP_ABS=3,
  OP_GTE_MAX=4,
  OP_LTE_MIN=5,
  OP_EQ=6,
  OP_SEL=8,
  OP_RSHIFT=0xF,
  OP_LSHIFT=0x11,
  OP_MULT_0=0xB,
  OP_MULT_1=0xC,
  OP_MULT_2=0xD,
  OP_RELU=0xE,
  OP_OR=0x12,
  OP_AND=0x13,
  OP_XOR=0x14,
  OP_INV=0x15,
  OP_CNTR=0x18
};

#endif
