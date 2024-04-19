.export _bitmap_clear

;
; Fill the entire bitmap layer with a single color.
; @param A-register color prefer COLOR_* definitions (e.g., COLOR_BLACK, COLOR_WHITE, etc.)
;
_bitmap_clear:
    ; Set up the VERA registers for fast, sequential writes starting at the
    ; beginning of the bitmap VRAM area.  Do this as inline assembly to
    ; preserve the value of the A-register as the color parameter.

    ; *((char*)0x9F20) = 0;   ; VRAM Address (bits 7:0)
    ldx #0
    stx $9f20

    ; *((char*)0x9F21) = 0;   ; VRAM Address (bits 15:8)
    stx $9f21

    ; *((char*)0x9F22) = 0b00010000; ; Addr. Inc.(4) ...(3) VRAM Addr. bit 16 (1)
    ldx #%00010000
    stx $9f22

    ; Execute the "poke" to VRAM 76,800 times (320 * 240).
    ; 76,800 = 4 (inner loop unroll) * 256 (X-reg. inner loop) * 75 (Y-reg. outer loop)

    ; lda %v", color);; first (color) argument is already in the A-register
    ldy #75         ; Y-register outer loop counter (counting down)
    ldx #0          ; X-register inner loop counter (counting up)

@loop:
    sta $9f23       ; VRAM data port 0
    sta $9f23       ; VRAM data port 0
    sta $9f23       ; VRAM data port 0
    sta $9f23       ; VRAM data port 0

    inx             ; sets Z status when X rolls over to zero
    bne @loop

    dey             ; sets Z status when Y counts down to zero
    bne @loop

    rts
