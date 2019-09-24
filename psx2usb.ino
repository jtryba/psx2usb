// Simple application to use a psx controller on a pc over usb.
//
// v0.1 - initial relase, only suppors 1 psx controller
// v0.2 - updated relase, supports 2 psx controllers
// v0.3 - updated relase, supports 2 psx controllers, using 2 joystick implimentations
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// by John Tryba
// 9/23/2019
// v0.3
//--------------------------------------------------------------------

#define PSX_LEFT      0
#define PSX_DOWN      1
#define PSX_RIGHT     2
#define PSX_UP        3
#define PSX_START     4
#define PSX_SELECT    5
#define PSX_SQUARE    6
#define PSX_X         7
#define PSX_O         8
#define PSX_TRIANGE   9
#define PSX_R1        10
#define PSX_L1        11
#define PSX_R2        12
#define PSX_L2        13

#define JOYSTICK_COUNT 2
#define BUTTON_COUNT 14

#include <Joystick.h>                                     // Includes the Joystick Library

#include <Psx.h>                                          // Includes the Psx Library 
// Any pins can be used since it is done in software

#define psxDelay 10

#define dataPin0 2
#define cmndPin0 3
#define attPin0 4
#define clockPin0 5
#define ackPinP0 6

#define dataPin1 7
#define cmndPin1 8
#define attPin1 9
#define clockPin1 10
#define ackPinP1 A0

#define debugPin A1

Joystick_ Joystick[JOYSTICK_COUNT] = {
  
  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD,
                   14, 0,                  // Button Count, Hat Switch Count
                   false, false, false,    // No X, Y, and Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false),   // No accelerator, brake, or steering

  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD,
                   14, 0,                  // Button Count, Hat Switch Count
                   false, false, false,    // No X, Y, and Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false)   // No accelerator, brake, or steering
};


Psx Psx0;                                                  // Initializes the library for player 1
Psx Psx1;                                                  // Initializes the library for player 2

byte button[14] = {0, 1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15};

unsigned int data[2];                                    // data stores the controller response
byte debug = 0x00;                                        // data stores the debug value

void setup() {
  Psx0.setupPins(dataPin0, cmndPin0, attPin0, clockPin0, psxDelay); // Defines what each pin is used
  // (Data Pin #, Cmnd Pin #, Att Pin #, Clk Pin #, Delay)
  // Delay measures how long the clock remains at each state,
  // measured in microseconds.
  // too small delay may not work (under 5)

  Psx1.setupPins(dataPin1, cmndPin1, attPin1, clockPin1, psxDelay);

  data[0] = Psx0.read();                                      // Psx.read() initiates the PSX controller and returns
  // the button data

  data[1] = Psx1.read();


  pinMode(ackPinP0, INPUT_PULLUP);                          // ACK Acknowledge signal from Controller to PSX.
  // This signal should go low for at least one clock period
  // after each 8 bits are sent and ATT is still held low.
  // If the ACK signal does not go low within about 60 us
  // the PSX will then start interogating other devices.


  pinMode(ackPinP1, INPUT_PULLUP);

  pinMode(debugPin, INPUT_PULLUP);
  
  if (digitalRead(debugPin) == LOW) {
    debug = 0xFF;
    Serial.begin(115200);
  }
  
  // Initialize Joystick Library
  for (int index = 0; index < JOYSTICK_COUNT; index++) {
    Joystick[index].begin();
  }
}

byte lastButtonState[2][BUTTON_COUNT] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void loop() {

  data[0] = Psx0.read();
  data[1] = Psx1.read();

  if (debug == 0xFF) {
    char buf[64];
    sprintf(buf, "0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d 0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            bitRead(data[0], button[PSX_LEFT]),
            bitRead(data[0], button[PSX_DOWN]),
            bitRead(data[0], button[PSX_RIGHT]),
            bitRead(data[0], button[PSX_UP]),
            bitRead(data[0], button[PSX_START]),
            bitRead(data[0], button[PSX_SELECT]),
            bitRead(data[0], button[PSX_SQUARE]),
            bitRead(data[0], button[PSX_X]),
            bitRead(data[0], button[PSX_O]),
            bitRead(data[0], button[PSX_TRIANGE]),
            bitRead(data[0], button[PSX_R1]),
            bitRead(data[0], button[PSX_L1]),
            bitRead(data[0], button[PSX_R2]),
            bitRead(data[0], button[PSX_L2]),
            bitRead(data[1], button[PSX_LEFT]),
            bitRead(data[1], button[PSX_DOWN]),
            bitRead(data[1], button[PSX_RIGHT]),
            bitRead(data[1], button[PSX_UP]),
            bitRead(data[1], button[PSX_START]),
            bitRead(data[1], button[PSX_SELECT]),
            bitRead(data[1], button[PSX_SQUARE]),
            bitRead(data[1], button[PSX_X]),
            bitRead(data[1], button[PSX_O]),
            bitRead(data[1], button[PSX_TRIANGE]),
            bitRead(data[1], button[PSX_R1]),
            bitRead(data[1], button[PSX_L1]),
            bitRead(data[1], button[PSX_R2]),
            bitRead(data[1], button[PSX_L2])
           );
    Serial.println(buf);                                   // Display the returned value
  }

  // Read psx btn values
  for (int joyIndex = 0; joyIndex < JOYSTICK_COUNT; joyIndex++) {
    
    for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++) {
      byte currentButtonState = bitRead(data[joyIndex], button[buttonIndex]);
      if (currentButtonState != lastButtonState[joyIndex][buttonIndex]) {
        Joystick[joyIndex].setButton(buttonIndex, currentButtonState);
        lastButtonState[joyIndex][buttonIndex] = currentButtonState;
      }
    }
  }

  delay(5);
}
