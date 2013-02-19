/* unicore32-opc.h -- Table of opcodes for the UniCore32 processor.
   Copyright 2013 Free Software Foundation, Inc.
   Contributed by LIU Zhiyou (liuzhiyou.cs@gmail.com)

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef UNICORE32_OPC_H
#define UNICORE32_OPC_H

#include "dis-asm.h"
#include "opcode/unicore32.h"
#include "libiberty.h"
#include "safe-ctype.h"
#include <string.h>

/***
 * Some comment to explain why I write code in this
 * header file instead of unicore32-opc.c.
 * In the table of instruction types, i.e. inst_types[],
 * I need to declare different types of instruction and
 * arguments by sepcifying their behaviour during
 * disassemble and assemble using different functions.
 * The problem is that in some argfrom_str_xxx, I need
 * to call some internal function of gas to generate
 * fixup or expression. And these function are not
 * link into libopcodes.a. So if I simply can't implemente
 * these argfrom_str_xxx function in unicore32-opc.c
 * otherwise only gas can correctly link with libopcodes.a
 *
 * And here is current solution.
 * 0. Using #ifndef to check if this file is included by
 * gas, if it's not, define argfrom_str_xxx as NULL to
 * avoid undefinition symbol.
 *
 * 1. include this file in unicore32-dis.c, so basicly
 * libopcodes.a is the same, execpt that all the
 * argfrom_str_xxx in the inst_types[] is NULL.
 *
 * 2. Define argfrom_str_xxx in gas, i.e, tc-unicore32.c,
 * then include this file. and after that we got another
 * inst_types[] with argfrom_str_xxx.
 *
 * Please notice that inst_types[] is declared as static
 * so the one used in tc-unicore32.c is not the one used
 * in unicore32-dis.c
 *
 * I considered setting argfrom_str in md_begin(),
 * But I hate to separate things into two place and
 * the good part of this solution is we can define every
 * thing in the same place (here!) by only using a
 * #ifndef marco. And I know by doing some I waste
 * some memory (with the size of inst_types[]).
 *
 * If you have any idea, I am happy to hear!
 ***/

static unsigned long get_inst_field(inst const *ainst, inst_fields field) {
    return (ainst->raw >> inst_offset[field]) &
        ((1UL << inst_width[field]) - 1UL);
}

static void set_inst_field(inst *ainst, inst_fields field, unsigned long value) {
    ainst->raw |= (value << inst_offset[field]) & inst_mask[field];
}

static long u2s(unsigned long us, short width) {
    return us | (-1L ^ ((1L << width) -1 ));
}

static void init_singlefield(inst *ainst, argument* arg) {
    arg->ucontent.raw = get_inst_field(ainst, arg->type->field1);
    arg->scontent = u2s(arg->ucontent.raw, inst_width[arg->type->field1]);
    return ;
}

static void set_field_singleufield(inst* ainst, argument* arg) {
    unsigned long content = arg->ucontent.raw;
    set_inst_field(ainst, arg->type->field1, content);
}

static void init_combinefield(inst *ainst, argument* arg) {
    unsigned long field_content = get_inst_field(ainst, arg->type->field1);
    field_content <<= inst_width[arg->type->field2];
    field_content |= get_inst_field(ainst, arg->type->field2);
    arg->ucontent.raw = field_content;
    arg->scontent = u2s(arg->ucontent.raw,
                       inst_width[arg->type->field1]+
                       inst_width[arg->type->field2]);
    return ;
}

static void set_field_combineufield(inst* ainst, argument* arg) {
    unsigned long content = arg->ucontent.raw;
    set_inst_field(ainst, arg->type->field2, content);
    set_inst_field(ainst, arg->type->field1,
                   content >> inst_width[arg->type->field2]);
}

/**
 * get register from string. Only support general purpose
 * register now.
 */
static const char* gpreg_map[] =
    {"sl", "fp", "ip", "sp", "lr", "pc"};
#ifdef TC_UNICORE32_H
static regs get_register(const char* str) {
    unsigned long reg_idx;
    while (*str != '\0' && *str == ' ') str++;
    if (*str == '\0') return nullregister;
    if (sscanf(str, "r%lu", &reg_idx)) {
        /* Need more work for support other type of register */
        return reg_idx;
    } else {
        while (str != NULL && *str == ' ' && *str != '\0')
            str++;
        unsigned int i;
        for (i = 0; i < ARRAY_SIZE(gpreg_map); i++)
          if (strlen(str) >= 2 &&
              str[0] == gpreg_map[i][0] && str[1] == gpreg_map[i][1])
            return i+26;
    }
    return nullregister;

}
#endif
/**
 * Get register name. Only support general purpose register now.
 */
