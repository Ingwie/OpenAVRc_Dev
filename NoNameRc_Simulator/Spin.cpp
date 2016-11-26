#include <wx/msgdlg.h>
#include "Spin.h"

Spin::Spin(const char* id,uint8_t* pin,uint8_t* ddr,uint8_t* port)
{
    P_id = id;
    P_pin = pin;
    P_ddr = ddr;
    P_port = port;
}

Spin::~Spin()
{
    //dtor
}

void Spin::init()
{
    *P_pin = *P_port;
}

bool Spin::GetPin(uint8_t num)
{
    return (*P_pin & (1<<num));
}

void Spin::SetPin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);
    }
    else
    {
        *P_pin |= (1<<num);
    }
}

void Spin::ResetPin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);

    }
    else
    {
        *P_pin &= ~(1<<num);
    }
}

void Spin::TogglePin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);

    }
    else
    {
        if (GetPin(num)) ResetPin(num);
        else SetPin(num);
    }
}
