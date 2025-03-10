rm RNGTEST.PRG xsrng.o random_test.o
cl65 --target cx16 --verbose random_test.c xsrng.s -o RNGTEST.PRG
x16emu -startin . -prg RNGTEST.PRG -run -echo -warp
