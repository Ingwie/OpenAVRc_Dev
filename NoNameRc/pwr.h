/*
 *************************************************************
 *                      NoNameRc                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#ifndef pwr_h
#define pwr_h

enum PowerState {
  e_power_on,
  e_power_trainer,
  e_power_usb,
  e_power_off,
  e_power_press,
};

#endif
