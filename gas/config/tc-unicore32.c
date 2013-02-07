/* tc-unicore32.c -- Assembler code for the UniCore32 CPU core.
   Copyright 2013 Free Software Foundation, Inc.

   Contributed by LIU Zhiyou <liuzhiyou.cs@gmail.com>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "dwarf2dbg.h"
#include "opcode/unicore32.h"

/* Word is considered here as a 16-bit unsigned short int.  */
#define WORD_SHIFT  32

/* Register is 2-byte size.  */
#define REG_SIZE   4

/* Maximum size of a single instruction (in words).  */
#define INSN_MAX_SIZE   1

/* Maximum bits which may be set in a `mask16' operand.  */
#define MAX_REGS_IN_MASK16  16

const char comment_chars[]        = ";";
const char line_comment_chars[]   = ";";
const char line_separator_chars[] = ";";
/* Don't know what they are yet */
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "fF";
#define OPENRISC_SHORTOPTS "m:"
const char * md_shortopts = OPENRISC_SHORTOPTS;
struct option md_longopts[] =
{
};
size_t md_longopts_size = sizeof (md_longopts);
const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

static unsigned long get_inst_field(inst const *ainst, inst_fields field);
static void set_inst_field(inst *ainst, inst_fields field, unsigned long value);
static long u2s(unsigned long us, short width);

static int endwith(const char* str, const char* end) {
    const char *strtail = str + strlen(str);
    const char *endtail = end + strlen(end);
    while (endtail >= end && strtail >= str && *endtail == *strtail) {
        endtail--;
        strtail--;
    }
    if (endtail < end)
        return 1;
    else
        return 0;
}

static int handle_arguments(inst* ainst,
                            const inst_type* this,
                            char **ops) {
    int i = 0;
    for (i = 0; i < MAX_ARG && this->args[i].from_str; i++) {
        if (!this->args[i].from_str(ainst,
                                    ainst->args+i,
                                    i,
                                    ops))
            return 0;
        else {
            ainst->args[i].type = this->args+i;
            this->args[i].set_field(ainst, ainst->args+i);
        }
    }
    return 1;
}

static int assemble_inst_arith(inst *ainst,const inst_type* this, char **ops) {
    ainst->raw = 0;
    if (!handle_arguments(ainst, this, ops))
        return 0;
    ainst->raw |= this->expect;
    if (endwith(ops[0], ".a"))
        ainst->raw |= inst_mask[InstField_S];
    return 1;
}

static int assemble_inst_ldst(inst *ainst,
                              const inst_type* this ATTRIBUTE_UNUSED,
                              char **ops) {
    ainst->raw = 0;
    if (!handle_arguments(ainst, this, ops))
        return 0;
    ainst->raw |= this->expect;
    unsigned long is_post = get_inst_field(ainst, InstField_P);
    unsigned long w_map[][2] = {
        {1, 0}, // !is_post
        {0, 1}  // is_post
    };
#define HANDLE_FIELD_W(prefix)                      \
    if (strstr(ops[0], prefix".w"))                 \
        set_inst_field(ainst, InstField_W, w_map[is_post][1]);  \
    else                                            \
        set_inst_field(ainst, InstField_W, w_map[is_post][0]);  \

    char *found;
    if ((found = strstr(ops[0], "ldb"))) {
        set_inst_field(ainst, InstField_B, 1);
        set_inst_field(ainst, InstField_L, 1);
        HANDLE_FIELD_W("ldb");
    } else if ((found = strstr(ops[0], "ldw"))) {
        set_inst_field(ainst, InstField_B, 0);
        set_inst_field(ainst, InstField_L, 1);
        HANDLE_FIELD_W("ldw");
    } else if ((found = strstr(ops[0], "stb"))) {
        set_inst_field(ainst, InstField_B, 1);
        set_inst_field(ainst, InstField_L, 0);
        HANDLE_FIELD_W("stb");
    } else if ((found = strstr(ops[0], "stw"))) {
        set_inst_field(ainst, InstField_B, 0);
        set_inst_field(ainst, InstField_L, 0);
        HANDLE_FIELD_W("stw");
    } else {
        return 0;
    }
    return 1;
}

static int argfrom_str_r(inst* ainst ATTRIBUTE_UNUSED,
                         argument* arg,
                         int index,
                         char** ops) {
    unsigned int reg_idx;
    if (sscanf(ops[index+1], "r%u", &reg_idx) != 1) {
        return 0;
    } else {
        arg->ucontent.areg = first_gpreg + reg_idx;
        if (is_gpreg(arg->ucontent.areg))
            return 1;
        else return 0;
    }
}

static int argfrom_str_shiftuimm(inst* ainst,
                                 argument* arg,
                                 int index,
                                 char* ops[]) {
    unsigned long uimm;
    char *found = NULL;
    char const * shift_map[] = {"<<", ">>", "|>", "<>"};
    unsigned long shift = 0;
    if (!ops[index+1] || strlen(ops[index+1]) == 0) {
        uimm = 0;
    } else {
        for (shift = 0; shift < 4; shift++)
            if ((found = strstr(ops[index+1], shift_map[shift]))) {
                break;
            }
        if (found == NULL)
            return 0;
        int result = sscanf(found+2, "#%lu", &uimm);
        if (result != 1) {
            return 0;
        }
    }
    /* when we reach here, shift should be meaningful */
    set_inst_field(ainst, InstField_Shift, shift);
    /* handling <> with #33 */
    if (shift == 3)  {
        if (uimm == 0)
            return 0;
        if (uimm == 33)
            uimm = 0;
    }
    if (shift == 2 || shift == 1) {
        if (uimm == 0)
            return 0;
        if (uimm == 32)
            uimm = 0;
    }
    arg->ucontent.uimm = uimm;
    return 1;
}



