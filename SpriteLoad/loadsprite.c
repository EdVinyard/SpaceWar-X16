//#include <stdio.h>
// #include <cx16.h>
//#include <string.h>

#define uchar unsigned char
#define ushort unsigned short

/** https://www.pagetable.com/c64ref/kernal/#LOAD */
#define KERNAL_LOAD (0xFFD5)
/** https://www.pagetable.com/c64ref/kernal/#SETLFS */
#define KERNAL_SETLFS (0xFFBA)
/** https://www.pagetable.com/c64ref/kernal/#SETNAM */
#define KERNAL_SETNAM (0xFFBD)
/** https://www.pagetable.com/c64ref/kernal/#BSOUT */
#define KERNAL_CHROUT (0xFFD2)

/**
 * Print a string.  Like the KERNAL routine CHROUT, but for a whole
 * zero-terminated, C-style string.
 *
 * (This isn't useful; Ed is learning how to invoke a KERNAL routine from C.)
 *
 * @param s a string of 0 to 255 characters; longer strings cause undefined
 * behavior
 * @returns count of characters printed
 */
// uchar strout(const char* s) {
//     // KLUDGE: Copy parameters from the stack into zero page, which will make
//     // the addresses absolute for the inline assembly that follows.  Learning
//     // how to use the "%o" (stack offset) format specifier would be better.
//     const register uchar zp_len = (uchar)strlen(s);
//     const register uchar* zp_s = s;

//     if (0 == zp_len) return zp_len;

//     asm("ldy #0");                      // Y = 0
// strout_loop:                            // do
//     asm("lda (%v),y", zp_s);            //     A = *(zp_s + Y)
//     asm("jsr %w", KERNAL_CHROUT);       //     CHROUT(A)
//     asm("iny");                         //     Y = Y + 1
//     asm("cpy %v", zp_len);              // while (zp_len <> Y)
//     asm("bne %g", strout_loop);

//     return zp_len;
// }

/**
 * Load the raw bytes of the specified file into video RAM. Indended to be very
 * similar to the BASIC BVLOAD command
 * https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2004%20-%20BASIC.md#bvload
 *
 * WARNING: Behavior is undefined and probably erroneous if logical file number
 * 1 is already open.
 *
 * @param filename for example, "SPRITE.BxsIN"
 * @param device 8 for the SDCARD
 * @param vram_addr_lo low two bytes of video RAM address (high bit is always 1
 * for this function)
 */
// void load_sprite(
//     char* filename,
//     uchar device,
//     ushort vram_addr)
// {
//     // KLUDGE: Copy parameters from the stack into zero page, which will make
//     // the addresses absolute for the inline assembly that follows.  Learning
//     // how to use the "%o" (stack offset) format specifier would be faster.
//     register char* zp_filename = filename;
//     register uchar zp_filename_len = strlen(zp_filename);
//     register uchar zp_device = device;
//     register uchar zp_vram_addr = vram_addr;

//     // Set the filename
//     asm("lda %v", zp_filename_len);
//     asm("ldx <%v", zp_filename);            // low byte
//     asm("ldy >%v", zp_filename);            // high byte
//     asm("jsr #%w", KERNAL_SETNAM);

//     // Set up a logical file 
//     asm("lda #1");                          // logical file number 1
//     asm("ldx #8");                          // disk drive / SD card is device 8
//     asm("ldy #$255");                       // secondary address (255 means "unused")
//     asm("jsr %w", KERNAL_SETLFS);

//     // Kernal LOAD
//     // https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2005%20-%20KERNAL.md#function-name-load
//     // See also https://www.pagetable.com/c64ref/kernal/#LOAD
//     asm("lda $3"); // 3 => $10000 + address
//     asm("ldx <%v", zp_vram_addr);           // low byte of target VRAM
//     asm("ldy >%v", zp_vram_addr);           // high byte of target VRAM
//     asm("jsr %w", KERNAL_LOAD);

//     // TODO: Kernal LOAD docs specify lo/hi addr. in X/Y with example.
// }

#define CHROUT (asm("jsr %w", KERNAL_CHROUT));

// PETSCII 'B'
#define BEE (0x42)
char CEE = 0x43; // PETSCII 'C'

void kernal_routine_parameter_examples() {
    register char DEE = 0x44; // PETSCII 'D'

    // without the "register" modifier, this variable will be allocated on the stack
    char EEE = 0x45; // PETSCII 'E'

    // literal value
    asm("lda #$41"); // PETSCII 'A'
    CHROUT;

    // #define value
    asm("lda #%b", BEE);
    CHROUT;

    // global variable
    asm("lda %v",  CEE);
    CHROUT;
    
    // With the "-r" compiler option, the "register" modifier places variables
    // in the zero-page so they're used just like a global variable.  Without
    // the compiler flag, we must treat them just like any other stack-allocated
    // local variable.
    //
    // A mismatch between the compiler flag and "register" variable allocation
    // method will be signaled by compiler errors like "Type of argument 1
    // differs from format specifier".
    #ifdef __OPT_r__
        asm("lda %v", DEE);
    #else
        asm("ldy #%o", DEE);
        asm("lda (sp),y");
    #endif
    CHROUT;

    // local variable (i.e., relative to the stack pointer register contents)
    asm("ldy #%o", EEE);
    asm("lda (sp),y");
    CHROUT;
}

void main() {
    kernal_routine_parameter_examples();
}
