#ifndef DEFAULTFRAME_H
#define DEFAULTFRAME_H
#include <wx/combobox.h>
#include <wx/string.h>



//(*Headers(CommunicationsFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/combobox.h>
//*)


class CommunicationsFrame: public wxFrame
{
public:

    CommunicationsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~CommunicationsFrame();

    //(*Declarations(CommunicationsFrame)
    wxStaticText* StaticText2;
    wxButton* ButtonEnter;
    wxStaticText* StaticText1;
    wxStaticText* StaticText3;
    wxStaticBox* StaticBox1;
    wxButton* ButtonSearchavrdudepath;
    wxButton* ButtonEsc;
    wxComboBox* ComboBox3;
    wxStaticText* StaticText5;
    wxButton* ButtonDetect;
    wxComboBox* ComboBox1;
    wxTextCtrl* TextCtrl1;
    wxComboBox* ComboBox2;
    //*)

protected:

    //(*Identifiers(CommunicationsFrame)
    static const long ID_STATICBOX1;
    static const long ID_COMBOBOX1;
    static const long ID_COMBOBOX2;
    static const long ID_STATICTEXT1;
    static const long ID_BUTTONENTER;
    static const long ID_STATICTEXT2;
    static const long ID_COMBOBOX3;
    static const long ID_STATICTEXT3;
    static const long ID_STATICTEXT5;
    static const long ID_TEXTCTRL1;
    static const long ID_BUTTONESC;
    static const long ID_BUTTONSEARCHAVRDUDEPATH;
    static const long ID_BUTTONDETECT;
    //*)
    wxWindow * Parent;

private:

    //(*Handlers(CommunicationsFrame)
    void OnButton1Click(wxCommandEvent& event);
    void OnButtonSearchavrdudepathClick(wxCommandEvent& event);
    void OnButtonEscClick(wxCommandEvent& event);
    void OnButtonEnterClick(wxCommandEvent& event);
    void OnButtonDetectClick(wxCommandEvent& event);
    void GetPortList(wxArrayString *port_list);
    void DetectSerial();

    //*)

    DECLARE_EVENT_TABLE()
};

#endif
