// include guard
#ifndef __IR_H_INCLUDED__
#define __IR_H_INCLUDED__

/* License: GNU GPLv3.
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * *****
 * 
 * Copyright @ 2019 Denis Chupau. All rights reserved.
 * 
 * *****
 * 
 * Small simple receiver library:
 *   it receives pulses from an IR remote sensor like this one: https://learn.adafruit.com/ir-sensor
 *   and reliably generates a binary code corresponding to each button.
 *  Exemple of usage:
// To be added in YOUR program (you must define IR_PIN and IR_INT beforehand):
void setup() {
  // setup IR reception
  IRsetup(IR_PIN, IR_INT);
}
void main() {
  // checking for IR message
  uint32_t ir_message = IRcheck();
  if (ir_message) {
    switch (ir_message) {
      case(ONOFF) : Serial.println("Button ONOFF pressed"); break;
      default: break;
    }
  }
}
* 
 */


// IR
//#define IRDEBUG  1     // if you want the IR code on the Serial console: uncomment

// After this much time has elapsed after the last transition, consider the message complete.
const uint16_t IRMSG_COMMIT_TIME_MICROSEC = 30000;
volatile unsigned long timeToCommitIRMessage;    // Time after which we should consider the message complete.
volatile unsigned long timeOfLastIRPinChange;  // Time when the last pin change happened on the IR decoder.
volatile uint32_t ir_message_buffer = 0;       // Space where IR message is built

struct irFlags_t {
  union {
    uint8_t value;
    struct {
      unsigned busy:1;    // What our protocol decoder is doing.
      unsigned pin:4;     // IR input pin, must be an "interrupt" pin
      unsigned inter:3;   // IR interrupt associated to the pin above
    };
  };
} __attribute__((packed));
volatile struct irFlags_t irFlags;


// remote keys / values
enum irKeys_t: uint32_t {
  IRKONOFF    = 0x659A38C7,  //
  IRKFUNCTION = 0x659A9966,
  IRKVOLMINUS = 0x659AD02F,
  IRKVOLPLUS  = 0x659A50AF,
  IRKWAKEUP   = 0x659A738C,
  IRKSLEEP    = 0x659A9867,
  IRKSFCMODE  = 0x659A5BA4,
  IRKTUNBAND  = 0x25DAC837,
  IRKNUMBER1  = 0x659A00FF,
  IRKNUMBER2  = 0x659A807F,
  IRKNUMBER3  = 0x659A40BF,
  IRKNUMBER4  = 0x659AC03F,
  IRKNUMBER5  = 0x659A20DF,
  IRKNUMBER6  = 0x659AA05F,
  IRKNUMBER7  = 0x659A609F,
  IRKNUMBER8  = 0x659AE01F,
  IRKNUMBER9  = 0x659A10EF,
  IRKNUMBER0  = 0x659A906F,
  IRKNUMBSUP  = 0x659A02FD,
  IRKNUMBCLR  = 0x659A827D,
  IRKCDRND    = 0x45BA52AD,
  IRKCDDISC   = 0x45BAB847,
  IRKCDREP    = 0x45BA30CF,
  IRKCDPGM    = 0x45BAB04F,
  IRKCDFRWD   = 0x45BA8877,
  IRKCDFFWD   = 0x45BA08F7,
  IRKCDPLAY   = 0x45BAE817,
  IRKCDSTOP   = 0x45BA6897,
  IRKCDPAUSE  = 0x45BA18E7,
  IRKTAPENRG  = 0x857A28D7,  // enreg
  IRKTAPSTOP  = 0x857A6897,  // stop
  IRKTAPFFWD  = 0x857A08F7,  // Fast forward >>
  IRKTAPFRWD  = 0x857A8877,  // Fast rewind <<
  IRKTAPPREV  = 0x857AA857,  // <
  IRKTAPNEXT  = 0x857AE817,  // >
  IRKTAPSTOP2 = 0x857A3AC5,  // stop
  IRKTAPFFWD2 = 0x857A6A95,  // Fast forward >>
  IRKTAPFRWD2 = 0x857AEA15,  // Fast rewind <<
  IRKTAPPREV2 = 0x857ADA25,  // <
  IRKTAPNEXT2 = 0x857ABA45  // >
};


// checks if some IR signal was received
uint32_t IRcheck() {
    // Check to see if we can commit a message. If so, process it.
  if (micros() >= timeToCommitIRMessage && irFlags.busy)
  {
#ifdef IRDEBUG
    Serial.println(ir_message_buffer, HEX);
#endif
    irFlags.busy = false;
    return ir_message_buffer;
  } else {
    return 0;
  }
}



bool getIRBusy(){
  if (irFlags.busy) return true;
  else return false;
}

// IR signal recieving
void IR_ISR() {
  // Interrupt on IR pin: FALLING edge.
  long currentTime = micros();
  long microsSinceLastChange = (currentTime - timeOfLastIRPinChange);
  timeOfLastIRPinChange = currentTime;

  // Bump up the commit time, since we got a level change.
  timeToCommitIRMessage = IRMSG_COMMIT_TIME_MICROSEC + timeOfLastIRPinChange;

  if (microsSinceLastChange < 1200)
  {
    // Space
    ir_message_buffer = ir_message_buffer << 1;
  }
  else if (microsSinceLastChange < 2500)
  {
    // Mark.
    ir_message_buffer = ir_message_buffer << 1;
    ir_message_buffer |= 1;
  }
  else if (microsSinceLastChange < IRMSG_COMMIT_TIME_MICROSEC)
  {
    // First LOW.
    ir_message_buffer = 0;
    irFlags.busy = true;
  }

}

// IR setup, call it in "setup()"
void IRsetup(byte irPin, byte irInt) {
  irFlags.value = 0;
  irFlags.pin = irPin;
  irFlags.inter = irInt;
  pinMode(irFlags.pin, INPUT);
  attachInterrupt(irFlags.inter, IR_ISR, FALLING);
  timeOfLastIRPinChange = micros();
  timeToCommitIRMessage = -1;
  irFlags.busy = false;
}

#endif // __IR_H_INCLUDED__
