#include "avr/io.h"

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n);

#define DATA_PIN DDRB
#define DATA_MASK 0x01

#define WRITE_HIGH DATA_PIN &= 0xFE
#define WRITE_LOW DATA_PIN |= 0x01

byte command = 0x00;
void readCommand();
void sendByte(byte b);

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
  pinMode(DATA_PIN, INPUT);
}


void loop() {
  while (!(DATA_PIN & 0x01));
  readCommand();
}

byte readBit() {
  byte thisBit = 0x00;

  // guestimating 3 cycles for each? 1 and, 1 shift, one or compound
  thisBit |= (DATA_PIN & DATA_MASK) << 3;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK) << 2;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK) << 1;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK);

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
      // sendByte(0x05);
      // sendByte(0x00);
      // sendByte(bitFlags);
      Serial.println("Got Status");
      break;
    case 0x02: // POLL
      // send button and stick state
      Serial.println("Got Poll");
      break;
    case 0x03: // READ
      Serial.println("Got Read");
      break;
    case 0x04: // WRITE
      Serial.println("Got Write");
      break;
    default:
      // some kind of error handling?
      Serial.print("Got Something Else: ");
      Serial.println(command, HEX);
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
