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

const char comment_chars[]        = "#";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = ";";
/* Don't know what they are yet */
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "dD";
#define OPENRISC_SHORTOPTS "m:"
const char * md_shortopts = OPENRISC_SHORTOPTS;
struct option md_longopts[] =
{
};
size_t md_longopts_size = sizeof (md_longopts);
const pseudo_typeS md_pseudo_table[] =
{
};
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

void
md_assemble (char * str)
{
    as_bad("%s", str);
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
