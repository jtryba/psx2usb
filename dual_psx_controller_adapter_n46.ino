// Simple application to use a psx controller on a pc over usb.
//
// v0.1 - initial relase, only suppors 1 psx controller
// v0.2 - updated relase, supports 2 psx controllers
// v0.3 - updated relase, supports 2 psx controllers, using 2 joystick implimentations
// v0.4 - updated relase, supports 2 psx controllers, and 1 n64 controller, using 3 joystick implimentations
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// by John Tryba
// 9/29/2019
// v0.4
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

#define JOYSTICK_COUNT 3
#define BUTTON_COUNT 14

#include <Joystick.h>                                     // Includes the Joystick Library

#include <N64Controller.h>                                // Includes the N64Controller Library 

#include <Psx.h>                                          // Includes the Psx Library 
// Any pins can be used since it is done in software

#define psxDelay 10

//---PIN-DEFINITIONS---

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

#define n64Pin 2

//---PIN-DEFINITIONS---

Joystick_ Joystick[JOYSTICK_COUNT] = {
  
  Joystick_(0x02, JOYSTICK_TYPE_GAMEPAD,
                   BUTTON_COUNT, 0,        // Button Count, Hat Switch Count
                   false, false, false,    // No X, Y, and Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false),   // No accelerator, brake, or steering

  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD,
                   BUTTON_COUNT, 0,        // Button Count, Hat Switch Count
                   false, false, false,    // No X, Y, and Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false),   // No accelerator, brake, or steering

  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD,
                   BUTTON_COUNT, 0,        // Button Count, Hat Switch Count
                   true, true, false,      // X, and Y, but no Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false)    // No accelerator, brake, or steering
};

Psx Psx0;                                                  // Initializes the library for player 1
Psx Psx1;                                                  // Initializes the library for player 2
N64Controller n64con(n64Pin);                              // Initializes the library for player 3

byte button[14] = {0, 1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15};

unsigned int data[JOYSTICK_COUNT];                       // data stores the controller response
byte debug = 0x00;                                       // data stores the debug value

void setup() {
  Psx0.setupPins(dataPin0, cmndPin0, attPin0, clockPin0, psxDelay); // Defines what each pin is used
  // (Data Pin #, Cmnd Pin #, Att Pin #, Clk Pin #, Delay)
  // Delay measures how long the clock remains at each state,
  // measured in microseconds.
  // too small delay may not work (under 5)

  Psx1.setupPins(dataPin1, cmndPin1, attPin1, clockPin1, psxDelay);

  n64con.begin();

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
    if (index == 2) {
      Joystick[index].setXAxisRange(-127, 127);
      Joystick[index].setYAxisRange(-127, 127);
    }
    Joystick[index].begin();
  }
}

byte lastButtonState[JOYSTICK_COUNT][BUTTON_COUNT] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int n64xAxis = 0;
int n64yAxis = 0;

unsigned int ReadN64() {
  
  unsigned int ret;
  memset(ret, 0, 16);
  
  n64con.update();

  n64xAxis = n64con.axis_x();
  n64yAxis = n64con.axis_y();
  
  char n54data[16];
  memset(n54data, 0, 16);
  
  sprintf(n54data, "0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
    n64con.A(),
    n64con.B(),
    n64con.Z(),
    n64con.Start(),
    n64con.D_up(),
    n64con.D_down(),
    n64con.D_left(),
    n64con.D_right(),
    n64con.L(),
    n64con.R(),
    n64con.C_up(),
    n64con.C_down(),
    n64con.C_left(),
    n64con.C_right()
  );
  ret = atoi(n54data);

  return ret;
}

void loop() {

  data[0] = Psx0.read();
  data[1] = Psx1.read();
  data[2] = ReadN64();

  if (debug == 0xFF) {
    char buf[64];
    sprintf(buf, "0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d 0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d  0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d[%d][%d]",
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
            bitRead(data[1], button[PSX_L2]),
            
            bitRead(data[2], A_IDX),
            bitRead(data[2], B_IDX),
            bitRead(data[2], Z_IDX),
            bitRead(data[2], START_IDX),
            bitRead(data[2], D_UP_IDX),
            bitRead(data[2], D_DOWN_IDX),
            bitRead(data[2], D_LEFT_IDX),
            bitRead(data[2], D_RIGHT_IDX),
            bitRead(data[2], L_IDX),
            bitRead(data[2], R_IDX),
            bitRead(data[2], C_UP_IDX),
            bitRead(data[2], C_DOWN_IDX),
            bitRead(data[2], C_LEFT_IDX),
            bitRead(data[2], C_RIGHT_IDX),
            n64xAxis,
            n64yAxis
           );
    Serial.println(buf);                                   // Display the returned value
  }

  // Read psx btn values
  for (int joyIndex = 0; joyIndex < JOYSTICK_COUNT; joyIndex++) {
    if (joyIndex == 2) {
        Joystick[joyIndex].setXAxis(n64xAxis);
        Joystick[joyIndex].setYAxis(n64yAxis);
    }
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
