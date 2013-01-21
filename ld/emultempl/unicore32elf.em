# This shell script emits a C file. -*- C -*-
# Copyright 2013 Free Software Foundation, Inc.
# Contributed by LIU Zhiyou <liuzhiyou.cs@gmail.com>
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.
#

# This file is sourced from elf32.em, and defines extra cr16-elf
# specific routines.
#
fragment <<EOF

#include "ldctor.h"

/* This function is run after all the input files have been opened.  */

static void
unicore32_elf_after_open (void)
{
  /* Call the standard elf routine.  */
  gld${EMULATION_NAME}_after_open ();
}

static void
unicore32elf_after_parse (void)
{
  after_parse_default ();
}

/* This is called after the sections have been attached to output
   sections, but before any sizes or addresses have been set.  */

static void
unicore32elf_before_allocation (void)
{
  /* Call the default first.  */
  gld${EMULATION_NAME}_before_allocation ();
}

EOF

# Put these extra unicore32-elf routines in ld_${EMULATION_NAME}_emulation
#
LDEMUL_AFTER_OPEN=unicore32_elf_after_open
LDEMUL_AFTER_PARSE=unicore32elf_after_parse
LDEMUL_BEFORE_ALLOCATION=unicore32elf_before_allocation
