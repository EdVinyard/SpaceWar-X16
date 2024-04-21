;--------------------------------------------------------------------
; an XOR-shift Linear-Feedback Shift Register pseudo-random number generator for
; the Commander X16 and the cc65 family of tools implemented from
; http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html?showComment=1557753115362#c6700504611821379366
; which is a clever 6502 port of the Z80 code posted in
; http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
; which implements the algorithm originally devised by George Marsaglia
;--------------------------------------------------------------------
.export _prng_seed
.export _prng_seed_clock
.export _prng

state_lo = $7e      ; state (low byte) in zero-page
state_hi = $7f      ; state (high byte) in zero-page

KERNAL_RDTIM = $ffde

;--------------------------------------------------------------------
_prng_seed:
; uses cc65 'fastcall' protocol
; input A-register low byte of 16-bit seed value
; input X-register high byte of 16-bit seed value
; no output
;--------------------------------------------------------------------
    sta state_lo            ; state = seed
    stx state_hi

    ora state_hi            ; if (0 == state)
    bne @nonzero_seed_value
    lda #1                  ;     state = 1;
    sta state_lo
@nonzero_seed_value:

    rts

;--------------------------------------------------------------------
_prng_seed_clock:
; no input
; no output
;--------------------------------------------------------------------
    ; There's conflicting documentation for RDTIME for C64
    ; (https://www.pagetable.com/c64ref/kernal/#RDTIM).  However, on the X16, I
    ; believe the returned values are such that
    ;   - A contains the low-order byte, 
    ;   - X the middle-order byte, and 
    ;   - Y the high-order byte.
@zero_seed_value:      ; do {
    jsr KERNAL_RDTIM   ;    A,X,Y = KERNAL_RDTIME()
    sta state_lo       ;    state_lo = A
    stx state_hi       ;    state_hi = X
                       ;    // discard hi-order byte in Y
    ora state_hi       ; } while (0 == state);
    beq @zero_seed_value
    rts

;--------------------------------------------------------------------
_prng:
; uses cc65 'fastcall' protocol
; no input
; output A random byte
; output X random byte
;--------------------------------------------------------------------
    ; uint16 s;    // state
    ; s ^= s << 7;
    ; s ^= s >> 9;
    ; s ^= s << 8;
    ; return s;

    lda state_hi
    lsr
    lda state_lo
    ror
    eor state_hi
    ; lda #1        ; INTENTIONAL BUG FOR TESTING THE UNIT TESTS
    sta state_hi    ; high byte of x ^= x << 7 done
    ror             ; A has now x >> 9 and high bit comes from low byte
    eor state_lo
    sta state_lo    ; x ^= x >> 9 and the low part of x ^= x << 7 done
    eor state_hi
    sta state_hi    ; x ^= x << 8 done

    ; return A,X;
    tax             ; X = "high" byte
    lda state_lo    ; A = "low" byte
    rts
