#include "avr/io.h"

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n);

#define DATA_PIN DDRD

#define WRITE_HIGH DATA_PIN &= 0xFE
#define WRITE_LOW DATA_PIN |= 0x01

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

byte readBit() {
  byte thisBit = 0x00;
  byte thisBitMask = 0x01;

  // guestimating 3 cycles for each? 1 and, 1 shift, one or compound
  thisBit |= (DATA_PIN & thisBitMask) << 3;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & thisBitMask) << 2;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & thisBitMask) << 1;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & thisBitMask);

  return thisBit;
}

byte readByte() {
  byte inByte = 0x00;
  short bitsLeftToRead = 8;

  while(bitsLeftToRead){        // 3 cycles when true
    bitsLeftToRead -= 1;        // 1 cycle
    byte thisBit = readBit();

    if(thisBit == 0x07){        // 3 cycles when true
      inByte |= (0x01 << bitsLeftToRead); // 2 cycles
      DELAY_CYCLES(6);
    } else {                    // 2 cycles when false
      DELAY_CYCLES(10);
    }
  }

  return inByte;
}

void readCommand() {
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
