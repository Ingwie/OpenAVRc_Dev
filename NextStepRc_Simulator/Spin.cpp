#include <wx/msgdlg.h>
#include "Spin.h"

Spin::Spin(const char* id,uint8_t* pin,uint8_t* ddr,uint8_t* port)
{
*pin = *port;
// wxMessageBox(id, _("Ceci est une sortie")); futur use ;-)

}

Spin::~Spin()
{
  //dtor
}
