#ifndef MULTIPLEXER_INIT_H
#define MULTIPLEXER_INIT_H

bool CHvalue[8]     = {0,0,0,0,0,0,0,0};
bool lastCHvalue[8] = {0,0,0,0,0,0,0,0};

auto timer = timer_create_default();

//---------------------------------------------------------------------------------------------------------------
// reads the value of the selected channel (0-7):

  bool readChannel(int channel){
    bool comValue = 0;

    switch(channel){
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
        HWSerial.println("Wrong channel number !");  
    }

    delay(5);
    comValue = digitalRead(comPin);
    return comValue;
  }

//---------------------------------------------------------------------------------------------------------------
// reads the value of all used buttons (depending on 'numButtons'):

  bool readButtons(void *){
    // get current values
    for(int i=0; i<numButtons; i++){ 
      CHvalue[i] = readChannel(i);
    }

    // check if values have changed
    for(int j=0; j<numButtons; j++){
      if(lastCHvalue[j] != CHvalue[j]){
        lastCHvalue[j] = CHvalue[j];

        HWSerial.print("Channel ");
        HWSerial.print(j);
        HWSerial.print(": ");
        HWSerial.println(CHvalue[j]);
      }
    }
    return true;    // to keep timer active
  }

//---------------------------------------------------------------------------------------------------------------
// Multiplexer init:

  void initMultiplexer(){
    pinMode(channelA, OUTPUT);
    pinMode(channelB, OUTPUT); 
    pinMode(channelC, OUTPUT); 
    pinMode(comPin, INPUT_PULLDOWN);

    digitalWrite(channelA, LOW);
    digitalWrite(channelB, HIGH);
    digitalWrite(channelC, LOW);

    timer.every(50, readButtons);
  }

#endif