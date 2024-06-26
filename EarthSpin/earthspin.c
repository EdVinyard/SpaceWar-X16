#include <string.h>
#include <cx16.h>
#include <stdio.h>

#include "bitmap_clear.h"
#include "xsrng.h"

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long

struct LoHi {
    uint8 lo;
    uint8 hi;
};

typedef union {
    uint16 s;
    struct LoHi b;
} TwoBytes;

/** https://www.pagetable.com/c64ref/kernal/#LOAD */
#define KERNAL_LOAD (0xFFD5)
/** https://www.pagetable.com/c64ref/kernal/#SETLFS */
#define KERNAL_SETLFS (0xFFBA)
/** https://www.pagetable.com/c64ref/kernal/#SETNAM */
#define KERNAL_SETNAM (0xFFBD)
/** https://www.pagetable.com/c64ref/kernal/#RDTIM */
#define KERNAL_RDTIM (0xFFDE)
/** https://www.pagetable.com/c64ref/kernal/#CHROUT */
#define KERNAL_CHROUT (0xFFD2)
#define CHROUT (asm("jsr %w", KERNAL_CHROUT))
#define NEWLINE asm("lda #13"); CHROUT

#define VERA_LAYER_ALL (0b11)
#define TRUE (1)
#define VERA_SPRITE_ATTR_BASE ((uint32)0x1FC00)
/**
 * Address of beginning of the sprite attribute block/structure for I-ith
 * sprite.
 *
 * VERA_SPRITE_ATTR_BASE + (I * sizeof(struct VeraSpriteAttr))
 * VERA_SPRITE_ATTR_BASE + (I * 8)
 * VERA_SPRITE_ATTR_BASE + (I << 3)
 */
#define VERA_SPRITE_ATTR_ADDR(I) (VERA_SPRITE_ATTR_BASE + (I << 3))

#define BPP_4 (0b00000000)
#define BPP_8 (0b10000000)
#define FRONT (0b00001100)
#define HEIGHT_8 (0)
#define WIDTH_8 (0)
#define OFFSET_0 (0)

/**
 * TODO: Is this even useful?  We have to use VPEEK and VPOKE, or manually
 * perform serial writes to VERA registers in order to write to VRAM, so I'm not
 * sure there's any point to this.  Maybe you coule write the structure data
 * into RAM first, and then fast-copy into VRAM?
 *
 * https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2009%20-%20VERA%20Programmer's%20Reference.md#sprite-attributes
 */
struct VeraSpriteAttr {
    /**
     * (low) bits 12 to 5 of the sprite data address; NOTICE: the minimum size
     * of a sprite is 32 bytes (4 bits * 64 pixels), so bits 4 to 0 of the
     * adrress are NOT present
     */
    uint8 address_lo;
    /**
     * bit 7 :(0 means 4 bit-per-pixel
     * bit 3-0: (high) bits 16 to 13 of the sprite data address
     */
    uint8 mode_and_address_hi;
    /** low 8 bits of X-coordinate */
    uint8 x_lo;
    /** high 2 bits of X-coordinate */
    uint8 x_hi;
    /** low 8 bits of Y-coordinate */
    uint8 y_lo;
    /** high 2 bits of Y-coordinate */
    uint8 y_hi;
    /**
     * bit 7-4: collision mask
     * bit 3-2: Z-depth (0 => disabled, 3 => in front, see docs for others)
     * bit 1:   vertical flip
     * bit 0:   horizonal flip
     */
    uint8 collision_mask_z_depth_vflip_hflip;
    /**
     * bit 7-6: sprite height (0 => 8px, 1 => 16px, 2 => 32px, 3 => 64px)
     * bit 5-4: sprite width (same values as height)
     * bit 3-0: palette offset
     */
    uint8 height_width_palette_offset;
};

/**
 * Load the raw bytes of the specified file into video RAM. Indended to be very
 * similar to the BASIC BVLOAD command
 * https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2004%20-%20BASIC.md#bvload
 *
 * WARNING: Behavior is undefined and probably erroneous if logical file number
 * 1 is already open.
 *
 * @param filename name of file in current directory of SD card; for example "SPRITE.BIN"
 * @param vram_addr_lo low two bytes of video RAM address (high bit is always 1
 * for this function)
 * @return 0 when the load succeeded; a non-zero error code otherwise
 */
