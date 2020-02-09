#include "avr/io.h"

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n);

#define DATA_PIN PIND0
#define DATA_MASK 0x01

#define WRITE_HIGH PORTB |= 0x01
#define WRITE_LOW PORTB &= 0xFE

volatile byte command = 0x00;
void readCommand();

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
//  DDRB &= ~(1 << 0);
//  PORTB |= (1 << 0);
  
//  pinMode(8, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), readCommand, FALLING);
}

byte arr[] { 0x00, 0x02 };
size_t arrayLen = 2;

void loop() {
//  sendBytes(arr, 2);
//  delay(1000);
if (DATA_PIN)
{
  Serial.println(DATA_PIN);
}
  //while (DATA_PIN&DATA_MASK) { DELAY_CYCLES(16); }
//  Serial.println(DATA_PIN&DATA_MASK, HEX);
  
}

// byte readBit() {
//   byte thisBit = 0x00;

//   // guestimating 3 cycles for each? 1 and, 1 shift, one or compound
//   thisBit |= (DATA_PIN & DATA_MASK) << 3;
//   DELAY_CYCLES(10);

//   thisBit |= (DATA_PIN & DATA_MASK) << 2;
//   DELAY_CYCLES(11);

//   thisBit |= (DATA_PIN & DATA_MASK) << 1;
//   DELAY_CYCLES(12);

//   thisBit |= (DATA_PIN & DATA_MASK);
//   DELAY_CYCLES(13);

//   return thisBit;
// }

// byte readByte() {
//   byte inByte = 0x00;
//   unsigned short bitsLeftToRead = 8;

//   byte thisBit;


//   do {        // 3 cycles when true
//     thisBit = readBit();
//     inByte <<= 1;
//     if(thisBit == 0x07){        // 3 cycles when true
//       ++inByte;
//       DELAY_CYCLES(7);
//     } else if(thisBit == 0x01) {                    // 2 cycles when false
//       DELAY_CYCLES(9);
//     } else {
//       Serial.println(thisBit, BIN);
//     }

//     bitsLeftToRead -= 1;        // 1 cycle
//   } while(bitsLeftToRead);

//   return inByte;
// }

byte readByte() {
  byte out = 0x00;
  int bitsLeftToRead = 8;
  while(bitsLeftToRead--) {
    byte cBit[4] {0x00, 0x00, 0x00, 0x00};
    byte bit = 0x00;
    int i = 0;
    do {
      bit <<= 1;
      bit |= (PIND & DATA_MASK);
    } while(i++ < 4);
    out <<= 1;
    out += (bit == 0b0111);
  }
  return out;
}

const byte n64status[] { 0x05, 0x00, 0x02 };

void readCommand() {
  command = readByte();
  Serial.println(command, HEX);
  switch (command) {
    case 0x00: // STATUS
      sendBytes(n64status, 3);
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
      Serial.println(command, BIN);
      break;
  }
}

void sendBytes(const byte* b, size_t n) {
  Serial.println(b[0], HEX);
  Serial.println(b[1], HEX);
  byte mask;
  while (n!=0) {
    WRITE_LOW; // 2 cycles
    mask = 0b10000000;
    // write start bit
    while (mask!=0) {
      DELAY_CYCLES(4); // 11 cycles
      if ((*b) & mask) { // 1 cycle (AND)
        WRITE_HIGH; // 2 cycles
        mask >>= 1; // 1 cc
        DELAY_CYCLES(33);
      }
      else {
        mask >>= 1; // 1 cc
        DELAY_CYCLES(30); // 31 cycles
        WRITE_HIGH; // 2 cycles
      }
      if (mask!=0) {
        DELAY_CYCLES(10);
        WRITE_LOW; // 2 cycles
        DELAY_CYCLES(2);
      }
    }  // 3 cycles
    ++b;
    --n;
  }
  WRITE_LOW;
  DELAY_CYCLES(30);
  WRITE_HIGH;
}