static const char* get_register_name(regs areg) {
    static char result[8];
    if (!is_gpreg(areg))
        return NULL;
    if (areg <= r25) {
        snprintf(result, 8, "r%u", areg);
        return result;
    } else {
        return gpreg_map[areg-r26];
    }
}

/***
 * print argument as regard register
 * TODO: Make it better
 ***/
static void print_arg_r(inst *ainst ATTRIBUTE_UNUSED,
                        argument *arg,
                        struct disassemble_info *info ATTRIBUTE_UNUSED,
                        char *output) {
    snprintf(output, PRINT_BUFFER_SIZE,
        "%s", get_register_name(arg->ucontent.areg));
}

/***
 * print argument as shift imm5.
 * in type: D_Imm_Shift, M_Imm_Shift
 ***/
static void print_arg_shiftimm(inst *ainst,
                               argument *arg,
                               struct disassemble_info *info ATTRIBUTE_UNUSED,
                               char *output) {
    char const * shift_map[] = {"<<", ">>", "|>", "<>"};
    unsigned long shift = get_inst_field(ainst, InstField_Shift);
    if (arg->ucontent.uimm == 0) {
        // |> or >>
        if (shift == 2 || shift == 1) {
            snprintf(output, PRINT_BUFFER_SIZE, "%s #32",
                shift_map[shift]);
            return ;
        }
        // <>
        if (shift == 3) {
          snprintf(output, PRINT_BUFFER_SIZE, "<> #33");
          return ;
        }
        else {
            *output = '\0';
            return;
        }
    }
    snprintf(output, PRINT_BUFFER_SIZE,
             "%s #%lu", shift_map[shift], arg->ucontent.uimm);
}

static void print_arg_shiftr(inst *ainst,
                               argument *arg,
                             struct disassemble_info *info ATTRIBUTE_UNUSED,
                             char * output) {
    char const * shift_map[] = {"<<", ">>", "|>", "<>"};
    unsigned long shift = get_inst_field(ainst, InstField_Shift);
    snprintf(output, PRINT_BUFFER_SIZE,
             "%s %s", shift_map[shift],
             get_register_name(arg->ucontent.areg));
}

/***
 * register as address base
 * in type: L/S instructions
 ***/
static void print_arg_rbase(inst *ainst, argument *arg,
                            struct disassemble_info *info ATTRIBUTE_UNUSED,
                            char *output) {
    regs rbase = arg->ucontent.areg;
    const char *fmt[] = {"[%s]%c", "[%s%c]"};
    char op[] = {'-', '+'};
    snprintf(output, PRINT_BUFFER_SIZE,
             fmt[get_inst_field(ainst, InstField_P)],
             get_register_name(rbase),
             op[get_inst_field(ainst, InstField_U)]);
}

/***
 * unsigned imm
 ***/
static void print_arg_uimm(inst *ainst  ATTRIBUTE_UNUSED,
                           argument *arg,
                           struct disassemble_info *info ATTRIBUTE_UNUSED,
                           char *output) {
    unsigned long uimm = arg->ucontent.uimm;
    if (uimm)
        snprintf(output, PRINT_BUFFER_SIZE,
                 "#%lu", uimm);
    else
        *output = '\0';
}

#if 0
/***
 * imm in inst with field `rotate', signed
 * in type: D_Immediate, MvToStFlag
 ***/
static void print_arg_rotateimm(inst *ainst ATTRIBUTE_UNUSED,
                                argument *arg ATTRIBUTE_UNUSED,
                                struct disassemble_info *info) {
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    /* TODO: stub */
    func(stream, "Just a stub");
}



/***
 * signed imm, for example, imm14 on L/S instructions
 ***/
static void print_arg_simm(inst *ainst ATTRIBUTE_UNUSED,
                           argument *arg,
                           struct disassemble_info *info) {
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    func(stream, "#%ld", arg->scontent);
}
#endif

/***
 * Register List, in L/S multipy
 */
