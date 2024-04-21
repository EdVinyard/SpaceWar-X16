#define uint16 unsigned short

/**
 * Seed the XOR-shift Linear-Feedback Shift Register pseudo-random number
 * generator initial state from the supplied value.  Use this function if
 * you want a deterministic sequence of random numbers.
 *
 * @param seed a NON-ZERO seed value
 */
void __fastcall__ prng_seed(const uint16 seed);

/**
 * Seed the XOR-shift Linear-Feedback Shift Register pseudo-random number
 * generator initial state from the two lower bytes of the system Jiffy clock.
 * Use this function if you want a non-deterministic sequence of random numbers.
 *
 * from
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366
 * which is a 6502 port of the Z80 code posted in
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
 * originally devised by George Marsaglia
 */
void __fastcall__ prng_seed_clock();

/**
 * Extract two pseudo-random bytes from the XOR-shift Linear-Feedback Shift
 * Register pseudo-random number generator.
 *
 * from
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366
 * which is a 6502 port of the Z80 code posted in
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
 * which implements the algorithm originally devised by George Marsaglia
 */
uint16 __fastcall__ prng();
