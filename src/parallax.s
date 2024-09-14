.include "nes.inc"

; placed before running nametable updates
.section .nmi.055,"axR",@progbits
.globl update_parallax_tiles
update_parallax_tiles:
  lda NAME_UPD_ENABLE ; don't run parallax updates if we need to update nametables
  bne .LskipParallax
    jsr flush_parallax_update
.LskipParallax:

.section .text.flush_parallax_update,"axR",@progbits
.globl flush_parallax_update
flush_parallax_update:
  lda scroll_diff
  beq .LskipUpdateParallax
  lda #$00
  sta PPUADDR
  lda #$90
  sta PPUADDR
  lda PPUCTRL_VAR
  and #$fb
  sta PPUCTRL
; this is where if this update loop isn't fast enough you can use
; an unrolled loop to make it faster
  ldx #127
.LparallaxLoop:
    lda parallax_buf - 127,x
    sta PPUDATA
    inx
    bmi .LparallaxLoop

  lda PPUCTRL_VAR
  sta PPUCTRL
.LskipUpdateParallax:
  rts
