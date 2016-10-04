#ifndef SPIN_H
#define SPIN_H

#include "../NextStepRc/nextsteprc.h"

class Spin
{
  public:
    Spin(const char* id, uint8_t* pin,uint8_t* ddr,uint8_t* port);
    virtual ~Spin();

  protected:

  private:
};

#endif // SPIN_H
