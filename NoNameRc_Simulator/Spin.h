#ifndef SPIN_H
#define SPIN_H

#include "../NoNameRc/NoNameRc.h"

class Spin
{
public:
    Spin(const char* id, uint8_t* pin,uint8_t* ddr,uint8_t* port);
    virtual ~Spin();
    void init();
    bool GetPin(uint8_t num);
    void SetPin(uint8_t num);
    void ResetPin(uint8_t num);
    void TogglePin(uint8_t num);

protected:

private:
    const char* P_id;
    uint8_t* P_pin;
    uint8_t* P_ddr;
    uint8_t* P_port;
};

#endif // SPIN_H
