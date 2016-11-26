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

#ifndef _FIFO_H_
#define _FIFO_H_

template <int N>
class Fifo
{
  public:
    Fifo():
      widx(0),
      ridx(0)
    {
    }

    void push(uint8_t byte) {
      uint32_t next = (widx+1) & (N-1);
      if (next != ridx) {
        fifo[widx] = byte;
        widx = next;
      }
    }

    bool pop(uint8_t & byte) {
      if (isEmpty()) {
        return false;
      }
      else {
        byte = fifo[ridx];
        ridx = (ridx+1) & (N-1);
        return true;
      }
    }

    bool isEmpty() {
      return (ridx == widx);
    }

    bool isFull() {
      uint32_t next = (widx+1) & (N-1);
      return (next == ridx);
    }

    void flush() {
      while (!isEmpty()) {};
    }

  protected:
    uint8_t fifo[N];
    volatile uint32_t widx;
    volatile uint32_t ridx;
};

#endif // _FIFO_H_
