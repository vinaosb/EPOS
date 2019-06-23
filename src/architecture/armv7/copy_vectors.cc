extern "C"
{
    void copy_vectors()
    {
#ifndef _DEFINES_
#define _DEFINES_
        typedef unsigned int u32;
#endif
        // copy vector table in ts.s to 0x0
        extern u32 vectors_start, vectors_end;
        u32 *vectors_src = &vectors_start;
        u32 *vectors_dst = (u32 *)0;
        while (vectors_src < &vectors_end)
            *vectors_dst++ = *vectors_src++;
    }
}