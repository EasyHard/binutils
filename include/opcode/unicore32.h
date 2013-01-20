/* unicore32.h -- Header file for UniCore32 opcode and register tables.
   Copyright 2013 Free Software Foundation, Inc.
   Contributed by LIU Zhiyou

   This file is part of GAS, GDB and the GNU binutils.

   GAS, GDB, and GNU binutils is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GAS, GDB, and GNU binutils are distributed in the hope that they will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _UNICORE32_H_
#define _UNICORE32_H_

struct disassemble_info;
/* enumeration of all the registers in Unicore32 */
typedef enum
{
    /* all general purpose registers in UniCore32 */
    r0, r1, r2, r3,
    r4, r5, r6, r7,
    r8, r9, r10, r11,
    r12, r13, r14, r15,
    r16, r17, r18, r19,
    r20, r21, r22, r23,
    r24, r25, r26, r27,
    r28, r29, r30, r31,
    MAX_GPREG,
    /* UniCore32 processor registers and special registers : */
    apreg, /* just a stub */
    MAX_PREG,
    /* Unicore32 coprocessor registers */
    acoreg, /* just a stub */
    MAX_COREG,
    /* Not a register.  */
    nullregister,
    MAX_REG
} regs;

/* util macros of enum reg */
#define first_gpreg r0
#define first_preg apreg
#define first_coreg acoreg
#define is_gpreg(x) ((x) < MAX_GPREG)
#define is_preg(x) ((x) >= first_preg && (x) < MAX_PREG)
#define is_coreg(x) ((x) >= first_coreg && (x) < MAX_COREG)

/***
 * Here we define fields which are used in UniCore32's
 * instructions. And I used -XMacro to make three things
 * (i.e. name, width and offset) are defined in the same
 * place.
 * `name' implies the field's name, `width' declare how
 * many bits the field contains. and `offset' is the
 * lowest bit (indexing begins with 0)belongs to the field.
 * If there is any question about `width' and `offset',
 * you can check the function `get_inst_field' in
 * opcodes/unicore32-opc.c
 ***/
#define InstField_DEF                           \
    InstF(InstField_Opcodes, 4, 25)             \
    InstF(InstField_S, 1, 24)                   \
    InstF(InstField_Rn, 5, 19)                  \
    InstF(InstField_Rd, 5, 14)                  \
    InstF(InstField_ShiftImm, 5, 9)             \
    InstF(InstField_Shift, 2, 6)                \
    InstF(InstField_Rm, 5, 0)                   \
    InstF(InstField_Rs, 5, 9)                   \
    InstF(InstField_N, 1, 26)                   \
    InstF(InstField_Cond, 4, 19)                \
    InstF(InstField_R, 1, 26)                   \
    InstF(InstField_A, 1, 25)                   \
    InstF(InstField_U, 1, 26)                   \
    InstF(InstField_RdLo, 5,14)                 \
    InstF(InstField_RdHi, 5, 9)                 \
    InstF(InstField_Z, 1, 26)                   \
    InstF(InstField_Rotate, 5, 9)               \
    InstF(InstField_Imm9, 9, 0)                 \
    InstF(InstField_B, 1, 26)                   \
    InstF(InstField_P, 1, 28)                   \
    InstF(InstField_W, 1, 25)                   \
    InstF(InstField_L, 1, 24)                   \
    InstF(InstField_Imm14, 14, 0)               \
    InstF(InstField_hRlist, 10, 9)              \
    InstF(InstField_lRlist, 6, 0)               \
    InstF(InstField_H, 1, 6)                    \
    InstF(InstField_hOff10, 5, 9)               \
    InstF(InstField_lOff10, 5, 0)               \
    InstF(InstField_hX, 20, 9)                  \
    InstF(InstField_lX, 0, 8)                   \
    InstF(InstField_sOff24, 24, 0)              \
    InstF(InstField_CRd, 5, 14)                 \
    InstF(InstField_Cp_num, 5, 9)               \
    InstF(InstField_Off10, 10, 0)               \
    InstF(InstField_8Rlist, 8, 0)               \
    InstF(InstField_DPOp1, 4, 24)               \
    InstF(InstField_CRn, 5, 19)                 \
    InstF(InstField_Op2, 4, 6)                  \
    InstF(InstField_CRm, 5, 0)                  \
    InstF(InstField_RTOp1, 3, 25)               \
    InstF(InstField_STnum, 24, 0)

typedef enum {
#define InstF(name, width, offset) name,
InstField_DEF
#undef InstF
} inst_fields;

extern const short inst_width[];
extern const short inst_offset[];
extern const long inst_mask[];
extern const char* (inst_field_str[]);

struct _inst;
typedef struct _inst inst;
struct _inst_type;
typedef struct _inst_type inst_type;
struct _argument;
typedef struct _argument argument;
struct _argument_type;
typedef struct _argument_type argument_type;

/**
 * Now we start to define the structural representation
 * of UniCore32 instructions.
 * All UniCore32 instructions are 32bit-fixed-sized and
 * have no more than 4 operands. Operands are called
 * arguments in the code.
 *
 * As you can image, different instructions have different
 * behavior, different operands and should be assemble
 * and disassemble in different way.
 * Thus, I use `inst' storing datas that every instruction
 * shares and defining different operands' types and
 * behavior by `argument_type args[];' functions.
 * And the same situation occurs for operands. So I did
 * the same thing.
 *
 * To get an idea about to assemble and disassemble an
 * instruction, read out the following structures and
 * comments. If you want to trace into details,
 * check `print_insn_unicore32' in opcodes/unicore32-dis.c
 * and `md_assemble' in gas/config/tc-unicore32.c
 **/

struct _argument{
    union {
        regs areg;
        unsigned long uimm;
        unsigned long raw;
    } ucontent;
    long scontent;
    const argument_type* type;
};

struct _argument_type{
    /**
     * For disassemble. `from_inst' will set correct value
     * for `struct argument' based on content of `inst *'.
     **/
    void (*from_inst)(inst *, argument*);
    /**
     * For assemble. after `from_str' correctly set value
     * of `struct argument' based on assembling string,
     * we can call this function to set field of `inst *'
     * to right content
     **/
    void (*set_field)(inst *, argument *);
    /* field1 and field2 are the above functions' argument
     * to sepcify which fields such arguments related to */
    inst_fields field1, field2;

    /* For disassemble. Define how to print such arguments */
    void (*print_arg)(inst *, argument *, struct disassemble_info *, char *);

    /* For assemble. Define how to construct 'struct argument'
     * based on assemblin string. Sould return 0 if the
     * assembling string doesn't match this type */
    int (*from_str)(inst *, argument*, int index, char **);
};

#define MAX_ARG 4
struct _inst_type {
    /* For assemble, a instruction *may* belongs to a type
     * only if it's starting with `prefix' */
    const char *prefix;

    /* For debugging, to adress which type it is. */
    const char *name;

    /***
     * For disassemble.
     * (inst.raw & mask) == expect should be true
     * *if and only if* inst is belong to such inst_type;
     ***/
    unsigned long mask;
    unsigned long expect;

    /* For disassemble, printing the instruction */
    void (*print)(inst *, struct disassemble_info *);

    /* For assemble. Return 0 means the instruction
     * storing in `char **' do not match this type */
    int (*assemble)(inst *, const inst_type*, char **);

    /* Types of arguments of such type of instuction */
    const argument_type args[MAX_ARG];
};

struct _inst{
    unsigned long raw; /* Binary representation of instruction */
    const inst_type  *type;
    argument args[MAX_ARG];
};

#define PRINT_BUFFER_SIZE 256
#endif
