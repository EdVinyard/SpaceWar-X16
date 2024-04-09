/**
 * Print "abcde" (PETSCII values 41-45) by supplying each character value in
 * various ways to the kernal routine.  All of the functional code is assembly
 * language, but the point is to demonstrate how to call a kernal routine using
 * values represented using various C-language features.
 */

/** 
 * CHROUT kernal routine (AKA, "BSOUT") that prints a single character at the
 * current cursor location and advances the cursor. 
 *
 * See https://www.pagetable.com/c64ref/kernal/#BSOUT for detailed information
 * about the equivalent Commodore 64 KERNAL routine.
 */
#define CHROUT (asm("jsr %w", 0xFFD2))

// PETSCII 'B'
#define BEE (0x42)

char CEE = 0x43; // PETSCII 'C'

void main() {
    // depending on --register-vars compiler option, this is either a global
    // (zero-page) address, or a stack-pointer-relative address
    register char DEE = 0x44; // PETSCII 'D'

    // without "register" modifier, this variable is allocated on the stack
    char EEE = 0x45; // PETSCII 'E'

    // literal value
    asm("lda #$41"); // PETSCII 'A' - see https://style64.org/petscii/
    CHROUT;

    // #define value
    asm("lda #%b", BEE);
    CHROUT;

    // global variable
    asm("lda %v",  CEE);
    CHROUT;
    
    // With the -r or --register-vars compiler option, the "register" modifier
    // places variables in the zero-page so they become a (temporarily) global
    // variable. Without the compiler flag, we must treat them just like any
    // other stack-allocated local variable.  See
    // https://www.cc65.org/doc/cc65-8.html#regvars
    //
    // A mismatch between the compiler flag and format specified (%v or %o) are
    // signaled by compiler errors like "Type of argument 1 differs from format
    // specifier".
    #ifdef __OPT_r__
        asm("lda %v", DEE);
    #else
        asm("ldy #%o", DEE);
        asm("lda (sp),y");
    #endif
    CHROUT;

    // local variable (i.e., relative to the stack pointer register contents)
    asm("ldy #%o", EEE);        // Y = stack-offset-of-EEE
    asm("lda (sp),y");          // A = *(C-stack-pointer + Y)
    // NOTE: CC65's C-stack pointer "(sp)" is NOT the same as the hardware stack
    // pointer register.  The CC65 docs
    // (https://cc65.github.io/doc/cc65-intern.html#s2) specify that the, "sp
    // pseudo-register is a zeropage pointer to the base of the C-stack."
    CHROUT;

    // Take a single-frame screenshot; assumes x16emu was started with "-gif"
    // command line option.
    *((char*)0x9FB5) = 1;
}
