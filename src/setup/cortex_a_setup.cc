// EPOS Cortex-A SETUP

#include <system/config.h>
#include <machine/cortex_a/scu.h>

extern "C" { void _vector_table() __attribute__ ((used, naked, section(".init"))); }

// Interrupt Vector Table
void _vector_table()
{
    ASM("\t\n\
    _vector_table:                                                           \t\n\
        b   _reset                                                           \t\n\
        b   _undefined_instruction                                           \t\n\
        b   _software_interrupt                                              \t\n\
        b   _prefetch_abort                                                  \t\n\
        b   _data_abort                                                      \t\n\
        nop                           // Reserved                            \t\n\
        b   _int_entry                // _irq                                \t\n\
        b   _fiq                                                             \t\n\
        ");



    __asm("_reset:");

  //create stack for all CPUs
    __asm("mrc p15, 0, r2, c0, c0, 5");// read CPU ID register into R2
    __asm("ands r2, r2, #0x03");// mask in only CPUID
     // set SVC stack
    __asm("mov r2, r2, LSL #14");
    __asm("ldr r0, =__boot_stack__ ");
    __asm("sub r0, r0, r2");
    __asm("mov sp, r0");

    ASM(
    // 1.MMU, L1$ disable
    //-----------------------------------------------
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Read System Control Register (SCTLR)
    "\n     BIC r1, r1, #1"             // mmu off
    "\n     BIC r1, r1, #(1 << 12)"     // i-cache off
    "\n     BIC r1, r1, #(1 << 2)"      // d-cache & L2-$ off
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Write System Control Register (SCTLR)
    //-----------------------------------------------
    // 2. invalidate: L1$, TLB, branch predictor
    //-----------------------------------------------
    "\n     MOV r0, #0"
    "\n     MCR p15, 0, r0, c7, c5, 0"  // Invalidate Instruction Cache
    "\n     MCR p15, 0, r0, c7, c5, 6"  // Invalidate branch prediction array
    "\n     MCR p15, 0, r0, c8, c7, 0"  // Invalidate entire Unified Main TLB
    "\n     ISB"                        // instr sync barrier
    //-----------------------------------------------
    // 2.a. Enable I cache + branch prediction
    //-----------------------------------------------
    "\n     MRC p15, 0, r0, c1, c0, 0"  // System control register
    "\n     ORR r0, r0, #1 << 12"       // Instruction cache enable
    "\n     ORR r0, r0, #1 << 11"       // Program flow prediction
    "\n     MCR p15, 0, r0, c1, c0, 0"  // System control register
    //-----------------------------------------------

    "\n     MRC p15, 1, R0, c0, c0, 1"  // Read CLIDR into R0
    "\n     ANDS R3, R0, #0x07000000"
    "\n     MOV R3, R3, LSR #23"        // Cache level value (naturally aligned)
    "\n     BEQ Finished"
    "\n     MOV R10, #0"
    "\n Loop1:"
    "\n     ADD R2, R10, R10, LSR #1"   // Work out 3 x cachelevel
    "\n     MOV R1, R0, LSR R2"         // bottom 3 bits are the Cache type for this level
    "\n     AND R1, R1, #7"             // get those 3 bits alone
    "\n     CMP R1, #2"
    "\n     BLT Skip"                   // no cache or only instruction cache at this
    "\n     MCR p15, 2, R10, c0, c0, 0" // write CSSELR from R10
    "\n     ISB"                        // ISB to sync the change to the CCSIDR
    "\n     MRC p15, 1, R1, c0, c0, 0"  // read current CCSIDR to R1
    "\n     AND R2, R1, #7"             // extract the line length field
    "\n     ADD R2, R2, #4"             // add 4 for the line length offset (log2 16 bytes)
    "\n     LDR R4, =0x3FF"
    "\n     ANDS R4, R4, R1, LSR #3"    // R4 is the max number on the way size (right aligned)
    "\n     CLZ R5, R4"                 // R5 is the bit position of the way size increment
    "\n     MOV R9, R4"                 // R9 working copy of the max way size (right aligned)
    "\n Loop2:"
    "\n     LDR R7, =0x00007FFF"
    "\n     ANDS R7, R7, R1, LSR #13"   // R7 is the max number of the index size (right aligned)
    "\n Loop3:"
    "\n     ORR R11, R10, R9, LSL R5"   // factor in the way number and cache number into R11
    "\n     ORR R11, R11, R7, LSL R2"   // factor in the index number
    "\n     MCR p15, 0, R11, c7, c10, 2"// DCCSW, clean by set/way
    "\n     SUBS R7, R7, #1"            // decrement the index
    "\n     BGE Loop3"
    "\n     SUBS R9, R9, #1"            // decrement the way number
    "\n     BGE Loop2"

    "\n Skip:"
    "\n     ADD R10, R10, #2"           // increment the cache number
    "\n     CMP R3, R10"
    "\n     BGT Loop1"
    "\n     DSB"
    "\n Finished:"
    "\n     ADD R4, #0"
    "\n     ADD R4, R4, #10"
    "\n");

    ASM(
    // Disable MMU
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Atribui-se ao R1 o valor do registrador 1 do
                                        // coprocessor 15
    "\n     BIC r1, r1, #0x1"           // Atribui-se ao bit 0 em R1 o valor 0, para
                                        // desligar a MMU
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Escreve-se no reg 1 do coprocessor 15
                                        // o que ha em R1, desabilitando a MMU

    // Disable L1 Caches
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Read Control Register configuration data
    "\n     BIC r1, r1, #(0x1 << 12)"   // Disable I Cache
    "\n     BIC r1, r1, #(0x1 << 2)"    // Disable D Cache
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Write Control Register configuration data

    // Invalidate L1 Caches
    // Invalidate Instruction cache
    "\n     MOV r1, #0"
    "\n     MCR p15, 0, r1, c7, c5, 0"

    // Invalidate Data cache
    // to make the code general purpose, we calculate the
    // cache size first and loop through each set + way
    "\n     MRC p15, 1, r0, c0, c0, 0"  // Read Cache Size ID
    "\n     LDR r3, =#0x1ff"
    "\n     AND r0, r3, r0, LSR #13"    // r0 = no. of sets - 1
    "\n     MOV r1, #0"                 // r1 = way counter way_loop
    "\n way_loop:"
    "\n     MOV r3, #0"                 // r3 = set counter set_loop
    "\n set_loop:"
    "\n     MOV r2, r1, LSL #30"
    "\n     ORR r2, r3, LSL #5"         // r2 = set/way cache operation format
    "\n     MCR p15, 0, r2, c7, c6, 2"  // Invalidate line described by r2
    "\n     ADD r3, r3, #1"             // Increment set counter
    "\n     CMP r0, r3"                 // Last set reached yet?
    "\n     BGT set_loop"               // if not, iterate set_loop
    "\n     ADD r1, r1, #1"             // else, next
    "\n     CMP r1, #4"                 // Last way reached yet?
    "\n     BNE way_loop"               // if not, iterate way_loop

    // Invalidate TLB
    "\n     MCR p15, 0, r1, c8, c7, 0"

    /* Branch Prediction Enable */  // Não tem no seminário

    /* Enable D-side Prefetch */    // Não tem no seminário

    // Aqui é criada uma L1 translation table na RAM que divide
    // todo o espaço de endereçamento de 4GB em seções de 1 MB,
    // todas com Full Access e Strongly Ordered
    "\n     LDR r0, =0xDE2"             // Atribui-se ao R0 parte do descriptor
//  "\n     LDR r1, =ttb_address"
    "\n     LDR r1, =0xFA0000"          // Atribui-se ao R1 endereço base
                                        // da L1 tranlastion table
    "\n     LDR r3, = 4095"             // R3 se torna o contador para o loop

    "\n write_pte:"                     // Label do loop para escrita das
                                        // page table entry (PTE) da translation table
    "\n     ORR r2, r0, r3, LSL #20"    // Atribui-se ao R2 OR entre o endereço
                                        // e os bits padrão da PTE
    "\n     STR r2, [r1, r3, LSL #2]"   // Escreve-se a PTE na translation table
                                        // (endereço de escrita é o ttb_address somado
                                        // com contador e multiplicado por 4)
    "\n     SUB r3, r3, #1"             // Decrementa-se contador do loop
    "\n     CMP r3, #-1"                // Faz-se a comparação para verificar
                                        // se loop acabou
    "\n     BNE write_pte"              // Caso o loop não tenha acabado,
                                        // escreve mais uma pte

    // Faz-se a primeira entrada da translation table
    // cacheable, normal, write-back, write allocate
    "\n     BIC r0, r0, #0xC"           // Limpa-se CB bits
    "\n     ORR r0, r0, #0X4"           // Write-back, write allocate
    "\n     BIC r0, r0, #0x7000"        // Limpa-se TEX bits
    "\n     ORR r0, r0, #0x5000"        // Faz-se TEX write-back e write allocate
    "\n     ORR r0, r0, #0x10000"       // Torna compartilhável
    "\n     STR r0, [r1]"               // Escreve-se na primeira entrada

    // Inicializa a MMU
    "\n     MOV r1,#0x0"
    "\n     MCR p15, 0, r1, c2, c0, 2"  // Escrita do Translation Table Base Control Register
//  "\n     LDR r1, =ttb_address"
    "\n     LDR r1, =0xFA0000"          // Atribui-se ao R1 endereço base
                                        // da L1 tranlastion table
    "\n     MCR p15, 0, r1, c2, c0, 0"  // Escreve-se no reg 1 do coprocessor 15 o que ha
                                        // em r1 (endereco base da tranlastion table)
                                    
    // In this simple example, we don't use TRE or Normal Memory Remap Register.
    // Set all Domains to Client
    "\n     LDR r1, =0x55555555"
    "\n     MCR p15, 0, r1, c3, c0, 0"  // Write Domain Access Control Register
    
    // Enable MMU
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Atribui-se ao R1 o valor do registrador 1 do
                                        // coprocessor 15
    "\n     ORR r1, r1, #0x1"           // Atribui-se ao bit 0 em R1 o valor 1, para
                                        // ligar a MMU
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Escreve-se no reg 1 do coprocessor 15
                                        // o que há em R1, habilitando a MMU
    "\n");

    //
    // SMP initialization 
    // -------------------
    __asm("MRC     p15, 0, r0, c0, c0, 5");     // Read CPU ID register
    __asm("ANDS    r0, r0, #0x03");             // Mask off, leaving the CPU ID field
    __asm("cmp r0, #0"); 
     __asm("BNE    secondary_cpus_init"); 

    // Enable the SCU
    // ---------------
    __asm("BL      enable_scu");
    
    
    ASM(
    // Clear the BSS
    "\n     eor     r0, r0"
    "\n     ldr     r1, =__bss_start__"
    "\n     ldr     r2, =__bss_end__"
    "\n .L1:"
    "\n     str     r0, [r1]"
    "\n     add     r1, #4"
    "\n     cmp     r1, r2"
    "\n     blt     .L1"
    "\n");

    __asm("b _start");

    // ------------------------------------------------------------
    // Initialization for SECONDARY CPUs
    // ------------------------------------------------------------
    __asm("secondary_cpus_init:");
    // Join SMP
    // ---------
    __asm("MRC     p15, 0, r0, c0, c0, 5");     // Read CPU ID register
    __asm("ANDS    r0, r0, #0x03");             // Mask off, leaving the CPU ID field
    __asm("MOV     r1, #0xF");                  // Move 0xF (represents all four ways) into r1
    __asm("BL      secure_SCU_invalidate");
    
    __asm("BL      enable_maintenance_broadcast");

    __asm("b _start");
}