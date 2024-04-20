# Fast Pseudo-Random Numbers for the Commander X16

This is a fast (~35 65c02 cycles) [XOR-shift](https://en.wikipedia.org/wiki/Xorshift)
[Linear-Feedback Shift Register
(LFSR)](https://en.wikipedia.org/wiki/Linear-feedback_shift_register)
 pseudo-random number generator implementation with unit tests.  It's a 16-bit
LSFR implementation copied from [the 6502
port](http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366),
of the Z80 code posted in [XOR-shift Pseudorandom numbers in
Z80](http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html)
based on the LFSR originally devised by [George
Marsaglia](https://en.wikipedia.org/wiki/George_Marsaglia).

The unit test mechanism is derived from
[MinUnit](https://jera.com/techinfo/jtns/jtn002), a beautifully minimal unit
testing framework for C by [John Brewer of Jera Design
LLC](https://jera.com/jbrewer).

## Testing

Running `make test` will compile, assembly, and link a `.PRG` file for the
Commander X16, then start the emulator, run the tests, echo all output to your
console, and shut down the emulator.  If all went well, you'll see the following
output

```
--- STARTING UNIT TESTS ---

PASS
PASS
PASS
PASS

--- SUCCESS; ALL 2 TESTS PASSED ---
SMC Power Off.
```

Otherwise, the output will include information about the failed test(s).
Execution stops as soon as the first test failure occurs.

```
--- STARTING UNIT TESTS ---

FAIL:
TEST-MANY-ITERATIONS: I 0 EXPECTED 0000 ACTUAL 0100
SMC Power Off.
```