static void print_arg_rmulti(inst *ainst,
                              argument *arg,
                              struct disassemble_info *info ATTRIBUTE_UNUSED,
                              char* output)
{
    unsigned long reg_bitmap = arg->ucontent.raw;
    int base = get_inst_field(ainst, InstField_H)? 16 : 0;
    strcat(output, "(");
    int first_reg = 1, i;
    for (i = 0; i < 16; i++) {
        if (reg_bitmap & (1 << i)) {
            if (first_reg)
                first_reg = 0;
            else
                strcat(output, ", ");
            strcat(output, get_register_name(base+i));
        }
    }
    strcat(output, ")");
}

#define NO_ARG { NULL, NULL, 0, 0, NULL, NULL}

static void print_inst_arith(inst *ainst, struct disassemble_info *info) {
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    func(stream, "%s", ainst->type->prefix);
    if (get_inst_field(ainst, InstField_S))
        func(stream, ".a");
}

/**
 * Disassemble ld/st instruction's suffix
 **/
static void print_inst_ldst_suffix(inst *ainst, struct disassemble_info *info)
{
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    unsigned long is_write = get_inst_field(ainst, InstField_W);
    unsigned long is_post = get_inst_field(ainst, InstField_P);
    if (is_post) {
        func(stream, "%s",
             is_write ? ".w":"");
    } else {
        func(stream, "%s",
             is_write ? ".u":".w");
    }
}

/**
 * Disassemble ldw/ldb/stw/stb with .u, .w or nothing
 **/
static void print_inst_ldst(inst *ainst, struct disassemble_info *info) {
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    unsigned long is_load = get_inst_field(ainst, InstField_L);
    unsigned long is_byte = get_inst_field(ainst, InstField_B);
    func(stream, "%s%c",
         is_load ? "ld":"st",
         is_byte ? 'b':'w');
    print_inst_ldst_suffix(ainst, info);
}

/**
 * Disassemble stm/ldm with suffix
 **/
static void print_inst_ldst_multi(inst *ainst, struct disassemble_info *info)
{
    PTR stream = info->stream;
    fprintf_ftype func = info->fprintf_func;
    func(stream, "%s", ainst->type->prefix);
    int is_write = get_inst_field(ainst, InstField_W);
    func(stream, "%s", is_write? ".w" : "");
}

/* Remember to add a similar #define here if you
 * add a new argfrom_str_xxx in tc-unicore32.c
 * and use it in inst_types[].
 * Check the head of this file to see why */
#ifndef TC_UNICORE32_H
#define assemble_inst_default NULL
#define assemble_inst_arith NULL
#define assemble_inst_ldst NULL
#define assemble_inst_ldst_multi NULL
#define argfrom_str_r NULL
#define argfrom_str_shiftuimm NULL
#define argfrom_str_shiftr NULL
#define argfrom_str_uimm NULL
#define argfrom_str_rbase NULL
#define argfrom_str_rmulti NULL
#define argfrom_str_ldst_symbol NULL
#endif

/* Macro for binary literal support */
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0)          \
    +((x&0x000000F0LU)?2:0)                     \
    +((x&0x00000F00LU)?4:0)                     \
    +((x&0x0000F000LU)?8:0)                     \
    +((x&0x000F0000LU)?16:0)                    \
    +((x&0x00F00000LU)?32:0)                    \
    +((x&0x0F000000LU)?64:0)                    \
    +((x&0xF0000000LU)?128:0)

#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8)   \
                        + B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24)   \
                                + ((unsigned long)B8(db2)<<16)  \
                                + ((unsigned long)B8(db3)<<8)   \
                                + B8(dlsb))

#define SingleSField(field) init_singlefield, set_field_singlesfield, field, 0
#define SingleUField(field) init_singlefield, set_field_singleufield, field, 0
#define CombineUField(field1, field2)                   \
    init_combinefield, set_field_combineufield, field1, field2
#define ARG_T(INIT, PRINT) {INIT, PRINT}
#define ARG_SingleSField(field, PRINT, AS) {SingleSField(field), PRINT, AS}
#define ARG_SingleUField(field, PRINT, AS) {SingleUField(field), PRINT, AS}
#define ARG_CombineUField(field1, field2, PRINT, AS)    \
    {CombineUField(field1, field2), PRINT, AS}


