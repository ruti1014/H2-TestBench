//Dummy program to simulate rs232 answers from the H-Cell


HardwareSerial SerialH(2); 


void setup() {
  Serial.begin(9600);
  SerialH.begin(115200, SERIAL_8N1, 16, 17);  //Baudrate, Protocol, RX, TX

  Serial.println("STARTING");
}


String cmd;
bool cmdRDY = false;
void loop() {
  if (SerialH.available()) {
    char tmp = SerialH.read();
    if (tmp == '*') cmd = "";
    else if (tmp == '\n') cmdRDY = true;

    cmd += tmp;
  }


  if (cmdRDY) {

    Serial.println(cmd);
    cmdRDY = false;

    if (cmd == "*Idn?\n") {
      SerialH.print("42069");
      Serial.println("Sending SerialNR");
    } else if (cmd == "*Vol?\n") {
      SerialH.print("14");
      Serial.println("Sending voltage");
    } else if (cmd == "*Amp?\n") {
      SerialH.print("24");
      Serial.println("Sending amperage");
    }
  }
}
