/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#include "../OpenAVRc.h"



static void PROTO_PPM16_reset()
{
  PROTO_PPM_reset();
  PROTO_PPMSIM_reset();
}


static void PROTO_PPM16_initialize()
{
  PROTO_PPM_initialize();
  PROTO_PPMSIM_initialize();
}


const void * PROTO_PPM16_Cmds(enum ProtoCmds cmd)
{
  switch (cmd) {
  case PROTOCMD_INIT:
    PROTO_PPM16_initialize();
    return 0;
  case PROTOCMD_RESET:
    PROTO_PPM16_reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
    sendOptionsSettingsPpm();
    return 0;
  default:
    break;
  }
  return 0;
}

