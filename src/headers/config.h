#ifndef CONFIG_H_
#define CONFIG_H_

enum PE_flag_sel {
  F_EQ=0,
  F_NE=1,
  F_CS=2,
  F_CC=3,
  F_MI=4,
  F_PL=5,
  F_VS=6,
  F_VC=7,
  F_HI=8,
  F_LS=9,
  F_GE=10,
  F_LT=11,
  F_GT=12,
  F_LE=13,
  F_LUT=14,
  F_PRED=15,
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
