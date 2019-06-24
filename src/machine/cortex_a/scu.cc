extern "C" {

    /* EXPORT get_cpu_id */
    __attribute__((naked)) unsigned int get_cpu_id(void){
    // Returns the CPU ID (0 to 3) of the CPU executed on
/* get_cpu_id PROC */
    __asm("MRC     p15, 0, r0, c0, c0, 5");   // Read CPU ID register
    __asm("AND     r0, r0, #0x03");           // Mask off, leaving the CPU ID field
    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------

    // SCU offset from base of private peripheral space --> 0x000

    /* EXPORT  enable_scu */
    __attribute__((naked)) void enable_scu(void){
    // Enables the SCU
/* enable_scu PROC */

     //enable scu
        __asm("MRC p15, 4, r0, c15, c0, 0");// Read periph base address
        __asm("LDR r1, [r0, #0x0]"); // Read the SCU Control Register
        __asm("ORR r1, r1, #0x1 ");// Set bit 0 (The Enable bit)
        __asm("STR r1, [r0, #0x0]");// Write back modifed value
        __asm("BX lr"); 
    /* ENDP */
}
// ------------------------------------------------------------

    /* EXPORT  join_smp */
    __attribute__((naked)) void join_smp(void){

    // SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg

    __asm("MRC     p15, 0, r0, c1, c0, 1");   // Read ACTLR
    __asm("ORR     r0, r0, #0x040");          // Set bit 6
    __asm("MCR     p15, 0, r0, c1, c0, 1");   // Write ACTLR

    __asm("BX      lr");
    /* ENDP */
}
// ------------------------------------------------------------

    /* EXPORT enable_maintenance_broadcast */
    __attribute__((naked)) void enable_maintenance_broadcast(void){
    // Enable the broadcasting of cache & TLB maintenance operations
    // When enabled AND in SMP, broadcast all "inner sharable"
    // cache and TLM maintenance operations to other SMP cores
/* enable_maintenance_broadcast PROC */
    __asm("MRC     p15, 0, r0, c1, c0, 1");   // Read Aux Ctrl register
    __asm("ORR     r0, r0, #0x01");           // Set the FW bit (bit 0)
    __asm("MCR     p15, 0, r0, c1, c0, 1");   // Write Aux Ctrl register

    __asm("BX      lr");
    /* ENDP */
}

// ------------------------------------------------------------

    /* EXPORT secure_SCU_invalidate */
    __attribute__((naked)) void secure_SCU_invalidate(unsigned int cpu, unsigned int ways){
/* secure_SCU_invalidate PROC */
    __asm("AND     r0, r0, #0x03");           // Mask off unused bits of CPU ID
    __asm("MOV     r0, r0, LSL #2");          // Convert into bit offset (four bits per core)
    
    __asm("AND     r1, r1, #0x0F");           // Mask off unused bits of ways
    __asm("MOV     r1, r1, LSL r0");          // Shift ways into the correct CPU field

    __asm("MRC     p15, 4, r2, c15, c0, 0");  // Read periph base address

    __asm("STR     r1, [r2, #0x0C]");         // Write to SCU Invalidate All in Secure State
    
    __asm("BX      lr");

    /* ENDP */
}

}
