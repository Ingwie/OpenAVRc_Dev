//

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "misc.h"

#ifndef MODULAR
#define PROTODEF(proto, module, map, cmd, name) extern const void * cmd(enum ProtoCmds);
#include "protocol.h"
#undef PROTODEF
#endif

#ifdef PROTO_HAS_A7105
#include "iface_a7105.h"
#endif

#ifdef PROTO_HAS_CYRF6936
#include "iface_cyrf6936.h"
#endif

#ifdef PROTO_HAS_CC2500
#include "iface_cc2500.h"
#include "cc2500.c"
#include "FRSKYV_cc2500.c"
#include "skyartec_cc2500.c"
#endif

#ifdef PROTO_HAS_NRF24L01
#include "iface_nrf24l01.h"
#endif

#endif // _INTERFACE_H_
