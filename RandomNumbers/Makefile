X16EMU := ../../x16emu.exe
LINK := cl65
COMPILE := cc65
ASSEMBLE := ca65

TARGET := RNGTEST.PRG

ALL_OBJS = \
	$(patsubst %.c,build/%.o,$(wildcard *.c)) \
	$(patsubst %.s,build/%.o,$(wildcard *.s))

OPTS := --target cx16 --verbose
COMPILER_OPTS := $(OPTS) --register-vars

build: $(TARGET)

test: $(TARGET)
	$(X16EMU) -startin . -prg $(TARGET) -run -echo -warp

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
