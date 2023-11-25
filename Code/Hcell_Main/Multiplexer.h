/*  ----------- Multiplexer.h -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: periodically reads 8 channels of a multiplexer (CH0 to CH7)
*                multiplexer used in H2-Testbench: CMOS Multiplexer CD4051BE
*   How to use:  call initMultiplexer() in setup
*                call multiplexerLoop() in loop
* 	             CHvalue contains the current value of each channel
*/

#ifndef MULTIPLEXER_INIT_H
#define MULTIPLEXER_INIT_H

bool CHvalue[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool lastCHvalue[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

bool readButtons = false;  // flag to indicate if timer has expired


// selects multiplexer channel (0-7)
void selectChannel(int channel) {
  switch (channel) {
    case 0:
      digitalWrite(channelA, LOW);
      digitalWrite(channelB, LOW);
      digitalWrite(channelC, LOW);
      break;
    case 1:
      digitalWrite(channelA, HIGH);
      digitalWrite(channelB, LOW);
      digitalWrite(channelC, LOW);
      break;
    case 2:
      digitalWrite(channelA, LOW);
      digitalWrite(channelB, HIGH);
      digitalWrite(channelC, LOW);
      break;
    case 3:
      digitalWrite(channelA, HIGH);
      digitalWrite(channelB, HIGH);
      digitalWrite(channelC, LOW);
      break;
    case 4:
      digitalWrite(channelA, LOW);
      digitalWrite(channelB, LOW);
      digitalWrite(channelC, HIGH);
      break;
    case 5:
      digitalWrite(channelA, HIGH);
      digitalWrite(channelB, LOW);
      digitalWrite(channelC, HIGH);
      break;
    case 6:
      digitalWrite(channelA, LOW);
      digitalWrite(channelB, HIGH);
      digitalWrite(channelC, HIGH);
      break;
    case 7:
      digitalWrite(channelA, HIGH);
      digitalWrite(channelB, HIGH);
      digitalWrite(channelC, HIGH);
      break;
    default:
      HWSerial.println("Wrong multiplexer channel number !");
  }
}

// timer function to set multiplexer read-flag
void readMultiplexer() {
  static int timeStamp = millis();
  if ((millis() - timeStamp) >= 100) {
    readButtons = true;
    timeStamp = millis();
  }
}

// reads all channels if timer in readMultiplexer() has expired,
// selects channel and waits 15ms before reading the selected channel
void multiplexerLoop() {
  static int CHindex = 0;
  static int time = 0;
  static bool CHselected = false;

  readMultiplexer();

  // select channel
  if (readButtons && !CHselected) {
    selectChannel(CHindex);
    time = millis();
    CHselected = true;
  }

  // wait after selecting a channel
  if (millis() >= (time + 15) && time != 0) {
    time = 0;

    // read value of selected channel
    CHvalue[CHindex] = digitalRead(comPin);

    // check if value has changed
    if (lastCHvalue[CHindex] != CHvalue[CHindex]) {
      lastCHvalue[CHindex] = CHvalue[CHindex];
      debugprint("Channel ");
      debugprint(CHindex);
      debugprint(": ");
      debugprintln(CHvalue[CHindex]);

      buttonInterpreter(CHindex, CHvalue[CHindex]);
    }

    CHselected = false;
    if (CHindex == 7) {
      CHindex = 0;
      readButtons = false;
    } else {
      CHindex++;
    }
  }
}

void initMultiplexer() {
  pinMode(channelA, OUTPUT);
  pinMode(channelB, OUTPUT);
  pinMode(channelC, OUTPUT);
  pinMode(comPin, INPUT_PULLDOWN);

  digitalWrite(channelA, LOW);
  digitalWrite(channelB, HIGH);
  digitalWrite(channelC, LOW);
}

#endif