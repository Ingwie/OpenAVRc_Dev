#include "BtnPPMMap.h"

BtnPPMMap::BtnPPMMap()
{
  channel[CH_THROTTLE] = PPM_FAIL_SAFE_VALUE;
  channel[CH_AILERON] = PPM_CENTER_VALUE;
  channel[CH_ELEVATOR] = PPM_CENTER_VALUE;
  channel[CH_RUDDER] = PPM_CENTER_VALUE;
  channel[CH_5] = PPM_FAIL_SAFE_VALUE;
  channel[CH_6] = PPM_FAIL_SAFE_VALUE;
  channel[CH_7] = PPM_FAIL_SAFE_VALUE;
  channel[CH_8] = PPM_FAIL_SAFE_VALUE;
}


int BtnPPMMap::getChannelValue(int c) {
  return channel[c];
}

void BtnPPMMap::mapChannelValue(int c, long value, int maxValue, boolean invert) {
  //Serial.print("X: ");
  //Serial.print(value);
  if (invert) {
    channel[c] = map(value, 0, maxValue, PPM_MAX_VALUE, PPM_MIN_VALUE);
  } else {
    channel[c] = map(value, 0, maxValue, PPM_MIN_VALUE, PPM_MAX_VALUE);
  }

}

void BtnPPMMap::arm() {
    //arm ch5 - 1500
    channel[CH_5] = 1500;
}

void BtnPPMMap::disArm() {
    //disarm ch5 - 950 (default)
    channel[CH_5] = 950;
}

void BtnPPMMap::acro() {
    channel[CH_6] = 950;
}

void BtnPPMMap::horizon() {
    channel[CH_6] = 1500;
}

void BtnPPMMap::turtle() {
    channel[CH_6] = 2010;
}

void BtnPPMMap::angle() {
    //put your own code here for this functionality
}

void BtnPPMMap::failsafe() {
    //put your own code here for this functionality
    //be sure to set up your FC software to failsafe on this value as well
}

void BtnPPMMap::beeper() {
    //put your own code here for this functionality
}

void BtnPPMMap::rth() {
    //put your own code here for this functionality
}

void BtnPPMMap::autotune() {
    //put your own code here for this functionality
}

void BtnPPMMap::autotrim() {
    //put your own code here for this functionality
}

void BtnPPMMap::hold() {
    //put your own code here for this functionality
}

void BtnPPMMap::debug()
{
  Serial.print("T:");
  Serial.print(channel[CH_THROTTLE]);
  Serial.print(" A:");
  Serial.print(channel[CH_AILERON]);
  Serial.print(" E:");
  Serial.print(channel[CH_ELEVATOR]);
  Serial.print(" R:");
  Serial.print(channel[CH_RUDDER]);
  Serial.print(" 5:");
  Serial.print(channel[CH_5]);
  Serial.print(" 6:");
  Serial.print(channel[CH_6]);
  Serial.print(" 7:");
  Serial.print(channel[CH_7]);
  Serial.print(" 8:");
  Serial.print(channel[CH_8]);
  Serial.println("");
}
