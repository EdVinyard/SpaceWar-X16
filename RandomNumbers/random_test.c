#include <string.h>
#include <cx16.h>
#include <stdio.h>

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long

 /* (M)inimal (U)nit testing adapted from https://jera.com/techinfo/jtns/jtn002 */
 #define mu_fail(message) do { return message; } while (0)
 #define mu_assert(message, test) do { if (!(test)) return message; } while (0)
 #define mu_run_test(test)  do { \
                                char *message = test(); \
                                tests_run++; \
                                if (message) { \
                                    tests_failed++; \
                                    puts("fail: "); \
                                    puts(message); \
                                    return message; \
                                } \
                                puts("pass"); \
                            } while (0)
 uint16 tests_run;
 uint16 tests_failed;

/** https://www.pagetable.com/c64ref/kernal/#RDTIM */
#define KERNAL_RDTIM (0xFFDE)

/** https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2005%20-%20KERNAL.md#function-name-i2c_write_byte */
#define KERNAL_I2C_WRITE_BYTE (0xFEC9)

/** pseudo-random number generator state (low byte) in zero-page */
#define prng_state_lo (0x7e)

/** pseudo-random number generator state (high byte) in zero-page */
#define prng_state_hi (0x7f)

/** pseudo-random number generator state in zero-page */
#define prng_state ((uint16*)prng_state_lo)

/**
 * Seed the XOR-shift Linear-Feedback Shift Register pseudo-random number
 * generator initial state from the supplied value.
 */
void __fastcall__ prng_seed(const register uint16 seed) {
    asm("lda <%v", seed);
    asm("ldx >%v", seed);
}

/**
 * Seed the XOR-shift Linear-Feedback Shift Register pseudo-random number
 * generator initial state from the two lower bytes of the system Jiffy clock.
 * 
 * from http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366
 * which is a 6502 port of the Z80 code posted in 
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html 
 * originally devised by George Marsaglia
 */
void __fastcall__ prng_seed_clock()
{
    // There's conflicting documentation for RDTIME for C64
    // (https://www.pagetable.com/c64ref/kernal/#RDTIM).  However, on the X16, I
    // believe the returned values are such that
    //   - A contains the low-order byte, 
    //   - X the middle-order byte, and 
    //   - Y the high-order byte.
PRNG_SEED_RETRY:                        // do {
    asm("jsr %w", KERNAL_RDTIM);        //    A,X,Y = KERNAL_RDTIME
    
    asm("sta %b", prng_state_lo);       //    *prng_state_lo = A
    asm("stx %b", prng_state_hi);       //    *prng_state_hi = X
                                        //    // discard hi-order byte in Y
    asm("ora %b", prng_state_hi);       // } while (0 == *prng_state);
    asm("beq %g", PRNG_SEED_RETRY);
}

/**
 * the state used by the `prng_slow()` XOR-shift LFSR PRNG
 */
uint16 prng_slow_state = 0;

/**
 * Extract two bytes from the XOR-shift Linear-Feedback Shift Register
 * pseudo-random number generator.
 *
 * This is a slow but easy-to-understand implementation.  It exists only to
 * check the correctness much faster `prng()` implementation.
 */
uint16 __fastcall__ prng_slow() {
    uint16 s = prng_slow_state; // only to make the next three lines short
    s ^= s << 7;
    s ^= s >> 9;
    s ^= s << 8;
    return prng_slow_state = s;
}

/**
 * Extract two bytes from the XOR-shift Linear-Feedback Shift Register
 * pseudo-random number generator.
 *
 * from
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366
 * which is a 6502 port of the Z80 code posted in
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
 * which implements the algorithm originally devised by George Marsaglia
 */
uint16 __fastcall__ prng() {
    // register uint16 result;
    // uint16 s;
    // s ^= s << 7;
    // s ^= s >> 9;
    // s ^= s << 8;
    // return s;

    asm("lda %b", prng_state_hi);
    asm("lsr");
    asm("lda %b", prng_state_lo);
    asm("ror");
    asm("eor %b", prng_state_hi);
    // asm("lda #1"); // INTENTIONAL BUG FOR TESTING THE UNIT TESTS
    asm("sta %b", prng_state_hi);   // high byte of x ^= x << 7 done
    asm("ror");                     // A has now x >> 9 and high bit comes from low byte
    asm("eor %b", prng_state_lo);
    asm("sta %b", prng_state_lo);   // x ^= x >> 9 and the low part of x ^= x << 7 done
    asm("eor %b", prng_state_hi);
    asm("sta %b", prng_state_hi);   // x ^= x << 8 done

    // return A,X;
    asm("tax");                     // X = "high" byte
    asm("lda %b", prng_state_lo);   // A = "low" byte
}

//===================================================================
// UNIT TESTS
//===================================================================
char error_message[255];

/** 
 * Starting from the same state, confirm across 2^16 iterations the two
 * implementations' output matches.
 *
 * This is redundant, but I'm leaving it in so there's more than one test in
 * this "test suite".
 */
static char* test_many_iterations() {
    uint16 expected, actual, i;

    prng_seed(1);                    // same seed for both PRNGs
    prng_slow_state = *prng_state;

    i = 0;
    do {
        expected = prng_slow();
        actual = prng();

        if (expected != actual) {
            sprintf(
                error_message, 
                "test-many-iterations: i %d expected %04x actual %04x", 
                i,
                expected,
                actual);
            mu_fail(error_message);
        }
    } while (++i);

    return NULL;
}

/** 
 * Starting from the every possible state, confirm that the two implementations'
 * output matches.
 */
static char* test_all_states() {
    uint16 expected, actual, i;

    i = 0;
    do {
        prng_seed(1);                    // same seed for both PRNGs
        prng_slow_state = *prng_state;

        expected = prng_slow();
        actual = prng();

        if (expected != actual) {
            sprintf(
                error_message, 
                "test-various-seeds: i %d expected %04x actual %04x", 
                i, 
                expected, 
                actual);
            mu_fail(error_message);
        }
    } while (++i);

    return NULL;
}

static char* all_tests() {
    puts("\n--- starting unit tests ---\n");
    mu_run_test(test_many_iterations);
    mu_run_test(test_all_states);
    
    printf("\n--- success; all %d tests passed ---\n", tests_run);
    return NULL;
}

/**
 * Turn off the system immediately.
 */
void power_off() {
    asm("ldx #$42");                        // System Management Controller device ID
    asm("ldy #1");                          // magic location for system poweroff
    asm("lda #0");                          // magic value for system poweroff
    asm("jsr %w", KERNAL_I2C_WRITE_BYTE);   // power off the system
}

void main() {
    all_tests();
    power_off();
}
