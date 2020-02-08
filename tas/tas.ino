#include "avr/io.h"
#include "pins_arduino.h"

#define DELAY_1US asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_2US asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_3US asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_4US asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")

#define DELAY_11CC asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_14CC asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_18CC asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_30CC asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define DELAY_31CC asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n")
#define D22 PA0

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n);

#define WRITE_HIGH DDRD &= 0xFE
#define WRITE_LOW DDRD |= 0x01

byte command = 0x00;
void readCommand();
void sendByte(byte b);

void setup() {
  Serial.begin(9600);
  digitalWrite(DATA_PIN, LOW);
  pinMode(DATA_PIN, INPUT); 
}


void loop() {
  while (!DATA_PIN);
  readCommand();
  
}

void readCommand() {
  // replace with read
  command = readByte();
  switch (command) {
    case 0x00: // STATUS
      sendByte(0x05);
      sendByte(0x00);
      // sendByte(bitFlags);
      break;
    case 0x02: // POLL
      // send button and stick state
      break;
    case 0x03: // READ
      break;
    case 0x04: // WRITE
      break;
    default:
      // some kind of error handling?
      break;
  }
}

void sendByte(byte b) {
  byte remainingBits = 8;
  byte mask = 0b01111111;
  // write start bit
  WRITE_LOW; // 2 cycles
  DELAY_CYCLES(11); // 11 cycles
  do {
    // If bit is a 1
    if (b & mask) { // 1 cycle (AND)
      // true -> 2 cycles (BREQ)
      WRITE_HIGH; // 2 cycles
      // Subtotal: 11 + 1 + 2 + 2 = 16 CC
      DELAY_CYCLES(30);
      // Subtotal = 16 + 30 = 46 cc
    }
    else {
      // false -> 1 cycle (BREQ)
      DELAY_CYCLES(31); // 31 cycles
      WRITE_HIGH; // 2 cycles
      // Subtotal: 11 + 1 + 1 + 31 + 2 = 46 cc
    }
    remainingBits--; // 1 cycle
    if (remainingBits==0) { // 2 cc if true
    } else { // 1 cc if false
      mask >>= 1; // 1 cc
    }
  } while (remainingBits); // 3 cycles
}
