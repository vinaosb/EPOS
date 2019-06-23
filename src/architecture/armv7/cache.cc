// ------------------------------------------------------------
// v7-A Cache and Branch Prediction Maintenance Operations
// ------------------------------------------------------------

    /* PRESERVE8 */

    /* AREA  v7CacheOpp,CODE,READONLY */

#include <architecture/armv7/cache.h>
extern "C" {
// ------------------------------------------------------------
// Cache Maintenance
// ------------------------------------------------------------

    /* EXPORT enable_caches */
    __attribute__((naked)) void enable_caches(void){
/* enable_caches  PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0");   // Read System Control Register configuration data
    __asm("ORR     r0, r0, #(1 << 2)");       // Set C bit
    __asm("ORR     r0, r0, #(1 << 12)");      // Set I bit
    __asm("MCR     p15, 0, r0, c1, c0, 0");   // Write System Control Register configuration data
    __asm("BX      lr");
    /* ENDP */
}

    /* EXPORT disable_caches */
    __attribute__((naked)) void disable_caches(void){
/* disable_caches  PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0");   // Read System Control Register configuration data
    __asm("BIC     r0, r0, #(1 << 2)");       // Clear C bit
    __asm("BIC     r0, r0, #(1 << 12)");      // Clear I bit
    __asm("MCR     p15, 0, r0, c1, c0, 0");   // Write System Control Register configuration data
    __asm("BX      lr");
    /* ENDP */
}

    /* EXPORT clean_dcache */
    __attribute__((naked)) void clean_dcache(void){
/* clean_dcache  PROC */
    __asm("PUSH    {r4-r12}");
    
    //
    // Based on code example given in section 11.2.4 of ARM DDI 0406B
    //

    __asm("MRC     p15, 1, r0, c0, c0, 1");     // Read CLIDR
    __asm("ANDS    r3, r0, #0x7000000");
    __asm("MOV     r3, r3, LSR #23");           // Cache level value (naturally aligned)
    __asm("BEQ     clean_dcache_finished");
    __asm("MOV     r10, #0");

    __asm("clean_dcache_loop1:");
    __asm("ADD     r2, r10, r10, LSR #1");      // Work out 3xcachelevel
    __asm("MOV     r1, r0, LSR r2");            // bottom 3 bits are the Cache type for this level
    __asm("AND     r1, r1, #7");                // get those 3 bits alone
    __asm("CMP     r1, #2");
    __asm("BLT     clean_dcache_skip");         // no cache or only instruction cache at this level
    __asm("MCR     p15, 2, r10, c0, c0, 0");    // write the Cache Size selection register
    __asm("ISB");                               // ISB to sync the change to the CacheSizeID reg
    __asm("MRC     p15, 1, r1, c0, c0, 0");     // reads current Cache Size ID register
    __asm("AND     r2, r1, #0x7");               // extract the line length field
    __asm("ADD     r2, r2, #4");                // add 4 for the line length offset (log2 16 bytes)
    __asm("LDR     r4, =0x3FF");
    __asm("ANDS    r4, r4, r1, LSR #3");        // R4 is the max number on the way size (right aligned)
    __asm("CLZ     r5, r4");                    // R5 is the bit position of the way size increment
    __asm("LDR     r7, =0x00007FFF");
    __asm("ANDS    r7, r7, r1, LSR #13");       // R7 is the max number of the index size (right aligned)

    __asm("clean_dcache_loop2:");
    __asm("MOV     r9, R4");                    // R9 working copy of the max way size (right aligned)

    __asm("clean_dcache_loop3:");
    __asm("ORR     r11, r10, r9, LSL r5");      // factor in the way number and cache number into R11
    __asm("ORR     r11, r11, r7, LSL r2");      // factor in the index number
    __asm("MCR     p15, 0, r11, c7, c10, 2");   // DCCSW - clean by set/way
    __asm("SUBS    r9, r9, #1");                // decrement the way number
    __asm("BGE     clean_dcache_loop3");
    __asm("SUBS    r7, r7, #1");                // decrement the index
    __asm("BGE     clean_dcache_loop2");

    __asm("clean_dcache_skip:");
    __asm("ADD     r10, r10, #2");              // increment the cache number
    __asm("CMP     r3, r10");
    __asm("BGT     clean_dcache_loop1");

    __asm("clean_dcache_finished:");
    __asm("POP     {r4-r12}");

    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT clean_invalidate_dcache */
    __attribute__((naked)) void clean_invalidate_dcache(void){
/* clean_invalidate_dcache  PROC */
    __asm("PUSH    {r4-r12}");
    
    //
    // Based on code example given in section 11.2.4 of ARM DDI 0406B
    //

    __asm("MRC     p15, 1, r0, c0, c0, 1");     // Read CLIDR
    __asm("ANDS    r3, r0, #0x7000000");
    __asm("MOV     r3, r3, LSR #23");           // Cache level value (naturally aligned)
    __asm("BEQ     clean_invalidate_dcache_finished");
    __asm("MOV     r10, #0");

    __asm("clean_invalidate_dcache_loop1:");
    __asm("ADD     r2, r10, r10, LSR #1");      // Work out 3xcachelevel
    __asm("MOV     r1, r0, LSR r2");            // bottom 3 bits are the Cache type for this level
    __asm("AND     r1, r1, #7");                // get those 3 bits alone
    __asm("CMP     r1, #2");
    __asm("BLT     clean_invalidate_dcache_skip"); // no cache or only instruction cache at this level
    __asm("MCR     p15, 2, r10, c0, c0, 0");    // write the Cache Size selection register
    __asm("ISB");                               // ISB to sync the change to the CacheSizeID reg
    __asm("MRC     p15, 1, r1, c0, c0, 0");     // reads current Cache Size ID register
    __asm("AND     r2, r1, #0x7");               // extract the line length field
    __asm("ADD     r2, r2, #4");                // add 4 for the line length offset (log2 16 bytes)
    __asm("LDR     r4, =0x3FF");
    __asm("ANDS    r4, r4, r1, LSR #3");        // R4 is the max number on the way size (right aligned)
    __asm("CLZ     r5, r4");                    // R5 is the bit position of the way size increment
    __asm("LDR     r7, =0x00007FFF");
    __asm("ANDS    r7, r7, r1, LSR #13");       // R7 is the max number of the index size (right aligned)

    __asm("clean_invalidate_dcache_loop2:");
    __asm("MOV     r9, R4");                    // R9 working copy of the max way size (right aligned)

    __asm("clean_invalidate_dcache_loop3:");
    __asm("ORR     r11, r10, r9, LSL r5");      // factor in the way number and cache number into R11
    __asm("ORR     r11, r11, r7, LSL r2");      // factor in the index number
    __asm("MCR     p15, 0, r11, c7, c14, 2");   // DCCISW - clean and invalidate by set/way
    __asm("SUBS    r9, r9, #1");                // decrement the way number
    __asm("BGE     clean_invalidate_dcache_loop3");
    __asm("SUBS    r7, r7, #1");                // decrement the index
    __asm("BGE     clean_invalidate_dcache_loop2");

    __asm("clean_invalidate_dcache_skip:");
    __asm("ADD     r10, r10, #2");              // increment the cache number
    __asm("CMP     r3, r10");
    __asm("BGT     clean_invalidate_dcache_loop1");

    __asm("clean_invalidate_dcache_finished:");
    __asm("POP     {r4-r12}");

    __asm("BX      lr");
    /* ENDP */
}

    /* EXPORT invalidate_caches */
    __attribute__((naked)) void invalidate_caches(void){
/* invalidate_caches PROC */
    __asm("PUSH    {r4-r12}");
    
    //
    // Based on code example given in section B2.2.4/11.2.4 of ARM DDI 0406B
    //

    __asm("MOV     r0, #0");
    __asm("MCR     p15, 0, r0, c7, c5, 0");     // ICIALLU - Invalidate entire I Cache, and flushes branch target cache

    __asm("MRC     p15, 1, r0, c0, c0, 1");     // Read CLIDR
    __asm("ANDS    r3, r0, #0x7000000");
    __asm("MOV     r3, r3, LSR #23");           // Cache level value (naturally aligned)
    __asm("BEQ     invalidate_caches_finished");
    __asm("MOV     r10, #0");

    __asm("invalidate_caches_loop1:");
    __asm("ADD     r2, r10, r10, LSR #1");      // Work out 3xcachelevel
    __asm("MOV     r1, r0, LSR r2");            // bottom 3 bits are the Cache type for this level
    __asm("AND     r1, r1, #7");                // get those 3 bits alone
    __asm("CMP     r1, #2");
    __asm("BLT     invalidate_caches_skip");    // no cache or only instruction cache at this level
    __asm("MCR     p15, 2, r10, c0, c0, 0");    // write the Cache Size selection register
    __asm("ISB");                               // ISB to sync the change to the CacheSizeID reg
    __asm("MRC     p15, 1, r1, c0, c0, 0");     // reads current Cache Size ID register
    __asm("AND     r2, r1, #0x7");               // extract the line length field
    __asm("ADD     r2, r2, #4");                // add 4 for the line length offset (log2 16 bytes)
    __asm("LDR     r4, =0x3FF");
    __asm("ANDS    r4, r4, r1, LSR #3");        // R4 is the max number on the way size (right aligned)
    __asm("CLZ     r5, r4");                    // R5 is the bit position of the way size increment
    __asm("LDR     r7, =0x00007FFF");
    __asm("ANDS    r7, r7, r1, LSR #13");       // R7 is the max number of the index size (right aligned)

    __asm("invalidate_caches_loop2:");
    __asm("MOV     r9, R4");                    // R9 working copy of the max way size (right aligned)

    __asm("invalidate_caches_loop3:");
    __asm("ORR     r11, r10, r9, LSL r5");      // factor in the way number and cache number into R11
    __asm("ORR     r11, r11, r7, LSL r2");      // factor in the index number
    __asm("MCR     p15, 0, r11, c7, c6, 2");    // DCISW - invalidate by set/way
    __asm("SUBS    r9, r9, #1");                // decrement the way number
    __asm("BGE     invalidate_caches_loop3");
    __asm("SUBS    r7, r7, #1");                // decrement the index
    __asm("BGE     invalidate_caches_loop2");

    __asm("invalidate_caches_skip:");
    __asm("ADD     r10, r10, #2");              // increment the cache number
    __asm("CMP     r3, r10");
    __asm("BGT     invalidate_caches_loop1");

    __asm("invalidate_caches_finished:");
    __asm("POP     {r4-r12}");
    __asm("BX      lr");
    /* ENDP */
}

    /* EXPORT invalidate_caches_is */
    __attribute__((naked)) void invalidate_caches_is(void){
/* invalidate_caches_is PROC */
    __asm("PUSH    {r4-r12}");

    __asm("MOV     r0, #0");
    __asm("MCR     p15, 0, r0, c7, c1, 0");     // ICIALLUIS - Invalidate entire I Cache inner shareable

    __asm("MRC     p15, 1, r0, c0, c0, 1");     // Read CLIDR
    __asm("ANDS    r3, r0, #0x7000000");
    __asm("MOV     r3, r3, LSR #23");           // Cache level value (naturally aligned)
    __asm("BEQ     invalidate_caches_is_finished");
    __asm("MOV     r10, #0");

    __asm("invalidate_caches_is_loop1:");
    __asm("ADD     r2, r10, r10, LSR #1");      // Work out 3xcachelevel
    __asm("MOV     r1, r0, LSR r2");            // bottom 3 bits are the Cache type for this level
    __asm("AND     r1, r1, #7");                // get those 3 bits alone
    __asm("CMP     r1, #2");
    __asm("BLT     invalidate_caches_is_skip"); // no cache or only instruction cache at this level
    __asm("MCR     p15, 2, r10, c0, c0, 0");    // write the Cache Size selection register
    __asm("ISB");                               // ISB to sync the change to the CacheSizeID reg
    __asm("MRC     p15, 1, r1, c0, c0, 0");     // reads current Cache Size ID register
    __asm("AND     r2, r1, #0x7");               // extract the line length field
    __asm("ADD     r2, r2, #4");                // add 4 for the line length offset (log2 16 bytes)
    __asm("LDR     r4, =0x3FF");
    __asm("ANDS    r4, r4, r1, LSR #3");        // R4 is the max number on the way size (right aligned)
    __asm("CLZ     r5, r4");                    // R5 is the bit position of the way size increment
    __asm("LDR     r7, =0x00007FFF");
    __asm("ANDS    r7, r7, r1, LSR #13");       // R7 is the max number of the index size (right aligned)

    __asm("invalidate_caches_is_loop2:");
    __asm("MOV     r9, R4");                    // R9 working copy of the max way size (right aligned)

    __asm("invalidate_caches_is_loop3:");
    __asm("ORR     r11, r10, r9, LSL r5");      // factor in the way number and cache number into R11
    __asm("ORR     r11, r11, r7, LSL r2");      // factor in the index number
    __asm("MCR     p15, 0, r11, c7, c6, 2");    // DCISW - clean by set/way
    __asm("SUBS    r9, r9, #1");                // decrement the way number
    __asm("BGE     invalidate_caches_is_loop3");
    __asm("SUBS    r7, r7, #1");                // decrement the index
    __asm("BGE     invalidate_caches_is_loop2");

    __asm("invalidate_caches_is_skip:");
    __asm("ADD     r10, r10, #2");              // increment the cache number
    __asm("CMP     r3, r10");
    __asm("BGT     invalidate_caches_is_loop1");

    __asm("invalidate_caches_is_finished:");
    __asm("POP     {r4-r12}");
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// TLB
// ------------------------------------------------------------

    /* EXPORT invalidate_unified_tlb */
    __attribute__((naked)) void invalidate_unified_tlb(void){
/* invalidate_unified_tlb PROC */
    __asm("MOV     r0, #0");
    __asm("MCR     p15, 0, r0, c8, c7, 0");                 // TLBIALL - Invalidate entire unified TLB
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT invalidate_unified_tlb_is */
    __attribute__((naked)) void invalidate_unified_tlb_is(void){
/* invalidate_unified_tlb_is PROC */
    __asm("MOV     r0, #1");
    __asm("MCR     p15, 0, r0, c8, c3, 0");                 // TLBIALLIS - Invalidate entire unified TLB Inner Shareable
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// Branch Prediction
// ------------------------------------------------------------

    /* EXPORT enable_branch_prediction */
    __attribute__((naked)) void enable_branch_prediction(void){
/* enable_branch_prediction PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0");                 // Read SCTLR
    __asm("ORR     r0, r0, #(1 << 11)");                    // Set the Z bit (bit 11)
    __asm("MCR     p15, 0,r0, c1, c0, 0");                  // Write SCTLR
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT disable_branch_prediction */
    __attribute__((naked)) void disable_branch_prediction(void){
/* disable_branch_prediction PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0");                 // Read SCTLR
    __asm("BIC     r0, r0, #(1 << 11)");                    // Clear the Z bit (bit 11)
    __asm("MCR     p15, 0,r0, c1, c0, 0");                  // Write SCTLR
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT invalidate_branch_target_cache */
    __attribute__((naked)) void invalidate_branch_target_cache(void){
/* invalidate_branch_target_cache PROC */
    __asm("MOV     r0, #0");
    __asm("MCR     p15, 0, r0, c7, c5, 6");                 // BPIALL - Invalidate entire branch predictor array
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT invalidate_branch_target_cache_is */
    __attribute__((naked)) void invalidate_branch_target_cache_is(void){
/* invalidate_branch_target_cache_is PROC */
    __asm("MOV     r0, #0");
    __asm("MCR     p15, 0, r0, c7, c1, 6");                 // BPIALLIS - Invalidate entire branch predictor array Inner Shareable
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// High Vecs
// ------------------------------------------------------------

    /* EXPORT enable_highvecs */
    __attribute__((naked)) void enable_highvecs(void){
/* enable_highvecs PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0"); // Read Control Register
    __asm("ORR     r0, r0, #(1 << 13)");    // Set the V bit (bit 13)
    __asm("MCR     p15, 0, r0, c1, c0, 0"); // Write Control Register
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT disable_highvecs */
    __attribute__((naked)) void disable_highvecs(void){
/* disable_highvecs PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 0"); // Read Control Register
    __asm("BIC     r0, r0, #(1 << 13)");    // Clear the V bit (bit 13)
    __asm("MCR     p15, 0, r0, c1, c0, 0"); // Write Control Register
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// Context ID
// ------------------------------------------------------------

    /* EXPORT get_context_id */
    __attribute__((naked)) uint32_t get_context_id(void){
/* get_context_id PROC */
    __asm("MRC     p15, 0, r0, c13, c0, 1"); // Read Context ID Register
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT set_context_id */
    __attribute__((naked)) void set_context_id(uint32_t){
/* set_context_id PROC */
    __asm("MCR     p15, 0, r0, c13, c0, 1"); // Write Context ID Register
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// ID registers
// ------------------------------------------------------------

    /* EXPORT get_MIDR */
    __attribute__((naked)) uint32_t get_MIDR(void){
/* get_MIDR PROC */
    __asm("MRC     p15, 0, r0, c0, c0, 0"); // Read Main ID Register (MIDR)
    __asm("BX      lr");
    /* ENDP */
}
    /* EXPORT get_MPIDR */
    __attribute__((naked)) uint32_t get_MPIDR(void){
/* get_MPIDR PROC */
    __asm("MRC     p15, 0, r0, c0 ,c0, 5");// Read Multiprocessor ID register (MPIDR)
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------
// End of code
// ------------------------------------------------------------

    /* END */

// ------------------------------------------------------------
// End of v7.s
// ------------------------------------------------------------
}