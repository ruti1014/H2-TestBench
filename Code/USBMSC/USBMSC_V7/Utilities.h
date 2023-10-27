



bool startHcell() {}
bool stopHcell() {}
void recording(){
  if (recordingFlag){
  }else{
  }
}

void buttonInterpreter(int button, int value) {
  //value == 0 button release
  //value == 1 button pressed
  switch (button) {
    case 0:  //hcell start == 1, stop == 0
      if (value == 1) startHcell();
      else stopHcell();
      break;
    case 1:  //recording start == 1, stop == 0
      if (value == 1) recordingFlag = true;
      else recordingFlag = false;
      break;
    case 2:  //OK
      if (value == 1) dPad.ok = true;
      else dPad.ok = false;
      break;
    case 3:  //left
      if (value == 1) dPad.left = true;
      else dPad.left = false;
      break;
    case 4:  //right
      if (value == 1) dPad.right = true;
      else dPad.right = false;
      break;
    case 5:  //up
      if (value == 1) dPad.up = true;
      else dPad.up = false;
      break;
    case 6:  //down
      if (value == 1) dPad.down = true;
      else dPad.down = false;
      break;
    default:
      break;
  }
}