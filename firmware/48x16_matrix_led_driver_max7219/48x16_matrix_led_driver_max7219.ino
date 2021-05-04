#include "LedControl.h"
#include <SD.h>

/*
  LedControl device set 1 pins
  D2 pin is connected to the DataIn (1)
  D1 pin is connected to the CLK (13)
  D0 pin is connected to LOAD (12)

  LedControl device set 1 pins
  D3 pin is connected to the DataIn (1)
  Rx pin is connected to the CLK (13)
  D4 pin is connected to LOAD (12)

  SD Card pins
  D8 pin is connected to SD card CS
  D7 pin is connected to SD card MOSI
  D6 pin is connected to SD card MISO
  D5 pin is connected to SD card SCK
*/

#define DATA_IN_1 D2
#define CLK_1 D1
#define LOAD_1  D0
#define DATA_IN_2 D3
#define CLK_2 3
#define LOAD_2  D4
#define DEVICES 6
#define CS_PIN  D8
#define COMMAND_PIN 1

byte frameData[96];
int designCount = 0;
int y = 0;
int delayTime = 10;
unsigned long timer;
boolean isReady = false;
boolean runIndecator = false;
LedControl deviceSet1 = LedControl(DATA_IN_1, CLK_1, LOAD_1, DEVICES);
LedControl deviceSet2 = LedControl(DATA_IN_2, CLK_2, LOAD_2, DEVICES);
File dataFile;