char load_sprite(const char* filename, const char* vram_addr)
{
    // Copy parameters from the stack into zero page, which will make the
    // addresses absolute for the inline assembly that follows.  Learning how to
    // use the "%o" (stack offset) format specifier might be better, but is more
    // complicated.  See ../KernalCall directory.
    register char error = 0;
    const register uint8 zp_filename_len = strlen(filename);
    const register char* zp_filename = filename;
    const register char* zp_vram_addr = vram_addr;

    // Set the filename
    asm("lda %v",    zp_filename_len);
    asm("ldx %v",    zp_filename);          // low byte
    asm("ldy %v+%b", zp_filename, 1);       // high byte
    asm("jsr %w",    KERNAL_SETNAM);

    // Set up a logical file
    asm("lda #1");                  // If only one file is opened at a time, $01 can be used.
    asm("ldx #8");                  // On the X16, $08 would be the SD card.
    asm("ldy #2");                  // 2: load whole file to X/Y addr; "headerless load"
    asm("jsr %w", KERNAL_SETLFS);

    // Kernal LOAD
    // https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2005%20-%20KERNAL.md#function-name-load
    // See also https://www.pagetable.com/c64ref/kernal/#LOAD
    asm("lda #3");                          // 3 means VRAM page 1 ($10000 + address)
    asm("ldx %v",    zp_vram_addr);         // low byte of target VRAM addr.
    asm("ldy %v+%b", zp_vram_addr, 1);      // high byte of target VRAM addr.
    asm("jsr %w", KERNAL_LOAD);

    asm("bcc %g", load_sprite_no_error);    // carry flag is set if there was an error
    asm("sta %v", error);                   // error code is in the A-register

    puts("LOAD error: ");
    switch (error) {
        case 4:
            puts("file not found");
            break;
        case 5:
            puts("device not present");
            break;
        case 8:
            puts("no name specified for serial load");
            break;
        case 9:
            puts("illegal device number");
            break;
        default:
            printf("%d", error);
            break;
    }
    return error;

load_sprite_no_error:
    // TODO: Kernal LOAD docs specify lo/hi addr. in X/Y with example.

    // asm("stx %v", zp_filename_len);
    // asm("sty %v", error);
    // printf("final write address + 1 was %x %x\n", error, zp_filename_len);
    return 0;
}

void __fastcall__ putdxb(uint8 value) {
    char mask = 0b10000000;

    putchar('b');
    for (mask = 0b10000000; mask != 0; mask >>= 1) {
        putchar((value & mask) ? '1' : '0');

        if (0b00010000 == mask) {
            putchar(' ');
        }
    }

    printf(" d%03d x%02x", value, value);
}

/**
 * Print the sprite attributes.
 *
 * @param i sprite index (0-127)
 */
void sprite_print_attrs(uint8 i) {
    uint8 j = 0;
    uint32 addr = VERA_SPRITE_ATTR_ADDR(i);

    for (j = 0; j < 8; j++) {
        printf("+%d  ", j);
        putdxb(vpeek(addr + j));
        putchar('\n');
    }
    putchar('\n');
}

/**
 * Set a sprite bitmap data address and color depth.
 *
 * @param i sprite index (0-127)
 * @param bpp bits-per-pixel (`BPP_8` or `BPP_4`)
 * @param sprite_data_addr address of start of sprite data (e.g., $13000); must
 * be evenly divisible by 32
 */
void sprite_set_addr(uint8 i, uint8 bpp, uint32 sprite_data_addr) {
    // beginning of the sprite attribute block/structure
    uint32 sprite_attr_addr = VERA_SPRITE_ATTR_ADDR(i);
    char bits_12_5 = 0;
    char bits_16_13 = 0;

    // discard the low 5 bits; address must be a multiple of 32
    sprite_data_addr >>= 5;
    bits_12_5 = (char)sprite_data_addr;

    // discard the low byte
    sprite_data_addr >>= 8;
    bits_16_13 = (char)sprite_data_addr;

    // printf("hi %x lo %x\n", bits_16_13, bits_12_5);
    vpoke(      bits_12_5,  sprite_attr_addr);
    vpoke(bpp | bits_16_13, sprite_attr_addr + 1);
}

