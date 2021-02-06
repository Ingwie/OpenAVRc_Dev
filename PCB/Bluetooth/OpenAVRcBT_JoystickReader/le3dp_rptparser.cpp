#include "le3dp_rptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
  joyEvents(evt)
{}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  bool match = true;

  // Checking if there are changes in report since the method was last called
  for (uint8_t i = 0; i < RPT_GAMEPAD_LEN; i++) {
    if ( buf[i] != oldPad[i] ) {
      match = false;
      break;
    }
  }
  // Calling Game Pad event handler
  if (!match && joyEvents) {
    joyEvents->OnGamePadChanged((const GamePadEventData*)buf);

    for (uint8_t i = 0; i < RPT_GAMEPAD_LEN; i++) oldPad[i] = buf[i];
  }
}

JoystickEvents::JoystickEvents(BtnPPMMap * bpm) {
  btnPPMMap = bpm;
}

void JoystickEvents::disconnectJoystick()
{
  btnPPMMap->disArm();
  //a better alternative is to call failsafe instead of disarm for anything other than a whoop
  //however, you need to tie failsafe to a mode in your FC software first
  //btnPPMMap->failsafe();
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt)
{
  /** Uncomment the code below to figure out what button maps to what
    Serial.print("X: ");
    PrintHex<uint16_t>(evt->x, 0x80);
    Serial.print(" Y: ");
    PrintHex<uint16_t>(evt->y, 0x80);
    Serial.print(" Hat Switch: ");
    PrintHex<uint8_t>(evt->hat, 0x80);
    Serial.print(" Twist: ");
    PrintHex<uint8_t>(evt->twist, 0x80);
    Serial.print(" Slider: ");
    PrintHex<uint8_t>(evt->slider, 0x80);
    Serial.print(" Buttons A: ");
    PrintHex<uint8_t>(evt->buttons_a, 0x80);
    Serial.print(" Buttons B: ");
    PrintHex<uint8_t>(evt->buttons_b, 0x80);
    Serial.println("");
  */
  
  btnPPMMap->mapChannelValue(BtnPPMMap::CH_THROTTLE, evt->slider, 255, true);
  btnPPMMap->mapChannelValue(BtnPPMMap::CH_AILERON, evt->x, 1024, false);
  btnPPMMap->mapChannelValue(BtnPPMMap::CH_ELEVATOR, evt->y, 1024, true);
  btnPPMMap->mapChannelValue(BtnPPMMap::CH_RUDDER, evt->twist, 255, false); 

  if (evt->buttons_b == 8) {
    btnPPMMap->arm();
  }
  if (evt->buttons_b == 4) {
    btnPPMMap->disArm();
  }
  if (evt->buttons_a == 2) {
    btnPPMMap->turtle();
  }
  if (evt->buttons_a == 4) {
    btnPPMMap->acro();
  }
  if (evt->buttons_a == 8) {
    btnPPMMap->horizon();
  }
  //You will probably want to add additional button handlers below and map them to functions like above.
  
  //btnPPMMap->debug();
}