const byte design[12][96] PROGMEM = {
  {254, 231, 127, 254, 231, 127, 0, 0, 0, 0, 0, 0, 96, 0, 6, 96, 0, 6, 144, 0, 9, 144, 0, 9, 8, 129, 16, 8, 129, 16, 4, 66, 32, 4, 66, 32, 98, 36, 70, 98, 36, 70, 241, 24, 143, 241, 24, 143, 241, 24, 143, 241, 24, 143, 98, 36, 70, 98, 36, 70, 4, 66, 32, 4, 66, 32, 8, 129, 16, 8, 129, 16, 144, 0, 9, 144, 0, 9, 96, 0, 6, 96, 0, 6, 0, 0, 0, 0, 0, 0, 254, 231, 127, 254, 231, 127},
  {252, 207, 255, 252, 207, 255, 0, 0, 0, 0, 0, 0, 192, 0, 12, 192, 0, 12, 32, 1, 18, 32, 1, 18, 16, 2, 33, 16, 2, 33, 8, 132, 64, 8, 132, 64, 196, 72, 140, 196, 72, 140, 227, 49, 30, 227, 49, 30, 227, 49, 30, 227, 49, 30, 196, 72, 140, 196, 72, 140, 8, 132, 64, 8, 132, 64, 16, 2, 33, 16, 2, 33, 32, 1, 18, 32, 1, 18, 192, 0, 12, 192, 0, 12, 0, 0, 0, 0, 0, 0, 252, 207, 255, 252, 207, 255},
  {249, 159, 255, 249, 159, 255, 0, 0, 0, 0, 0, 0, 128, 1, 24, 128, 1, 24, 64, 2, 36, 64, 2, 36, 32, 4, 66, 32, 4, 66, 16, 8, 129, 16, 8, 129, 137, 145, 24, 137, 145, 24, 198, 99, 60, 198, 99, 60, 198, 99, 60, 198, 99, 60, 137, 145, 24, 137, 145, 24, 16, 8, 129, 16, 8, 129, 32, 4, 66, 32, 4, 66, 64, 2, 36, 64, 2, 36, 128, 1, 24, 128, 1, 24, 0, 0, 0, 0, 0, 0, 249, 159, 255, 249, 159, 255},
  {243, 63, 255, 243, 63, 255, 0, 0, 0, 0, 0, 0, 0, 3, 48, 0, 3, 48, 128, 4, 72, 128, 4, 72, 64, 8, 132, 64, 8, 132, 33, 16, 2, 33, 16, 2, 18, 35, 49, 18, 35, 49, 140, 199, 120, 140, 199, 120, 140, 199, 120, 140, 199, 120, 18, 35, 49, 18, 35, 49, 33, 16, 2, 33, 16, 2, 64, 8, 132, 64, 8, 132, 128, 4, 72, 128, 4, 72, 0, 3, 48, 0, 3, 48, 0, 0, 0, 0, 0, 0, 243, 63, 255, 243, 63, 255},
  {231, 127, 254, 231, 127, 254, 0, 0, 0, 0, 0, 0, 0, 6, 96, 0, 6, 96, 0, 9, 144, 0, 9, 144, 129, 16, 8, 129, 16, 8, 66, 32, 4, 66, 32, 4, 36, 70, 98, 36, 70, 98, 24, 143, 241, 24, 143, 241, 24, 143, 241, 24, 143, 241, 36, 70, 98, 36, 70, 98, 66, 32, 4, 66, 32, 4, 129, 16, 8, 129, 16, 8, 0, 9, 144, 0, 9, 144, 0, 6, 96, 0, 6, 96, 0, 0, 0, 0, 0, 0, 231, 127, 254, 231, 127, 254},
  {207, 255, 252, 207, 255, 252, 0, 0, 0, 0, 0, 0, 0, 12, 192, 0, 12, 192, 1, 18, 32, 1, 18, 32, 2, 33, 16, 2, 33, 16, 132, 64, 8, 132, 64, 8, 72, 140, 196, 72, 140, 196, 49, 30, 227, 49, 30, 227, 49, 30, 227, 49, 30, 227, 72, 140, 196, 72, 140, 196, 132, 64, 8, 132, 64, 8, 2, 33, 16, 2, 33, 16, 1, 18, 32, 1, 18, 32, 0, 12, 192, 0, 12, 192, 0, 0, 0, 0, 0, 0, 207, 255, 252, 207, 255, 252},
  {159, 255, 249, 159, 255, 249, 0, 0, 0, 0, 0, 0, 1, 24, 128, 1, 24, 128, 2, 36, 64, 2, 36, 64, 4, 66, 32, 4, 66, 32, 8, 129, 16, 8, 129, 16, 145, 24, 137, 145, 24, 137, 99, 60, 198, 99, 60, 198, 99, 60, 198, 99, 60, 198, 145, 24, 137, 145, 24, 137, 8, 129, 16, 8, 129, 16, 4, 66, 32, 4, 66, 32, 2, 36, 64, 2, 36, 64, 1, 24, 128, 1, 24, 128, 0, 0, 0, 0, 0, 0, 159, 255, 249, 159, 255, 249},
  {63, 255, 243, 63, 255, 243, 0, 0, 0, 0, 0, 0, 3, 48, 0, 3, 48, 0, 4, 72, 128, 4, 72, 128, 8, 132, 64, 8, 132, 64, 16, 2, 33, 16, 2, 33, 35, 49, 18, 35, 49, 18, 199, 120, 140, 199, 120, 140, 199, 120, 140, 199, 120, 140, 35, 49, 18, 35, 49, 18, 16, 2, 33, 16, 2, 33, 8, 132, 64, 8, 132, 64, 4, 72, 128, 4, 72, 128, 3, 48, 0, 3, 48, 0, 0, 0, 0, 0, 0, 0, 63, 255, 243, 63, 255, 243},
  {127, 254, 231, 127, 254, 231, 0, 0, 0, 0, 0, 0, 6, 96, 0, 6, 96, 0, 9, 144, 0, 9, 144, 0, 16, 8, 129, 16, 8, 129, 32, 4, 66, 32, 4, 66, 70, 98, 36, 70, 98, 36, 143, 241, 24, 143, 241, 24, 143, 241, 24, 143, 241, 24, 70, 98, 36, 70, 98, 36, 32, 4, 66, 32, 4, 66, 16, 8, 129, 16, 8, 129, 9, 144, 0, 9, 144, 0, 6, 96, 0, 6, 96, 0, 0, 0, 0, 0, 0, 0, 127, 254, 231, 127, 254, 231},
  {255, 252, 207, 255, 252, 207, 0, 0, 0, 0, 0, 0, 12, 192, 0, 12, 192, 0, 18, 32, 1, 18, 32, 1, 33, 16, 2, 33, 16, 2, 64, 8, 132, 64, 8, 132, 140, 196, 72, 140, 196, 72, 30, 227, 49, 30, 227, 49, 30, 227, 49, 30, 227, 49, 140, 196, 72, 140, 196, 72, 64, 8, 132, 64, 8, 132, 33, 16, 2, 33, 16, 2, 18, 32, 1, 18, 32, 1, 12, 192, 0, 12, 192, 0, 0, 0, 0, 0, 0, 0, 255, 252, 207, 255, 252, 207},
  {255, 249, 159, 255, 249, 159, 0, 0, 0, 0, 0, 0, 24, 128, 1, 24, 128, 1, 36, 64, 2, 36, 64, 2, 66, 32, 4, 66, 32, 4, 129, 16, 8, 129, 16, 8, 24, 137, 145, 24, 137, 145, 60, 198, 99, 60, 198, 99, 60, 198, 99, 60, 198, 99, 24, 137, 145, 24, 137, 145, 129, 16, 8, 129, 16, 8, 66, 32, 4, 66, 32, 4, 36, 64, 2, 36, 64, 2, 24, 128, 1, 24, 128, 1, 0, 0, 0, 0, 0, 0, 255, 249, 159, 255, 249, 159},
  {255, 243, 63, 255, 243, 63, 0, 0, 0, 0, 0, 0, 48, 0, 3, 48, 0, 3, 72, 128, 4, 72, 128, 4, 132, 64, 8, 132, 64, 8, 2, 33, 16, 2, 33, 16, 49, 18, 35, 49, 18, 35, 120, 140, 199, 120, 140, 199, 120, 140, 199, 120, 140, 199, 49, 18, 35, 49, 18, 35, 2, 33, 16, 2, 33, 16, 132, 64, 8, 132, 64, 8, 72, 128, 4, 72, 128, 4, 48, 0, 3, 48, 0, 3, 0, 0, 0, 0, 0, 0, 255, 243, 63, 255, 243, 63}
};

