/* Assemble and disassemble functions for UniCore32 processor
   Copyright 2013 Free Software Foundation, Inc.
   Contributed by LIU Zhiyou (liuzhiyou.cs@gmail.com).

   This file is part of GAS, GDB and the GNU binutils.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "dis-asm.h"
#include "opcode/unicore32.h"
#include "libiberty.h"

#define ILLEGAL  "illegal"

/* using XMacro to define utils array of instruction fields */
const short inst_width[] = {
#define InstF(name, width, offset)    width,
InstField_DEF
#undef InstF
};

const short inst_offset[] = {
#define InstF(name, width, offset)    offset,
InstField_DEF
#undef InstF
};

const long inst_mask[] =     {
#define InstF(name, width, offset)    ((1 << width) - 1)  << offset,
InstField_DEF
#undef InstF
};

const char *inst_field_str[] = {
#define InstF(name, width, offset) #name ,
InstField_DEF
#undef InstF
};

#include "opcode/unicore32-opc.h"

/* Retrieve a single word from a given memory address.  */
static unsigned long
get_word_at (bfd_vma memaddr, struct disassemble_info *info)
{
    bfd_byte buffer[4];
    int status;
    unsigned long insn = 0;

    status = info->read_memory_func (memaddr, buffer, 4, info);

    if (status == 0)
        insn = (unsigned long) bfd_getl32 (buffer);

    return insn;
}

int
print_insn_unicore32 (bfd_vma memaddr, struct disassemble_info *info)
{
    info->display_endian = BFD_ENDIAN_LITTLE;
    info->bytes_per_chunk = 4;
    unsigned long raw = get_word_at(memaddr, info);
    int i = 0, j;
    for (i = 0; i < NUMINST; i++)
        if ((inst_types[i].mask & raw) == inst_types[i].expect) {
            /* Initialize */
            inst ainst;
            ainst.raw = raw;
            ainst.type = inst_types+i;
            for (j = 0; j < MAX_ARG && ainst.type->args[j].from_inst; j++) {
                ainst.args[j].type = ainst.type->args+j;
                ainst.type->args[j].from_inst(&ainst, ainst.args+j);
            }
            /* Disassembler */
            ainst.type->print(&ainst, info);
            char args_output[MAX_ARG][PRINT_BUFFER_SIZE];
            for (j = 0; j < MAX_ARG && ainst.type->args[j].from_inst; j++)
                ainst.type->args[j].print_arg(&ainst, &ainst.args[j],
                                              info, (char*)(args_output+j));
            for (j = 0; j < MAX_ARG && ainst.type->args[j].from_inst; j++) {
                if (strlen(args_output[j]) == 0)
                    continue;
                /* first one and shift arg (4th) do not need comma */
                if (j < 3 && j > 0) info->fprintf_func(info->stream, ",");
                info->fprintf_func(info->stream, " ");
                info->fprintf_func(info->stream, "%s", args_output[j]);
            }
            return 4;
        }

    /* No match found.  */
    info->fprintf_func (info->stream,"%s ",ILLEGAL);
    return 4;
}
