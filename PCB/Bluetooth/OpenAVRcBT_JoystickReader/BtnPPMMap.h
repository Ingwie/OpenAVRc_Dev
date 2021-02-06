#include <usbhid.h>

/**
 * Button to PPM Mapping - Maps the joystick buttons and actions to different PPM values
 */
class BtnPPMMap
{
  public: 
    BtnPPMMap();
    
    static const int CH_THROTTLE = 0;
    static const int CH_AILERON = 1;
    static const int CH_ELEVATOR = 2;
    static const int CH_RUDDER = 3;
    static const int CH_5 = 4;
    static const int CH_6 = 5;
    static const int CH_7 = 6;
    static const int CH_8 = 7;
    static const int NUM_CHANNELS = 8;
    static const int PPM_MAX_VALUE = 2000;
    static const int PPM_MIN_VALUE = 1000;
    static const int PPM_CENTER_VALUE = ((PPM_MAX_VALUE - PPM_MIN_VALUE) / 2) + PPM_MIN_VALUE;
    static const int PPM_FAIL_SAFE_VALUE = 950;

    void mapChannelValue(int channel, long value, int maxValue, boolean invert);
    int getChannelValue(int channel);
    //Below are the different "Modes". You may need to add some more of your favorites to this list.
    void disArm();
    void arm();
    void turtle();
    void acro();
    void horizon();
    void angle();
    void beeper();
    void debug();
    void failsafe();
    void rth();
    void autotune();
    void autotrim();
    void hold();

  private:
    int channel[NUM_CHANNELS] = {};
};
