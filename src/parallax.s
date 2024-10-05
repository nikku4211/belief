; thanks jroweboy
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
  bne 1f
		jmp .LskipUpdateParallax
1:
  lda #$00
  sta PPUADDR
  lda #$B0
  sta PPUADDR
  lda PPUCTRL_VAR
  and #$fb
  sta PPUCTRL
;this is where if this update loop isn't fast enough you can use
;an unrolled loop to make it faster
  ;ldx #127
;.LparallaxLoop:
.rept 96
    lda parallax_buf + \+
    sta PPUDATA
    ;bmi .LparallaxLoop
.endr

  lda PPUCTRL_VAR
  sta PPUCTRL
.LskipUpdateParallax:
  rts
