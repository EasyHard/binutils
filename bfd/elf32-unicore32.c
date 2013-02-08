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

#include "elf/unicore32.h"

/* unicore32_reloc_map array maps BFD relocation
   enum into a UniCore32 GAS relocation type.  */

struct unicore32_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_enum; /* BFD relocation enum.  */
  unsigned short unicore32_reloc_type;     /* UniCore32 relocation type. */
};

#define MAP_RELOC(name) {BFD_RELOC_UNICORE32_##name, R_UNICORE32_##name},

static const struct unicore32_reloc_map unicore32_reloc_map[R_UNICORE32_MAX] =
{
    {BFD_RELOC_NONE,           R_UNICORE32_NONE},
    MAP_RELOC(ABS32)
    MAP_RELOC(IMM14)
};

static reloc_howto_type unicore32_elf_howto_table[] =
{
  HOWTO (R_UNICORE32_NONE,         /* type */
         0,                        /* rightshift */
         4,                        /* size */
         32,                       /* bitsize */
         FALSE,                    /* pc_relative */
         0,                        /* bitpos */
         complain_overflow_dont,   /* complain_on_overflow */
         bfd_elf_generic_reloc,    /* special_function */
         "R_UNICORE32_NONE",       /* name */
         FALSE,                    /* partial_inplace */
         0,                        /* src_mask */
         0,                        /* dst_mask */
         FALSE),                   /* pcrel_offset */

  HOWTO (R_UNICORE32_ABS32,        /* type */
         0,                        /* rightshift */
         4,                        /* size */
         32,                        /* bitsize */
         FALSE,                    /* pc_relative */
         0,                        /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,    /* special_function */
         "R_UNICORE32_ABS32",      /* name */
         FALSE,                    /* partial_inplace */
         0x0,                      /* src_mask */
         0xffffffff,               /* dst_mask */
         FALSE),                   /* pcrel_offset */

  HOWTO (R_UNICORE32_IMM14,        /* type */
         0,                        /* rightshift */
         4,                        /* size */
         14,                        /* bitsize */
         TRUE,                    /* pc_relative */
         0,                        /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,    /* special_function */
         "R_UNICORE32_IMM14",      /* name */
         FALSE,                    /* partial_inplace */
         0x0,                      /* src_mask */
         0x3fff,                     /* dst_mask */
         FALSE),                   /* pcrel_offset */
};

/* Stub */
static reloc_howto_type *
elf_unicore32_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                            bfd_reloc_code_real_type code) {
  unsigned int i;

  for (i = 0; i < R_UNICORE32_MAX; i++)
    if (code == unicore32_reloc_map[i].bfd_reloc_enum)
      return &unicore32_elf_howto_table[unicore32_reloc_map[i].unicore32_reloc_type];

  _bfd_error_handler ("Unsupported UniCore32 relocation type: 0x%x\n", code);
  return NULL;
}

/* Stub */
static reloc_howto_type *
elf_unicore32_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                            const char *r_name)
{
  unsigned int i;

  for (i = 0; ARRAY_SIZE (unicore32_elf_howto_table); i++)
    if (unicore32_elf_howto_table[i].name != NULL
        && strcasecmp (unicore32_elf_howto_table[i].name, r_name) == 0)
      return unicore32_elf_howto_table + i;

  return NULL;
}

/* Retrieve a howto ptr using an internal relocation entry.  */
static void
elf_unicore32_info_to_howto (bfd *abfd ATTRIBUTE_UNUSED, arelent *cache_ptr,
                        Elf_Internal_Rela *dst)
{
  unsigned int r_type = ELF32_R_TYPE (dst->r_info);

  BFD_ASSERT (r_type < (unsigned int) R_UNICORE32_MAX);
  cache_ptr->howto = unicore32_elf_howto_table + r_type;
}

#define ELF_ARCH                       bfd_arch_unicore32
#define ELF_MACHINE_CODE               EM_UNICORE
#define ELF_MAXPAGESIZE                        0x1000

#define TARGET_LITTLE_SYM              bfd_elf32_unicore32_vec
#define TARGET_LITTLE_NAME             "elf32-unicore32"

#define bfd_elf32_bfd_reloc_type_lookup   elf_unicore32_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup   elf_unicore32_reloc_name_lookup

#define elf_info_to_howto                 elf_unicore32_info_to_howto
#define elf_info_to_howto_rel             0

#include "elf32-target.h"
