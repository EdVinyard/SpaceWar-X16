#include <string.h>
#include <cx16.h>
#include <stdio.h>

#define ushort unsigned short

/** https://www.pagetable.com/c64ref/kernal/#LOAD */
#define KERNAL_LOAD (0xFFD5)
/** https://www.pagetable.com/c64ref/kernal/#SETLFS */
#define KERNAL_SETLFS (0xFFBA)
/** https://www.pagetable.com/c64ref/kernal/#SETNAM */
#define KERNAL_SETNAM (0xFFBD)

#define BPP_4 (0b00000000)
#define BPP_8 (0b10000000)
#define FRONT (0b00001100)
#define HEIGHT_8 (0)
#define WIDTH_8 (0)
#define OFFSET_0 (0)

/**
 * https://github.com/X16Community/x16-docs/blob/master/X16%20Reference%20-%2009%20-%20VERA%20Programmer's%20Reference.md#sprite-attributes
 */
struct VeraSpriteAttr {
    /** low 8 bits of sprite data address */
    unsigned char address_lo;
    /**
     * bit 7 :(0 means 4 bit-per-pixel
     * bit 3-0: high bits of sprite data address
     */
    unsigned char mode_and_address_hi;
    /** low 8 bits of X-coordinate */
    unsigned char x_lo;
    /** high 2 bits of X-coordinate */
    unsigned char x_hi;
    /** low 8 bits of Y-coordinate */
    unsigned char y_lo;
    /** high 2 bits of Y-coordinate */
    unsigned char y_hi;
    /**
     * bit 7-4: collision mask
     * bit 3-2: Z-depth (0 => disabled, 3 => in front, see docs for others)
     * bit 1:   vertical flip
     * bit 0:   horizonal flip
     */
    unsigned char collision_mask_z_depth_vflip_hflip;
    /**
     * bit 7-6: sprite height (0 => 8px, 1 => 16px, 2 => 32px, 3 => 64px)
     * bit 5-4: sprite width (same values as height)
     * bit 3-0: palette offset
     */
    unsigned char height_width_palette_offset;
};
#define VERA_SPRITE_ATTR (0xFC00)

/** 
 * CHROUT kernal routine (AKA, "BSOUT") that prints a single character at the
 * current cursor location and advances the cursor. 
 *
 * See https://www.pagetable.com/c64ref/kernal/#BSOUT for detailed information
 * about the equivalent Commodore 64 KERNAL routine.
 */
#define KERNAL_CHROUT (0xFFD2)
#define CHROUT (asm("jsr %w", KERNAL_CHROUT))

void print_sprite_0() {
    char r, c;
    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            printf("%d ", vpeek(0x13000 + (8*r) + c));
        }

        putchar('\n');
    }

    putchar('\n');
}

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
    const register char zp_filename_len = strlen(filename);
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

void main() {
    vera_layer_enable(0b11);
    vera_sprites_enable(1);

    // STARTHERE 
    //
    // sprite data seems to be loaded into VRAM correctly, but it does not
    // appear (BASIC commands can be used to configure the sprite, but the
    // following code doesn't work.)
    vpoke(0x00,         VERA_SPRITE_ATTR+0); // addr lo
    vpoke(BPP_8 | 0x30, VERA_SPRITE_ATTR+1); // mode, addr hi
    vpoke(0,            VERA_SPRITE_ATTR+2); // x-coord lo (256)
    vpoke(1,            VERA_SPRITE_ATTR+3); // x-coord hi
    vpoke(10,           VERA_SPRITE_ATTR+4); // y-coord lo (10)
    vpoke(0,            VERA_SPRITE_ATTR+5); // y-coord hi
    vpoke(FRONT,        VERA_SPRITE_ATTR+6); // coll mask, z-depth, v- and h-flip
    vpoke(0,            VERA_SPRITE_ATTR+7); // height, width, palette offset

    print_sprite_0();
    load_sprite("spritex.bin", (char*)0x3000); // BEWARE: with alt chars, case is swapped!
    print_sprite_0();

    // Take a single-frame screenshot; assumes x16emu was started with "-gif"
    // command line option.
    *((char*)0x9FB5) = 1;
}
