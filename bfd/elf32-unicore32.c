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


/* Stub */
static reloc_howto_type *
elf_unicore32_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                            bfd_reloc_code_real_type code ATTRIBUTE_UNUSED) {
    return NULL;
}

/* Stub */
static reloc_howto_type *
elf_unicore32_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                            const char *r_name ATTRIBUTE_UNUSED)
{
    return NULL;
}

#define ELF_ARCH                       bfd_arch_unicore32
#define ELF_MACHINE_CODE               EM_UNICORE
#define ELF_MAXPAGESIZE                        0x1000

#define TARGET_LITTLE_SYM              bfd_elf32_unicore32_vec
#define TARGET_LITTLE_NAME             "elf32-unicore32"

#define bfd_elf32_bfd_reloc_type_lookup   elf_unicore32_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup   elf_unicore32_reloc_name_lookup

#include "elf32-target.h"
