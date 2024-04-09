X16EMU := ../../x16emu.exe
CL65 := cl65

SRC_FILES := $(wildcard *.C)
PRG_FILES := $(patsubst %.C,%.PRG,$(SRC_FILES))

COMPILER_OPTS := --target cx16 --register-vars --verbose

all: $(PRG_FILES)

%.PRG: %.C
	$(CL65) $(COMPILER_OPTS) -o $@ -l $(patsubst %.C,%.list,$<) $<

run: all
	$(X16EMU) -scale 2 -gif "spriteload.gif",wait -startin . -prg ./SPRITELOAD.PRG -run
