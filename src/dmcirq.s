;thanks nesdev wiki and jroweboy
.include "nes.inc"

.section .init.100,"axR",@progbits
	lda #$40	;disable APU frame counter
	sta APU_PAD2

.section .dpcm.000,"axR",@progbits
.globl dmc_tiny_dummy
  .byte $00,$00,$00,$00,$00,$00,$00,$00	;dummy DPCM sample for visual timing
	.byte $00

.section .nmi.005,"axR",@progbits
.globl start_dummy_sample
	lda #0
	sta dmc_irq_measure_flag
	ldx #88
start_dummy_sample:
	lda #$8F			;play dummy sample at rate $F to find out
	sta APU_MODCTRL
	lda #__dpcm_offset ;how long it takes until an IRQ happens
	sta APU_MODADDR
	lda #0
	sta APU_MODLEN
	sei
	lda #$10 
	sta APU_CHANCTRL
	sta APU_CHANCTRL
	sta APU_CHANCTRL
	cli
	
.globl where_in_the_period
where_in_the_period:
	dex	;2 cycles
	bne where_in_the_period		;poll loop to measure the duration before an IRQ, also 2 cycles
	; 2 + 2 = 4 cycles
	
.section .prg_rom_fixed.irq,"axR",@progbits
.globl irq
irq:
	pha
	tya
	pha
	txa
	pha
	
	lda #$00
	sta APU_MODCTRL
	
	lda dmc_irq_measure_flag ;if this isn't the first IRQ in frame, skip this part
	bne irq1
	lda #1
	sta dmc_irq_measure_flag
.globl start_main_sample
start_main_sample:
	lda #$8f	;okay, now play the dummy DPCM sample at rate $0
	sta APU_MODCTRL
	lda #__dpcm_offset
	sta APU_MODADDR
	lda #1
	sta APU_MODLEN
	lda #$10 
	sta APU_CHANCTRL
	sta APU_CHANCTRL
	sta APU_CHANCTRL
	
	stx dmc_irq_measure	;store IRQ measure
	ldx #1
irq_done:
  pla
	tax
	pla
	tay
	pla
	rti
	
irq1:
	lda #$80	;okay, now play the dummy DPCM sample at rate $0
	sta APU_MODCTRL
	lda #0
	sta dmc_irq_measure_flag
	lda #108
	sec
	sbc dmc_irq_measure
	tax

irq_sync_wait:
	dex ;2 cycles
	bne irq_sync_wait ;2 cycles
irq_split_effect:
	lda scroll_x	;thanks nesdev wiki
	eor old_scroll_x
	and #%11111000
	eor old_scroll_x
	sta PPUSCROLL
	bit PPUSTATUS
	lda scroll_x
	sta old_scroll_x
	nop
	nop
	sta PPUSCROLL
	bit PPUSTATUS
main_irq_done:
	pla
	tax
	pla
	tay
	pla
	rti