void setup() {

  pinMode(COMMAND_PIN, OUTPUT);
  digitalWrite(COMMAND_PIN, LOW);
  //The MAX72XX is in power-saving mode on startup, need to do a wakeup call
  stopShutDown();
  //Set the brightness, medium values 8
  adjustBrightness(15);
  //Clear the display
  blankDisplay();

  //Begin SD Card. Blink 2 times in 500 milliseconds interval on error
  if (!SD.begin(CS_PIN)) {
    statusBlink(2, 500);
    delay(2000);
    return;
  }

  //Open the matrix designs file to read
  dataFile = SD.open("matrix_designs.bin");

  //If file read failes, blink 4 times in 200 milliseconds interval
  if (!dataFile) {
    statusBlink(4, 200);
    delay(2000);
    return;
  }

  //If all good, blink 5 times in 100 milliseconds interval
  statusBlink(5, 100);

  //Reset timer
  timer = millis();
}

void loop() {
  if ((millis() - timer) > (delayTime * 10)) { //Triggers at interval given in 'delaytime'
    if (isReady) { //Check if ready to display the frame
      timer = millis(); //Reset timer
      showFrame(); //Display the frame
      y++;
      if (y == 12) {
        y = 0;
      }
      isReady = false; //Mark not ready then it can read the next frame from the file
      runIndecate(); //Indecate that the programe is running
    }
  }

  if (!isReady) {
    //    memcpy(frameData, design[y], sizeof(frameData));
    //    isReady = true;

    //======================== To test without SD Card, uncomment above two lines and comment bellow if else statement ======================
    
    if (dataFile.available() > 0) { //Check if data available in the file to read
      dataFile.read(frameData, 96); //Read a frame. 96 bytes (rows x chip count x bytes per chip = 2 x 6 x 8)
      isReady = true; //Marke as ready to display the frame
      dataFile.read(); // Read the new line charactor and ignore it
    } else {
      blankDisplay(); //Clear the display
      dataFile.seek(0); //Return back to begining of the file
      statusBlink(2, 1000); //Blink command indecator to show that file returned to beginning
    }
  }
}

void showFrame() {
  int devRowVal = 0;
  //  first device row
  for (int row = 0; row < 8; row++) {
    for (int device = 0; device < 6; device++) {
      deviceSet1.setRow(device, row, frameData[devRowVal]);
      devRowVal++;
    }
  }

  //second device row
  for (int row = 0; row < 8; row++) {
    for (int device = 0; device < 6; device++) {
      deviceSet2.setRow(device, row, frameData[devRowVal]);
      devRowVal++;
    }
  }
}

void statusBlink(int count, int dly) {
  for (int i = 0; i < count; i++) {
    digitalWrite(COMMAND_PIN, HIGH);
    delay(dly);
    digitalWrite(COMMAND_PIN, LOW);
    delay(dly);
  }
}

void runIndecate() {
  if (runIndecator) {
    digitalWrite(COMMAND_PIN, LOW);
    runIndecator = false;
  } else {
    digitalWrite(COMMAND_PIN, HIGH);
    runIndecator = true;
  }
}

void stopShutDown() {
  for (int i = 0; i < DEVICES; i++) {
    deviceSet1.shutdown(i, false);
  }

  for (int i = 0; i < DEVICES; i++) {
    deviceSet2.shutdown(i, false);
  }
}

void adjustBrightness(int value) {
  for (int i = 0; i < DEVICES; i++) {
    deviceSet1.setIntensity(i, value);
  }

  for (int i = 0; i < DEVICES; i++) {
    deviceSet2.setIntensity(i, value);
  }
}

void blankDisplay() {
  for (int i = 0; i < DEVICES; i++) {
    deviceSet1.clearDisplay(i);
  }

  for (int i = 0; i < DEVICES; i++) {
    deviceSet2.clearDisplay(i);
  }
}
