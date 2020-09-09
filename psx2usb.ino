// Simple application to use a psx controller on a pc over usb.
//
// v0.1 - initial relase, only suppors 1 psx controller
// v0.2 - updated relase, supports 2 psx controllers
// v0.3 - updated relase, supports 2 psx controllers, using 2 joystick implimentations
// v0.4 - added psx port pinout, changed pins for new case, and cleaned up code a bit

// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.

// by John Tryba
// 9/8/2020
// v0.4
//--------------------------------------------------------------------

#include <Joystick.h>  // Includes the Joystick Library
#include <Psx.h>       // Includes the Psx Library 

#define DEBUG_PIN      A3
#define JOYSTICK_COUNT 2
#define BUTTON_COUNT   14
#define PSX_DELAY      10
#define PSX_LEFT       0
#define PSX_DOWN       1
#define PSX_RIGHT      2
#define PSX_UP         3
#define PSX_START      4
#define PSX_SELECT     5
#define PSX_SQUARE     6
#define PSX_X          7
#define PSX_O          8
#define PSX_TRIANGE    9
#define PSX_R1         10
#define PSX_L1         11
#define PSX_R2         12
#define PSX_L2         13

// (controller plug front)
//   123 456 789
// _______________
// \ 000 000 000 /
//  -------------
// 1-DATA
// 2-CMD
// 3-N/C
// 4-GND
// 5-VCC (+5v)
// 6-ATT
// 7-Clock
// 8-N/C
// 9-ACK (Not Used here)
// Any pins can be used since it is done in software

// player 1
#define DATA0  9
#define CMD0  8
#define ATT0   7
#define CLOCK0 6
#define ACK0  5

// player 2
#define DATA1  14
#define CMD1  16
#define ATT1   A1
#define CLOCK1 15
#define ACK1  10

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

Psx Psx[JOYSTICK_COUNT];
byte debug = 0x00;
unsigned int data[JOYSTICK_COUNT];
byte lastButtonState[JOYSTICK_COUNT][BUTTON_COUNT];

void setup() {
  Psx[0].setupPins(DATA0, CMD0, ATT0, CLOCK0, PSX_DELAY); // Defines what each pin is used
  // (Data Pin #, Cmnd Pin #, Att Pin #, Clk Pin #, Delay)
  // Delay measures how long the clock remains at each state,
  // measured in microseconds.
  // too small delay may not work (under 5)
  Psx[1].setupPins(DATA1, CMD1, ATT1, CLOCK1, PSX_DELAY);

  pinMode(ACK0, INPUT_PULLUP); // ACK Acknowledge signal from Controller to PSX.
  // This signal should go low for at least one clock period
  // after each 8 bits are sent and ATT is still held low.
  // If the ACK signal does not go low within about 60 us
  // the PSX will then start interogating other devices.
  pinMode(ACK1, INPUT_PULLUP);

  pinMode(DEBUG_PIN, INPUT_PULLUP); // sync to ground to enable debug printing
  if (digitalRead(DEBUG_PIN) == LOW) {
    debug = 0xFF;
    Serial.begin(115200);
  }
  
  // Initialize Joystick Library
  for (int index = 0; index < JOYSTICK_COUNT; index++) {
    Joystick[index].begin();
  }
}

void loop() {
  for (int joyIndex = 0; joyIndex < JOYSTICK_COUNT; joyIndex++) {
    data[joyIndex] = Psx[joyIndex].read();
    for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++) {
      byte currentButtonState = bitRead(data[joyIndex], buttonIndex);
      if (currentButtonState != lastButtonState[joyIndex][buttonIndex]) {
        Joystick[joyIndex].setButton(buttonIndex, currentButtonState);
        lastButtonState[joyIndex][buttonIndex] = currentButtonState;
      }
    }
  }
  if (!DebugPrint())
    delay(5);
}

bool DebugPrint()
{
  if (debug == 0xFF) {
    char buf[64];
    sprintf(buf, "0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d 0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            bitRead(data[0], PSX_LEFT),
            bitRead(data[0], PSX_DOWN),
            bitRead(data[0], PSX_RIGHT),
            bitRead(data[0], PSX_UP),
            bitRead(data[0], PSX_START),
            bitRead(data[0], PSX_SELECT),
            bitRead(data[0], PSX_SQUARE),
            bitRead(data[0], PSX_X),
            bitRead(data[0], PSX_O),
            bitRead(data[0], PSX_TRIANGE),
            bitRead(data[0], PSX_R1),
            bitRead(data[0], PSX_L1),
            bitRead(data[0], PSX_R2),
            bitRead(data[0], PSX_L2),
            bitRead(data[1], PSX_LEFT),
            bitRead(data[1], PSX_DOWN),
            bitRead(data[1], PSX_RIGHT),
            bitRead(data[1], PSX_UP),
            bitRead(data[1], PSX_START),
            bitRead(data[1], PSX_SELECT),
            bitRead(data[1], PSX_SQUARE),
            bitRead(data[1], PSX_X),
            bitRead(data[1], PSX_O),
            bitRead(data[1], PSX_TRIANGE),
            bitRead(data[1], PSX_R1),
            bitRead(data[1], PSX_L1),
            bitRead(data[1], PSX_R2),
            bitRead(data[1], PSX_L2)
           );
    Serial.println(buf);
    return true;
  }
  return false;
}
