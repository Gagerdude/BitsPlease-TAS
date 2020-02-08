; Blink LED on PB5(Arduino Uno pin 13)
; http://forum.arduino.cc/index.php?topic=159572#msg1194604

#define __SFR_OFFSET 0

;#include "avr/io.h"

;.macro DELAY_1_US
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;  nop
;.endmacro
;.macro DELAY_2_US
;  DELAY_1_US
;  DELAY_1_US
;.endmacro
;.macro DELAY_3_US
;  DELAY_2_US
;  DELAY_1_US
;.endmacro
;.macro DELAY_4_US
;  DELAY_3_US
;  DELAY_1_US
;.endmacro

/*.global start
.global blink
.global IService
*/
//.section .text 
.org 0x0000 ; Address is zero
    JMP start ; The reset-vector on Address 0000
.org 0x0001
    JMP IService

//.section .text
start:
  LDI   R16, high(RAMEND) ; Upper byte
  OUT   SPH,R16 ; to stack pointer
  LDI   R16, low(RAMEND) ; Lower byte
  OUT   SPL,R16 ; to stack pointer
  sbi   DDRB,5  ; Set PB5 as output
  sei
  reti

blink:
  ldi   r20,250 ; Set the delay duration in ms. Maximum value is 255.
  sbi   PORTB,5 ; Set PB5 HIGH
  ldi   r20,250
  cbi   PORTB,5 ; Set PB5 LOW
  ret

IService: ; Here we start with the Interrupt-Service-Routine
    push R16 ; save a register to stack
    in R16,SREG ; read status register
    call blink ; do something
    out SREG,R16 ; restore old status
    reti ; and return from int
