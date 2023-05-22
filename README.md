# simpleIRreceiver
A simple IR reciver library for Arduino projects, inspired from several more complex libraries.

Several years ago, I was searching for a mean to interface an IR remote control unit to an arduino "smart" (but unconnected) RGB LED-strip lighting fixture.

But all of the libraries I found online were too complicated for just receiving an IR code, so I wrote mine based on the simplest one I found online.

It receives pulses from an IR remote sensor like this one: https://learn.adafruit.com/ir-sensor

and reliably generates a binary code corresponding to each button.

*  Exemple of usage:
```C
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
```
