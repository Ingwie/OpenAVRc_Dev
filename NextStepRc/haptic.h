/*
 *************************************************************
 *                      NEXTSTEPRC                           *
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

#ifndef haptic_h
#define haptic_h

#define HAPTIC_QUEUE_LENGTH  4

class hapticQueue
{
  public:

    hapticQueue();

    // only difference between these two functions is that one does the
    // interupt queue (Now) and the other queues for playing ASAP.
    void play(uint8_t tLen, uint8_t tPause, uint8_t tRepeat=0);

    inline bool busy() { return (buzzTimeLeft > 0); }

    void event(uint8_t e);

    // heartbeat is responsibile for issueing the haptic buzzs and general square waves
    // it is essentially the life of the class.
    void heartbeat();

    // bool freeslots(uint8_t slots);

    inline bool empty() {
      return (t_queueRidx == t_queueWidx);
    }

  protected:
    inline uint8_t getHapticLength(uint8_t tLen) {
      return ((g_eeGeneral.hapticLength * 2) + tLen) * 2;
    }

  private:
    uint8_t t_queueRidx;
    uint8_t t_queueWidx;

    uint8_t buzzTimeLeft;
    uint8_t buzzPause;

    uint8_t hapticTick;

    // queue arrays
    uint8_t queueHapticLength[HAPTIC_QUEUE_LENGTH];
    uint8_t queueHapticPause[HAPTIC_QUEUE_LENGTH];
    uint8_t queueHapticRepeat[HAPTIC_QUEUE_LENGTH];
};

//wrapper function - dirty but results in a space saving!!!
extern hapticQueue haptic;

#define IS_HAPTIC_BUSY()     haptic.busy()
#define HAPTIC_HEARTBEAT()   haptic.heartbeat()


#endif // haptic_h
