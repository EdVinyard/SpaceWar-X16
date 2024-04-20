X16EMU := ../../x16emu.exe
LINK := cl65
COMPILE := cc65
ASSEMBLE := ca65

TARGET := EARTHSPIN.PRG

ALL_OBJS = \
	$(patsubst %.c,build/%.o,$(wildcard *.c)) \
	$(patsubst %.s,build/%.o,$(wildcard *.s))

OPTS := --target cx16 --verbose
COMPILER_OPTS := $(OPTS) --register-vars

run: $(TARGET)
	$(X16EMU) -scale 2 -gif "earthspin.gif",wait -startin . -prg ./EARTHSPIN.PRG -run

build: $(TARGET)

clean:
	rm build/*
	rm $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(LINK) $(OPTS) -o $@ -l build/$(subst .PRG,.list,$@) -m build/$(subst .PRG,.mmap,$@) $(ALL_OBJS)

build/%.o: build/%.s
	$(ASSEMBLE) $(OPTS) -o $@ $<

build/%.o: %.s
	$(ASSEMBLE) $(OPTS) -o $@ $<

build/%.s: %.c
	$(COMPILE) $(COMPILER_OPTS) -o $@ $<
