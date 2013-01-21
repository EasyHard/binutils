# Linker Script for MPRC's UNICORE32-ELF32.

# The next line should be uncommented if it is desired to link
# without libstart.o and directly enter main.

# ENTRY=_main

test -z "$ENTRY" && ENTRY=_start
cat <<EOF

/* Example Linker Script for linking NS CR16 elf32 files. */

EOF