static const inst_type inst_types[] = {

// for D_Imm_Shift and D_Reg_Shift
#define ARITH_INS(prefix, opcodes)                                  \
    { prefix,                                                       \
      prefix"shiftimm",                                             \
      /**/B32(11111110, 00000000, 00000001, 00100000),              \
      B32(00##opcodes##0, 00000000, 00000000, 00000000),              \
      print_inst_arith,                                             \
      assemble_inst_arith,                                          \
      {ARG_SingleUField(InstField_Rd, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_Rn, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_Rm, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_ShiftImm, print_arg_shiftimm,     \
                        argfrom_str_shiftuimm)                           \
      }                                                             \
    },                                                              \
    { prefix,                                                       \
      prefix"shiftr",                                               \
      /**/B32(11111110, 00000000, 00000001, 00100000),              \
      B32(00##opcodes##0, 00000000, 00000000, 00100000),              \
      print_inst_arith,                                             \
      assemble_inst_arith,                                          \
      {ARG_SingleUField(InstField_Rd, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_Rn, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_Rm, print_arg_r, argfrom_str_r),  \
       ARG_SingleUField(InstField_Rs, print_arg_shiftr,             \
                        argfrom_str_shiftr),                             \
      }                                                             \
    }

    ARITH_INS("and", 00000),
    ARITH_INS("xor", 00001),
    ARITH_INS("sub", 00010),
    ARITH_INS("rsub", 00011),
    ARITH_INS("add", 00100),
    ARITH_INS("addc", 00101),
    ARITH_INS("subc", 00110),
    ARITH_INS("rsubc", 00111),
    ARITH_INS("undefined", 01000),
    ARITH_INS("undefined", 01001),
    ARITH_INS("undefined", 01010),
    ARITH_INS("undefined", 01011),
    ARITH_INS("or", 01100),
    ARITH_INS("mov", 01101),
    ARITH_INS("andn", 01110),
    ARITH_INS("not", 01111),
    ARITH_INS("and", 10000),
    ARITH_INS("xor", 10001),
    ARITH_INS("sub", 10010),
    ARITH_INS("rsub", 10011),
    ARITH_INS("add", 10100),
    ARITH_INS("addc", 10101),
    ARITH_INS("subc", 10110),
    ARITH_INS("rsubc", 10111),
    ARITH_INS("undefined", 11000),
    ARITH_INS("undefined", 11001),
    ARITH_INS("undefined", 11010),
    ARITH_INS("undefined", 11011),
    ARITH_INS("or", 11100),
    ARITH_INS("mov", 11101),
    ARITH_INS("andn", 11110),
    ARITH_INS("not", 11111),

    {"", "ldst-imm14",
     B32(11100000, 00000000, 00000000, 00000000),
     B32(01100000, 00000000, 00000000, 00000000),
     print_inst_ldst,
     assemble_inst_ldst,
     {ARG_SingleUField(InstField_Rd, print_arg_r, argfrom_str_r),
      ARG_SingleUField(InstField_Rn, print_arg_rbase, argfrom_str_rbase),
      ARG_SingleUField(InstField_Imm14, print_arg_uimm, argfrom_str_uimm),
      NO_ARG
     }
    },

    {"ldw", "ldst-pc-imm14-symbol",
     B32(11111111, 00000000, 00000000, 00000000),
     B32(01111001, 00000000, 00000000, 00000000),
     NULL, // never use this for disassemble ldst-imm14 will do
     assemble_inst_default,
     {ARG_SingleUField(InstField_Rd, NULL, argfrom_str_r),
      ARG_SingleUField(InstField_Imm14, NULL, argfrom_str_ldst_symbol),
      NO_ARG,
      NO_ARG}
    },

    {"stm", "stm-st-multi-registers",
     B32(11100001, 00000000, 00000001, 10000000),
     B32(10000000, 00000000, 00000000, 00000000),
     print_inst_ldst_multi,
     assemble_inst_ldst_multi,
     {ARG_CombineUField(InstField_hRlist, InstField_lRlist,
                        print_arg_rmulti, argfrom_str_rmulti),
      ARG_SingleUField(InstField_Rn, print_arg_rbase, argfrom_str_rbase),
      NO_ARG, NO_ARG}
    },
};
static const long NUMINST = ARRAY_SIZE(inst_types);

#endif
