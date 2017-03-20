/*
*/



#ifndef misc_h
#define misc_h

void CLOCK_StartTimer(uint16_t us, uint16_t (*cb)(void));
void CLOCK_StopTimer();
uint32_t CLOCK_getms(void);
void CLOCK_delayms(uint32_t delay_ms);
void PROTOCOL_SetBindState(uint32_t msec);

volatile enum PROTO_MODE proto_mode;


#endif
