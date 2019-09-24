// Simple application to use a psx controller on a pc over usb.
//
// v0.1 - initial relase, only suppors 1 psx controller
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// by John Tryba
// 9/19/2019
// v0.1
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

#include <Joystick.h>                                     // Includes the Joystick Library

#include <Psx.h>                                          // Includes the Psx Library 
// Any pins can be used since it is done in software

#define psxDelay 10

#define dataPin0 2
#define cmndPin0 3
#define attPin0 4
#define clockPin0 5
#define ackPinP0 6

#define debugPin A1

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   14, 0,                  // Button Count, Hat Switch Count
                   false, false, false,    // No X, Y, and Z Axis
                   false, false, false,    // No Rx, Ry, or Rz
                   false, false,           // No rudder or throttle
                   false, false, false);   // No accelerator, brake, or steering



Psx Psx0;                                                  // Initializes the library for player 1

byte button[14] = {0, 1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 15};

unsigned int data0 = 0;                                   // data stores the controller response
byte debug = 0x00;                                        // data stores the debug value

void setup() {
  Psx0.setupPins(dataPin0, cmndPin0, attPin0, clockPin0, psxDelay); // Defines what each pin is used
  // (Data Pin #, Cmnd Pin #, Att Pin #, Clk Pin #, Delay)
  // Delay measures how long the clock remains at each state,
  // measured in microseconds.
  // too small delay may not work (under 5)

  data0 = Psx0.read();                                      // Psx.read() initiates the PSX controller and returns
  // the button data


  pinMode(ackPinP0, INPUT_PULLUP);                          // ACK Acknowledge signal from Controller to PSX.
  // This signal should go low for at least one clock period
  // after each 8 bits are sent and ATT is still held low.
  // If the ACK signal does not go low within about 60 us
  // the PSX will then start interogating other devices.

  pinMode(debugPin, INPUT_PULLUP);
  
  if (digitalRead(debugPin) == LOW) {
    debug = 0xFF;
    Serial.begin(115200);
  }
  
  // Initialize Joystick Library
  Joystick.begin();

}

byte lastButtonState[14] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void loop() {

  data0 = Psx0.read();

  if (debug == 0xFF) {
    char buf[32];
    sprintf(buf, "0x%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            bitRead(data0, button[PSX_LEFT]),
            bitRead(data0, button[PSX_DOWN]),
            bitRead(data0, button[PSX_RIGHT]),
            bitRead(data0, button[PSX_UP]),
            bitRead(data0, button[PSX_START]),
            bitRead(data0, button[PSX_SELECT]),
            bitRead(data0, button[PSX_SQUARE]),
            bitRead(data0, button[PSX_X]),
            bitRead(data0, button[PSX_O]),
            bitRead(data0, button[PSX_TRIANGE]),
            bitRead(data0, button[PSX_R1]),
            bitRead(data0, button[PSX_L1]),
            bitRead(data0, button[PSX_R2]),
            bitRead(data0, button[PSX_L2])
           );
    Serial.println(buf);                                   // Display the returned value
  }

  // Read psx btn values
  for (int index = 0; index < 14; index++)
  {
    byte currentButtonState = bitRead(data0, button[index]);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  delay(5);
}
