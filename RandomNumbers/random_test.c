#include <string.h>
#include <cx16.h>
#include <stdio.h>

#include "xsrng.h"

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long

#define UINT16_MAX (65535UL)

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
 uint16 tests_run = 0;
 uint16 tests_failed = 0;

/** https://www.pagetable.com/c64ref/kernal/#RDTIM */
#define KERNAL_RDTIM (0xFFDE)

/** https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2005%20-%20KERNAL.md#function-name-i2c_write_byte */
#define KERNAL_I2C_WRITE_BYTE (0xFEC9)

/** pseudo-random number generator state in zero-page */
#define prng_state ((uint16*)0x7e)

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

//===================================================================
// UNIT TESTS
//===================================================================
char error_message[255];

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
                "test-various-seeds: i %u expected %04x actual %04x", 
                i, 
                expected, 
                actual);
            mu_fail(error_message);
        }
    } while (++i);

    return NULL;
}

/** 
 * Seeding from the clock must never produce a state of zero.
 */
static char* test_prng_seed_clock() {
    uint8 i;

    for (i = 0; i < 100; i++) {
        prng_seed_clock();
        mu_assert("test-prng-seed-clock: prng state was zero", *prng_state != 0);
    }

    return NULL;    
}

/** 
 * correctly set the internal PRNG state
 */
static char* test_prng_seed() {
    uint16 seed;

    for (seed = 200; seed < 300; seed++) {
        prng_seed(seed);
        sprintf(error_message, "test-prng-seed: expected %04x actual %04x", seed, *prng_state);
        mu_assert(error_message, *prng_state == seed);
    }

    return NULL;    
}

static char* test_prng_period() {
    uint16 first_output, random, period;

    prng_seed(1);
    first_output = prng();
    period = 0;
    while (random != first_output) {
        random = prng();
        period++;
    }

    if (UINT16_MAX != period) {
        sprintf(error_message, "unexpected period; expected %u actual %u", UINT16_MAX, period);
        mu_fail(error_message);
    }
}

static char* all_tests() {
    puts("\n--- starting unit tests ---\n");
    mu_run_test(test_prng_seed);
    mu_run_test(test_prng_seed_clock);
    mu_run_test(test_all_states);
    mu_run_test(test_prng_period);
    
    printf("\n--- success; all %u tests passed ---\n", tests_run);
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