static int argfrom_str_shiftr(inst* ainst,
                              argument* arg,
                              int index,
                              char** ops) {
    char const * shift_map[] = {"<<", ">>", "|>", "<>"};
    unsigned long shift; char * found = NULL;
    for (shift = 0; shift < 4; shift++)
        if ((found = strstr(ops[index+1], shift_map[shift]))) {
            set_inst_field(ainst, InstField_Shift, shift);
            break;
        }
    if (found == NULL)
        return 0;
    unsigned int reg_idx;
    if (sscanf(found+2, "r%u", &reg_idx) != 1) {
        return 0;
    } else {
        arg->ucontent.areg = first_gpreg + reg_idx;
        if (is_gpreg(arg->ucontent.areg))
            return 1;
        else return 0;
    }
}

static int argfrom_str_uimm(inst* ainst ATTRIBUTE_UNUSED,
                            argument* arg,
                            int index,
                            char** ops) {
    unsigned long uimm;
    if (strlen(ops[index+1]) == 0)
        uimm = 0;
    else if (sscanf(ops[index+1], "#%lu", &uimm) != 1)
        return 0;
    arg->ucontent.uimm = uimm;
    return 1;
}

static int argfrom_str_rbase(inst* ainst,
                              argument* arg,
                              int index,
                              char** ops) {
    unsigned long reg_idx;
    if (sscanf(ops[index+1], "[r%lu", &reg_idx) != 1)
        return 0;
    if (!is_gpreg(reg_idx))
        return 0;
    if (endwith(ops[index+1], "+]")) {
        set_inst_field(ainst, InstField_U, 1);
        set_inst_field(ainst, InstField_P, 1);
    } else if (endwith(ops[index+1], "-]")) {
        set_inst_field(ainst, InstField_U, 0);
        set_inst_field(ainst, InstField_P, 1);
    } else if (endwith(ops[index+1], "]+")) {
        set_inst_field(ainst, InstField_U, 1);
        set_inst_field(ainst, InstField_P, 0);
    } else if (endwith(ops[index+1], "]-")) {
        set_inst_field(ainst, InstField_U, 0);
        set_inst_field(ainst, InstField_P, 0);
    } else {
        return 0;
    }
    arg->ucontent.areg = reg_idx;
    return 1;
}
#include "opcode/unicore32-opc.h"

int
md_parse_option (int c ATTRIBUTE_UNUSED, char * arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE * stream ATTRIBUTE_UNUSED)
{
}

void
md_begin (void)
{
}

static int outplace_split_params(char *result[], char *str) {
    int count = 0;
    char* curr = strtok(str, " ");
    while (curr) {
        strcpy(result[count], curr);
        count++;
        curr = strtok(NULL, ",");
    }
    // handle the last argument for
    // <<, >>, |>, <>
    const char* shifts[] = {"<<", ">>", "|>", "<>"};
    int i;char* found = NULL, *tmp;
    for (i = 0; i < 4; i++)
        if ((tmp = strstr(result[count-1], shifts[i])))
            found = tmp;
    if (found) {
        strcpy(result[count], found);
        *found = '\0';
        count++;
    }
    return count;
}

static int start_with(const char* str, const char* start) {
    while (*str && *start && *str == *start) {
        str++;start++;
    }
    return !(*start);
}

void
md_assemble (char * str)
{
    char* ops[8];
    char storage[8][256];
    char old[256];
    strncpy(old, str, 256);
    memset(ops, 0, sizeof ops);
    memset(storage, 0, sizeof storage);
    int i;
    for (i = 0; i < 8; i++)
        ops[i] = (char*)(&storage[i]);
    outplace_split_params(ops, str);
    inst ainst;
    for (i = 0; i < NUMINST; i++) {
        if (start_with(ops[0], inst_types[i].prefix) &&
            inst_types[i].assemble(&ainst, inst_types+i, ops))
        {
            dwarf2_emit_insn (0);
            char *this_frag = frag_more(4);
            md_number_to_chars (this_frag, (valueT) ainst.raw, 4);
            return ;
        }
    }
    as_bad("unknown instruction: %s", old);
}

void
md_operand (expressionS * expressionP)
{
    expression (expressionP);
}

valueT
md_section_align (segT segment, valueT size)
{
  int align = bfd_get_section_alignment (stdoutput, segment);
  return ((size + (1 << align) - 1) & (-1 << align));
}

symbolS *
md_undefined_symbol (char * name ATTRIBUTE_UNUSED)
{
  return 0;
}

int
md_estimate_size_before_relax (fragS * fragP ATTRIBUTE_UNUSED,
                               segT segment ATTRIBUTE_UNUSED)
{
    return 0;
}

void
md_convert_frag (bfd *   abfd ATTRIBUTE_UNUSED,
		 segT    sec  ATTRIBUTE_UNUSED,
		 fragS * fragP ATTRIBUTE_UNUSED)
{
    return ;
}

char *
md_atof (int type, char * litP, int *  sizeP)
{
  return ieee_md_atof (type, litP, sizeP, TRUE);
}

arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED,
              fixS * fixP ATTRIBUTE_UNUSED)
{
    return NULL;
}

long
md_pcrel_from (fixS *fixp)
{
  return fixp->fx_frag->fr_address + fixp->fx_where;
}

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED,
              valueT *valP ATTRIBUTE_UNUSED,
              segT seg ATTRIBUTE_UNUSED)
{
}
