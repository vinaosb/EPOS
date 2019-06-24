// EPOS ARMv7 Program Starter

        .file "armv7_crt0.S"

        // Entry point called by SETUP for processors
        .section .text
        .align  2
        .global _start
        .type   _start, function
_start:
        // Temporary stack(s) for INIT are created and configure by SETUP

        // Call _init
        bl      _init

        // Call main
        .align  2
        .globl  __epos_app_entry
        .type   __epos_app_entry, function
__epos_app_entry:
        bl      __pre_main
        bl      main
        push    {r0}         // save main's return value to be used by exit()
        bl      _fini
        bl      _exit