/**
 * Set a sprite on-screen coordinates.
 *
 * @param i sprite index (0-127)
 */
void sprite_move(uint8 i, uint16 x, uint16 y) {
    uint32 addr = VERA_SPRITE_ATTR_ADDR(i);
    TwoBytes xb, yb;
    xb.s = x;
    yb.s = y;

    vpoke(xb.b.lo, addr + 2);
    vpoke(xb.b.hi, addr + 3);
    vpoke(yb.b.lo, addr + 4);
    vpoke(yb.b.hi, addr + 5);
}

#define PET_CHARSET_UPPER_GRAPH (2)
void __fastcall__ screen_set_charset(const uint8 pet_charset) {
    // The first argument was passed in the A register, which is also the argument
    // to screen_set_charset, so there's no need to load the A register again.
    // See https://cc65.github.io/doc/cc65-intern.html#ss2.1
    //asm("lda %v", pet_charset); // PET upper/graph charset

    asm("jsr $FF62");           // KERNAL routine screen_set_charset
}

/**
 * Take a single-frame screenshot.  x16emu MUST have been started with "-gif"
 * command line option.
 */
void __fastcall__ screenshot() {
    *((char*)0x9FB5) = 1;
}

void rdtim_test() {
    uint16 i = 0;

    while (1) {
        asm("jsr %w", KERNAL_RDTIM);
        asm("phy");
        asm("phx");

        asm("jsr %v", putdxb);  // print A
        asm("lda #141"); CHROUT;// newline

        asm("pla");             // print X
        asm("jsr %v", putdxb);
        asm("lda #141"); CHROUT;// newline

        asm("pla");             // print Y
        asm("jsr %v", putdxb); 
        asm("lda #141"); CHROUT;// newline

        asm("lda #141"); CHROUT;// newline
        while (++i) {}          // wait a bit
    }
}

void draw_stars_naive() {
    uint8 i, c, y;
    uint16 random, x;
    uint32 addr;

    vpoke(4, 319);
    vpoke(4, 0x12bff);
    
    for (i = 0; i < 100; i++) {
        do { x = prng(); } while (x > 319);
        do { y = prng(); } while (y > 239);
        addr = 320*(uint32)y + (uint32)x;
        c = 16 + (prng() & 0b1111); // a greyscale color, index 16-31
        vpoke(c, addr);
    }
}

void draw_stars_by_interval() {
    uint32 addr = 0; // VRAM address (offset into bitmap memory $00000-$12bff)
    uint16 random;
    uint8 color;
    uint16 star_count = 0;

    while (addr < 0x12c00) {
        random = prng();
        color = 16 + (random >> 12); // top 4 bits are color
        addr += random & 0x3ff;      // bottom 10 bits are interval
        vpoke(color, addr);
        star_count++;
    }

    putdxb(star_count);
}

void main() {
    // KLUDGE: cc65 always switches to the lower/upper character set.
    // Put it back!  See https://cc65.github.io/mailarchive/2004-09/4446.html
    // and https://discord.com/channels/547559626024157184/629903553028161566/1227803125818069102
    screen_set_charset(PET_CHARSET_UPPER_GRAPH);
    videomode(VIDEOMODE_320x240);
    bitmap_clear(COLOR_BLACK);

    vera_layer_enable(VERA_LAYER_ALL);
    vera_sprites_enable(TRUE);

    sprite_set_addr(0, BPP_8, 0x13000);
    sprite_move(0, 152, 112);
    vpoke(FRONT,      VERA_SPRITE_ATTR_BASE+6); // coll. mask, z-depth, v- and h-flip
    vpoke(0b01010000, VERA_SPRITE_ATTR_BASE+7); // height, width, palette offset
    // sprite_print_attrs(0);
    load_sprite("earth.bin", (char*)0x3000); // lower-case in source => upper-case on X16

    prng_seed(1);
    draw_stars_by_interval();

    screenshot();
}
