#define comPin 25
#define channelA 16
#define channelB 17
#define channelC 18

bool readChannel (int channel);

bool CH0 = 0;
bool lastCH0 = 0;
bool CH1 = 0;
bool lastCH1 = 0;
bool CH2 = 0;
bool lastCH2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(channelA, OUTPUT);
  pinMode(channelB, OUTPUT); 
  pinMode(channelC, OUTPUT); 
  pinMode(comPin, INPUT_PULLDOWN);
  digitalWrite(channelA, LOW);
  digitalWrite(channelB, HIGH);
  digitalWrite(channelC, LOW);
}

void loop() {

  CH0 = readChannel(0);
  CH2 = readChannel(2);
  CH1 = readChannel(1);
  

  if(lastCH0 != CH0)
  {
    Serial.print("Channel 0: ");
    Serial.println(CH0);
    lastCH0 = CH0;
  }

  if(lastCH1 != CH1)
  {
    Serial.print("Channel 1: ");
    Serial.println(CH1);
    lastCH1 = CH1;
  }

  if(lastCH2 != CH2)
  {
    Serial.print("Channel 2: ");
    Serial.println(CH2);
    lastCH2 = CH2;
  }
  
  delay(50);
}


bool readChannel(int channel){
  bool comValue = 0;

  switch(channel)
  {
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
    default:
      Serial.println("Channel not connected !");  
  }

  comValue = digitalRead(comPin);

  return comValue;
}
