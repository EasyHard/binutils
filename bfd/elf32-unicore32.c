/*
 * 32-bit ELF support for UniCore32
 * Contributed by Guan Xuetao <gxt@mprc.pku.edu.cn>
 * This file is part of BFD, the Binary File Descriptor library.
 */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "libiberty.h"


#define ELF_ARCH                       bfd_arch_unicore32
#define ELF_MACHINE_CODE               EM_UNICORE32
#define ELF_MAXPAGESIZE                        0x1000

#define TARGET_LITTLE_SYM              bfd_elf32_unicore32_vec
#define TARGET_LITTLE_NAME             "elf32-unicore32"

#include "elf32-target.h"
