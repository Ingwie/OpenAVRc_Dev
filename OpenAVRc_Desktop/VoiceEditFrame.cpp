#include "VoiceEditFrame.h"
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>


extern wxString AppPath;
extern wxString voice_Langue;
wxString voiceText;
wxString voicePrompt;
wxString file;
wxString line;

//(*InternalHeaders(VoiceEditFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(VoiceEditFrame)
const long VoiceEditFrame::ID_STATICBOX1 = wxNewId();
const long VoiceEditFrame::ID_GRID1 = wxNewId();
const long VoiceEditFrame::ID_BUTTONSAUVEGARDER = wxNewId();
const long VoiceEditFrame::ID_BUTTONGENERER = wxNewId();
const long VoiceEditFrame::ID_PANEL1 = wxNewId();
//*)



BEGIN_EVENT_TABLE(VoiceEditFrame,wxFrame)
    //(*EventTable(VoiceEditFrame)
    //*)
END_EVENT_TABLE()

VoiceEditFrame::VoiceEditFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(VoiceEditFrame)
    Create(parent, wxID_ANY, _("Voix"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(410,521));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP_BOOK")),wxART_FRAME_ICON));
    	SetIcon(FrameIcon);
    }
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(160,208), wxSize(408,520), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Double cliquez pour écouter, click pour éditer."), wxPoint(16,8), wxSize(376,472), 0, _T("ID_STATICBOX1"));
    VoiceGrid = new wxGrid(Panel1, ID_GRID1, wxPoint(24,48), wxSize(360,424), 0, _T("ID_GRID1"));
    VoiceGrid->CreateGrid(512,2);
    VoiceGrid->EnableEditing(true);
    VoiceGrid->EnableGridLines(true);
    VoiceGrid->SetColLabelSize(20);
    VoiceGrid->SetRowLabelSize(40);
    VoiceGrid->SetDefaultColSize(150, true);
    VoiceGrid->SetColLabelValue(0, _("Nom"));
    VoiceGrid->SetColLabelValue(1, _("Annonces vocales"));
    VoiceGrid->SetRowLabelValue(0, _("0000"));
    VoiceGrid->SetRowLabelValue(1, _("0001"));
    VoiceGrid->SetRowLabelValue(2, _("0002"));
    VoiceGrid->SetRowLabelValue(3, _("0003"));
    VoiceGrid->SetRowLabelValue(4, _("0004"));
    VoiceGrid->SetRowLabelValue(5, _("0005"));
    VoiceGrid->SetRowLabelValue(6, _("0006"));
    VoiceGrid->SetRowLabelValue(7, _("0007"));
    VoiceGrid->SetRowLabelValue(8, _("0008"));
    VoiceGrid->SetRowLabelValue(9, _("0009"));
    VoiceGrid->SetRowLabelValue(10, _("0010"));
    VoiceGrid->SetRowLabelValue(11, _("0011"));
    VoiceGrid->SetRowLabelValue(12, _("0012"));
    VoiceGrid->SetRowLabelValue(13, _("0013"));
    VoiceGrid->SetRowLabelValue(14, _("0014"));
    VoiceGrid->SetRowLabelValue(15, _("0015"));
    VoiceGrid->SetRowLabelValue(16, _("0016"));
    VoiceGrid->SetRowLabelValue(17, _("0017"));
    VoiceGrid->SetRowLabelValue(18, _("0018"));
    VoiceGrid->SetRowLabelValue(19, _("0019"));
    VoiceGrid->SetRowLabelValue(20, _("0020"));
    VoiceGrid->SetRowLabelValue(21, _("0021"));
    VoiceGrid->SetRowLabelValue(22, _("0022"));
    VoiceGrid->SetRowLabelValue(23, _("0023"));
    VoiceGrid->SetRowLabelValue(24, _("0024"));
    VoiceGrid->SetRowLabelValue(25, _("0025"));
    VoiceGrid->SetRowLabelValue(26, _("0026"));
    VoiceGrid->SetRowLabelValue(27, _("0027"));
    VoiceGrid->SetRowLabelValue(28, _("0028"));
    VoiceGrid->SetRowLabelValue(29, _("0029"));
    VoiceGrid->SetRowLabelValue(30, _("0030"));
    VoiceGrid->SetRowLabelValue(31, _("0031"));
    VoiceGrid->SetRowLabelValue(32, _("0032"));
    VoiceGrid->SetRowLabelValue(33, _("0033"));
    VoiceGrid->SetRowLabelValue(34, _("0034"));
    VoiceGrid->SetRowLabelValue(35, _("0035"));
    VoiceGrid->SetRowLabelValue(36, _("0036"));
    VoiceGrid->SetRowLabelValue(37, _("0037"));
    VoiceGrid->SetRowLabelValue(38, _("0038"));
    VoiceGrid->SetRowLabelValue(39, _("0039"));
    VoiceGrid->SetRowLabelValue(40, _("0040"));
    VoiceGrid->SetRowLabelValue(41, _("0041"));
    VoiceGrid->SetRowLabelValue(42, _("0042"));
    VoiceGrid->SetRowLabelValue(43, _("0043"));
    VoiceGrid->SetRowLabelValue(44, _("0044"));
    VoiceGrid->SetRowLabelValue(45, _("0045"));
    VoiceGrid->SetRowLabelValue(46, _("0046"));
    VoiceGrid->SetRowLabelValue(47, _("0047"));
    VoiceGrid->SetRowLabelValue(48, _("0048"));
    VoiceGrid->SetRowLabelValue(49, _("0049"));
    VoiceGrid->SetRowLabelValue(50, _("0050"));
    VoiceGrid->SetRowLabelValue(51, _("0051"));
    VoiceGrid->SetRowLabelValue(52, _("0052"));
    VoiceGrid->SetRowLabelValue(53, _("0053"));
    VoiceGrid->SetRowLabelValue(54, _("0054"));
    VoiceGrid->SetRowLabelValue(55, _("0055"));
    VoiceGrid->SetRowLabelValue(56, _("0056"));
    VoiceGrid->SetRowLabelValue(57, _("0057"));
    VoiceGrid->SetRowLabelValue(58, _("0058"));
    VoiceGrid->SetRowLabelValue(59, _("0059"));
    VoiceGrid->SetRowLabelValue(60, _("0060"));
    VoiceGrid->SetRowLabelValue(61, _("0061"));
    VoiceGrid->SetRowLabelValue(62, _("0062"));
    VoiceGrid->SetRowLabelValue(63, _("0063"));
    VoiceGrid->SetRowLabelValue(64, _("0064"));
    VoiceGrid->SetRowLabelValue(65, _("0065"));
    VoiceGrid->SetRowLabelValue(66, _("0066"));
    VoiceGrid->SetRowLabelValue(67, _("0067"));
    VoiceGrid->SetRowLabelValue(68, _("0068"));
    VoiceGrid->SetRowLabelValue(69, _("0069"));
    VoiceGrid->SetRowLabelValue(70, _("0070"));
    VoiceGrid->SetRowLabelValue(71, _("0071"));
    VoiceGrid->SetRowLabelValue(72, _("0072"));
    VoiceGrid->SetRowLabelValue(73, _("0073"));
    VoiceGrid->SetRowLabelValue(74, _("0074"));
    VoiceGrid->SetRowLabelValue(75, _("0075"));
    VoiceGrid->SetRowLabelValue(76, _("0076"));
    VoiceGrid->SetRowLabelValue(77, _("0077"));
    VoiceGrid->SetRowLabelValue(78, _("0078"));
    VoiceGrid->SetRowLabelValue(79, _("0079"));
    VoiceGrid->SetRowLabelValue(80, _("0080"));
    VoiceGrid->SetRowLabelValue(81, _("0081"));
    VoiceGrid->SetRowLabelValue(82, _("0082"));
    VoiceGrid->SetRowLabelValue(83, _("0083"));
    VoiceGrid->SetRowLabelValue(84, _("0084"));
    VoiceGrid->SetRowLabelValue(85, _("0085"));
    VoiceGrid->SetRowLabelValue(86, _("0086"));
    VoiceGrid->SetRowLabelValue(87, _("0087"));
    VoiceGrid->SetRowLabelValue(88, _("0088"));
    VoiceGrid->SetRowLabelValue(89, _("0089"));
    VoiceGrid->SetRowLabelValue(90, _("0090"));
    VoiceGrid->SetRowLabelValue(91, _("0091"));
    VoiceGrid->SetRowLabelValue(92, _("0092"));
    VoiceGrid->SetRowLabelValue(93, _("0093"));
    VoiceGrid->SetRowLabelValue(94, _("0094"));
    VoiceGrid->SetRowLabelValue(95, _("0095"));
    VoiceGrid->SetRowLabelValue(96, _("0096"));
    VoiceGrid->SetRowLabelValue(97, _("0097"));
    VoiceGrid->SetRowLabelValue(98, _("0098"));
    VoiceGrid->SetRowLabelValue(99, _("0099"));
    VoiceGrid->SetRowLabelValue(100, _("0100"));
    VoiceGrid->SetRowLabelValue(101, _("0101"));
    VoiceGrid->SetRowLabelValue(102, _("0102"));
    VoiceGrid->SetRowLabelValue(103, _("0103"));
    VoiceGrid->SetRowLabelValue(104, _("0104"));
    VoiceGrid->SetRowLabelValue(105, _("0105"));
    VoiceGrid->SetRowLabelValue(106, _("0106"));
    VoiceGrid->SetRowLabelValue(107, _("0107"));
    VoiceGrid->SetRowLabelValue(108, _("0108"));
    VoiceGrid->SetRowLabelValue(109, _("0109"));
    VoiceGrid->SetRowLabelValue(110, _("0110"));
    VoiceGrid->SetRowLabelValue(111, _("0111"));
    VoiceGrid->SetRowLabelValue(112, _("0112"));
    VoiceGrid->SetRowLabelValue(113, _("0113"));
    VoiceGrid->SetRowLabelValue(114, _("0114"));
    VoiceGrid->SetRowLabelValue(115, _("0115"));
    VoiceGrid->SetRowLabelValue(116, _("0116"));
    VoiceGrid->SetRowLabelValue(117, _("0117"));
    VoiceGrid->SetRowLabelValue(118, _("0118"));
    VoiceGrid->SetRowLabelValue(119, _("0119"));
    VoiceGrid->SetRowLabelValue(120, _("0120"));
    VoiceGrid->SetRowLabelValue(121, _("0121"));
    VoiceGrid->SetRowLabelValue(122, _("0122"));
    VoiceGrid->SetRowLabelValue(123, _("0123"));
    VoiceGrid->SetRowLabelValue(124, _("0124"));
    VoiceGrid->SetRowLabelValue(125, _("0125"));
    VoiceGrid->SetRowLabelValue(126, _("0126"));
    VoiceGrid->SetRowLabelValue(127, _("0127"));
    VoiceGrid->SetRowLabelValue(128, _("0128"));
    VoiceGrid->SetRowLabelValue(129, _("0129"));
    VoiceGrid->SetRowLabelValue(130, _("0130"));
    VoiceGrid->SetRowLabelValue(131, _("0131"));
    VoiceGrid->SetRowLabelValue(132, _("0132"));
    VoiceGrid->SetRowLabelValue(133, _("0133"));
    VoiceGrid->SetRowLabelValue(134, _("0134"));
    VoiceGrid->SetRowLabelValue(135, _("0135"));
    VoiceGrid->SetRowLabelValue(136, _("0136"));
    VoiceGrid->SetRowLabelValue(137, _("0137"));
    VoiceGrid->SetRowLabelValue(138, _("0138"));
    VoiceGrid->SetRowLabelValue(139, _("0139"));
    VoiceGrid->SetRowLabelValue(140, _("0140"));
    VoiceGrid->SetRowLabelValue(141, _("0141"));
    VoiceGrid->SetRowLabelValue(142, _("0142"));
    VoiceGrid->SetRowLabelValue(143, _("0143"));
    VoiceGrid->SetRowLabelValue(144, _("0144"));
    VoiceGrid->SetRowLabelValue(145, _("0145"));
    VoiceGrid->SetRowLabelValue(146, _("0146"));
    VoiceGrid->SetRowLabelValue(147, _("0147"));
    VoiceGrid->SetRowLabelValue(148, _("0148"));
    VoiceGrid->SetRowLabelValue(149, _("0149"));
    VoiceGrid->SetRowLabelValue(150, _("0150"));
    VoiceGrid->SetRowLabelValue(151, _("0151"));
    VoiceGrid->SetRowLabelValue(152, _("0152"));
    VoiceGrid->SetRowLabelValue(153, _("0153"));
    VoiceGrid->SetRowLabelValue(154, _("0154"));
    VoiceGrid->SetRowLabelValue(155, _("0155"));
    VoiceGrid->SetRowLabelValue(156, _("0156"));
    VoiceGrid->SetRowLabelValue(157, _("0157"));
    VoiceGrid->SetRowLabelValue(158, _("0158"));
    VoiceGrid->SetRowLabelValue(159, _("0159"));
    VoiceGrid->SetRowLabelValue(160, _("0160"));
    VoiceGrid->SetRowLabelValue(161, _("0161"));
    VoiceGrid->SetRowLabelValue(162, _("0162"));
    VoiceGrid->SetRowLabelValue(163, _("0163"));
    VoiceGrid->SetRowLabelValue(164, _("0164"));
    VoiceGrid->SetRowLabelValue(165, _("0165"));
    VoiceGrid->SetRowLabelValue(166, _("0166"));
    VoiceGrid->SetRowLabelValue(167, _("0167"));
    VoiceGrid->SetRowLabelValue(168, _("0168"));
    VoiceGrid->SetRowLabelValue(169, _("0169"));
    VoiceGrid->SetRowLabelValue(170, _("0170"));
    VoiceGrid->SetRowLabelValue(171, _("0171"));
    VoiceGrid->SetRowLabelValue(172, _("0172"));
    VoiceGrid->SetRowLabelValue(173, _("0173"));
    VoiceGrid->SetRowLabelValue(174, _("0174"));
    VoiceGrid->SetRowLabelValue(175, _("0175"));
    VoiceGrid->SetRowLabelValue(176, _("0176"));
    VoiceGrid->SetRowLabelValue(177, _("0177"));
    VoiceGrid->SetRowLabelValue(178, _("0178"));
    VoiceGrid->SetRowLabelValue(179, _("0179"));
    VoiceGrid->SetRowLabelValue(180, _("0180"));
    VoiceGrid->SetRowLabelValue(181, _("0181"));
    VoiceGrid->SetRowLabelValue(182, _("0182"));
    VoiceGrid->SetRowLabelValue(183, _("0183"));
    VoiceGrid->SetRowLabelValue(184, _("0184"));
    VoiceGrid->SetRowLabelValue(185, _("0185"));
    VoiceGrid->SetRowLabelValue(186, _("0186"));
    VoiceGrid->SetRowLabelValue(187, _("0187"));
    VoiceGrid->SetRowLabelValue(188, _("0188"));
    VoiceGrid->SetRowLabelValue(189, _("0189"));
    VoiceGrid->SetRowLabelValue(190, _("0190"));
    VoiceGrid->SetRowLabelValue(191, _("0191"));
    VoiceGrid->SetRowLabelValue(192, _("0192"));
    VoiceGrid->SetRowLabelValue(193, _("0193"));
    VoiceGrid->SetRowLabelValue(194, _("0194"));
    VoiceGrid->SetRowLabelValue(195, _("0195"));
    VoiceGrid->SetRowLabelValue(196, _("0196"));
    VoiceGrid->SetRowLabelValue(197, _("0197"));
    VoiceGrid->SetRowLabelValue(198, _("0198"));
    VoiceGrid->SetRowLabelValue(199, _("0199"));
    VoiceGrid->SetRowLabelValue(200, _("0200"));
    VoiceGrid->SetRowLabelValue(201, _("0201"));
    VoiceGrid->SetRowLabelValue(202, _("0202"));
    VoiceGrid->SetRowLabelValue(203, _("0203"));
    VoiceGrid->SetRowLabelValue(204, _("0204"));
    VoiceGrid->SetRowLabelValue(205, _("0205"));
    VoiceGrid->SetRowLabelValue(206, _("0206"));
    VoiceGrid->SetRowLabelValue(207, _("0207"));
    VoiceGrid->SetRowLabelValue(208, _("0208"));
    VoiceGrid->SetRowLabelValue(209, _("0209"));
    VoiceGrid->SetRowLabelValue(210, _("0210"));
    VoiceGrid->SetRowLabelValue(211, _("0211"));
    VoiceGrid->SetRowLabelValue(212, _("0212"));
    VoiceGrid->SetRowLabelValue(213, _("0213"));
    VoiceGrid->SetRowLabelValue(214, _("0214"));
    VoiceGrid->SetRowLabelValue(215, _("0215"));
    VoiceGrid->SetRowLabelValue(216, _("0216"));
    VoiceGrid->SetRowLabelValue(217, _("0217"));
    VoiceGrid->SetRowLabelValue(218, _("0218"));
    VoiceGrid->SetRowLabelValue(219, _("0219"));
    VoiceGrid->SetRowLabelValue(220, _("0220"));
    VoiceGrid->SetRowLabelValue(221, _("0221"));
    VoiceGrid->SetRowLabelValue(222, _("0222"));
    VoiceGrid->SetRowLabelValue(223, _("0223"));
    VoiceGrid->SetRowLabelValue(224, _("0224"));
    VoiceGrid->SetRowLabelValue(225, _("0225"));
    VoiceGrid->SetRowLabelValue(226, _("0226"));
    VoiceGrid->SetRowLabelValue(227, _("0227"));
    VoiceGrid->SetRowLabelValue(228, _("0228"));
    VoiceGrid->SetRowLabelValue(229, _("0229"));
    VoiceGrid->SetRowLabelValue(230, _("0230"));
    VoiceGrid->SetRowLabelValue(231, _("0231"));
    VoiceGrid->SetRowLabelValue(232, _("0232"));
    VoiceGrid->SetRowLabelValue(233, _("0233"));
    VoiceGrid->SetRowLabelValue(234, _("0234"));
    VoiceGrid->SetRowLabelValue(235, _("0235"));
    VoiceGrid->SetRowLabelValue(236, _("0236"));
    VoiceGrid->SetRowLabelValue(237, _("0237"));
    VoiceGrid->SetRowLabelValue(238, _("0238"));
    VoiceGrid->SetRowLabelValue(239, _("0239"));
    VoiceGrid->SetRowLabelValue(240, _("0240"));
    VoiceGrid->SetRowLabelValue(241, _("0241"));
    VoiceGrid->SetRowLabelValue(242, _("0242"));
    VoiceGrid->SetRowLabelValue(243, _("0243"));
    VoiceGrid->SetRowLabelValue(244, _("0244"));
    VoiceGrid->SetRowLabelValue(245, _("0245"));
    VoiceGrid->SetRowLabelValue(246, _("0246"));
    VoiceGrid->SetRowLabelValue(247, _("0247"));
    VoiceGrid->SetRowLabelValue(248, _("0248"));
    VoiceGrid->SetRowLabelValue(249, _("0249"));
    VoiceGrid->SetRowLabelValue(250, _("0250"));
    VoiceGrid->SetRowLabelValue(251, _("0251"));
    VoiceGrid->SetRowLabelValue(252, _("0252"));
    VoiceGrid->SetRowLabelValue(253, _("0253"));
    VoiceGrid->SetRowLabelValue(254, _("0254"));
    VoiceGrid->SetRowLabelValue(255, _("0255"));
    VoiceGrid->SetRowLabelValue(256, _("0256"));
    VoiceGrid->SetRowLabelValue(257, _("0257"));
    VoiceGrid->SetRowLabelValue(258, _("0258"));
    VoiceGrid->SetRowLabelValue(259, _("0259"));
    VoiceGrid->SetRowLabelValue(260, _("0260"));
    VoiceGrid->SetRowLabelValue(261, _("0261"));
    VoiceGrid->SetRowLabelValue(262, _("0262"));
    VoiceGrid->SetRowLabelValue(263, _("0263"));
    VoiceGrid->SetRowLabelValue(264, _("0264"));
    VoiceGrid->SetRowLabelValue(265, _("0265"));
    VoiceGrid->SetRowLabelValue(266, _("0266"));
    VoiceGrid->SetRowLabelValue(267, _("0267"));
    VoiceGrid->SetRowLabelValue(268, _("0268"));
    VoiceGrid->SetRowLabelValue(269, _("0269"));
    VoiceGrid->SetRowLabelValue(270, _("0270"));
    VoiceGrid->SetRowLabelValue(271, _("0271"));
    VoiceGrid->SetRowLabelValue(272, _("0272"));
    VoiceGrid->SetRowLabelValue(273, _("0273"));
    VoiceGrid->SetRowLabelValue(274, _("0274"));
    VoiceGrid->SetRowLabelValue(275, _("0275"));
    VoiceGrid->SetRowLabelValue(276, _("0276"));
    VoiceGrid->SetRowLabelValue(277, _("0277"));
    VoiceGrid->SetRowLabelValue(278, _("0278"));
    VoiceGrid->SetRowLabelValue(279, _("0279"));
    VoiceGrid->SetRowLabelValue(280, _("0280"));
    VoiceGrid->SetRowLabelValue(281, _("0281"));
    VoiceGrid->SetRowLabelValue(282, _("0282"));
    VoiceGrid->SetRowLabelValue(283, _("0283"));
    VoiceGrid->SetRowLabelValue(284, _("0284"));
    VoiceGrid->SetRowLabelValue(285, _("0285"));
    VoiceGrid->SetRowLabelValue(286, _("0286"));
    VoiceGrid->SetRowLabelValue(287, _("0287"));
    VoiceGrid->SetRowLabelValue(288, _("0288"));
    VoiceGrid->SetRowLabelValue(289, _("0289"));
    VoiceGrid->SetRowLabelValue(290, _("0290"));
    VoiceGrid->SetRowLabelValue(291, _("0291"));
    VoiceGrid->SetRowLabelValue(292, _("0292"));
    VoiceGrid->SetRowLabelValue(293, _("0293"));
    VoiceGrid->SetRowLabelValue(294, _("0294"));
    VoiceGrid->SetRowLabelValue(295, _("0295"));
    VoiceGrid->SetRowLabelValue(296, _("0296"));
    VoiceGrid->SetRowLabelValue(297, _("0297"));
    VoiceGrid->SetRowLabelValue(298, _("0298"));
    VoiceGrid->SetRowLabelValue(299, _("0299"));
    VoiceGrid->SetRowLabelValue(300, _("0300"));
    VoiceGrid->SetRowLabelValue(301, _("0301"));
    VoiceGrid->SetRowLabelValue(302, _("0302"));
    VoiceGrid->SetRowLabelValue(303, _("0303"));
    VoiceGrid->SetRowLabelValue(304, _("0304"));
    VoiceGrid->SetRowLabelValue(305, _("0305"));
    VoiceGrid->SetRowLabelValue(306, _("0306"));
    VoiceGrid->SetRowLabelValue(307, _("0307"));
    VoiceGrid->SetRowLabelValue(308, _("0308"));
    VoiceGrid->SetRowLabelValue(309, _("0309"));
    VoiceGrid->SetRowLabelValue(310, _("0310"));
    VoiceGrid->SetRowLabelValue(311, _("0311"));
    VoiceGrid->SetRowLabelValue(312, _("0312"));
    VoiceGrid->SetRowLabelValue(313, _("0313"));
    VoiceGrid->SetRowLabelValue(314, _("0314"));
    VoiceGrid->SetRowLabelValue(315, _("0315"));
    VoiceGrid->SetRowLabelValue(316, _("0316"));
    VoiceGrid->SetRowLabelValue(317, _("0317"));
    VoiceGrid->SetRowLabelValue(318, _("0318"));
    VoiceGrid->SetRowLabelValue(319, _("0319"));
    VoiceGrid->SetRowLabelValue(320, _("0320"));
    VoiceGrid->SetRowLabelValue(321, _("0321"));
    VoiceGrid->SetRowLabelValue(322, _("0322"));
    VoiceGrid->SetRowLabelValue(323, _("0323"));
    VoiceGrid->SetRowLabelValue(324, _("0324"));
    VoiceGrid->SetRowLabelValue(325, _("0325"));
    VoiceGrid->SetRowLabelValue(326, _("0326"));
    VoiceGrid->SetRowLabelValue(327, _("0327"));
    VoiceGrid->SetRowLabelValue(328, _("0328"));
    VoiceGrid->SetRowLabelValue(329, _("0329"));
    VoiceGrid->SetRowLabelValue(330, _("0330"));
    VoiceGrid->SetRowLabelValue(331, _("0331"));
    VoiceGrid->SetRowLabelValue(332, _("0332"));
    VoiceGrid->SetRowLabelValue(333, _("0333"));
    VoiceGrid->SetRowLabelValue(334, _("0334"));
    VoiceGrid->SetRowLabelValue(335, _("0335"));
    VoiceGrid->SetRowLabelValue(336, _("0336"));
    VoiceGrid->SetRowLabelValue(337, _("0337"));
    VoiceGrid->SetRowLabelValue(338, _("0338"));
    VoiceGrid->SetRowLabelValue(339, _("0339"));
    VoiceGrid->SetRowLabelValue(340, _("0340"));
    VoiceGrid->SetRowLabelValue(341, _("0341"));
    VoiceGrid->SetRowLabelValue(342, _("0342"));
    VoiceGrid->SetRowLabelValue(343, _("0343"));
    VoiceGrid->SetRowLabelValue(344, _("0344"));
    VoiceGrid->SetRowLabelValue(345, _("0345"));
    VoiceGrid->SetRowLabelValue(346, _("0346"));
    VoiceGrid->SetRowLabelValue(347, _("0347"));
    VoiceGrid->SetRowLabelValue(348, _("0348"));
    VoiceGrid->SetRowLabelValue(349, _("0349"));
    VoiceGrid->SetRowLabelValue(350, _("0350"));
    VoiceGrid->SetRowLabelValue(351, _("0351"));
    VoiceGrid->SetRowLabelValue(352, _("0352"));
    VoiceGrid->SetRowLabelValue(353, _("0353"));
    VoiceGrid->SetRowLabelValue(354, _("0354"));
    VoiceGrid->SetRowLabelValue(355, _("0355"));
    VoiceGrid->SetRowLabelValue(356, _("0356"));
    VoiceGrid->SetRowLabelValue(357, _("0357"));
    VoiceGrid->SetRowLabelValue(358, _("0358"));
    VoiceGrid->SetRowLabelValue(359, _("0359"));
    VoiceGrid->SetRowLabelValue(360, _("0360"));
    VoiceGrid->SetRowLabelValue(361, _("0361"));
    VoiceGrid->SetRowLabelValue(362, _("0362"));
    VoiceGrid->SetRowLabelValue(363, _("0363"));
    VoiceGrid->SetRowLabelValue(364, _("0364"));
    VoiceGrid->SetRowLabelValue(365, _("0365"));
    VoiceGrid->SetRowLabelValue(366, _("0366"));
    VoiceGrid->SetRowLabelValue(367, _("0367"));
    VoiceGrid->SetRowLabelValue(368, _("0368"));
    VoiceGrid->SetRowLabelValue(369, _("0369"));
    VoiceGrid->SetRowLabelValue(370, _("0370"));
    VoiceGrid->SetRowLabelValue(371, _("0371"));
    VoiceGrid->SetRowLabelValue(372, _("0372"));
    VoiceGrid->SetRowLabelValue(373, _("0373"));
    VoiceGrid->SetRowLabelValue(374, _("0374"));
    VoiceGrid->SetRowLabelValue(375, _("0375"));
    VoiceGrid->SetRowLabelValue(376, _("0376"));
    VoiceGrid->SetRowLabelValue(377, _("0377"));
    VoiceGrid->SetRowLabelValue(378, _("0378"));
    VoiceGrid->SetRowLabelValue(379, _("0379"));
    VoiceGrid->SetRowLabelValue(380, _("0380"));
    VoiceGrid->SetRowLabelValue(381, _("0381"));
    VoiceGrid->SetRowLabelValue(382, _("0382"));
    VoiceGrid->SetRowLabelValue(383, _("0383"));
    VoiceGrid->SetRowLabelValue(384, _("0384"));
    VoiceGrid->SetRowLabelValue(385, _("0385"));
    VoiceGrid->SetRowLabelValue(386, _("0386"));
    VoiceGrid->SetRowLabelValue(387, _("0387"));
    VoiceGrid->SetRowLabelValue(388, _("0388"));
    VoiceGrid->SetRowLabelValue(389, _("0389"));
    VoiceGrid->SetRowLabelValue(390, _("0390"));
    VoiceGrid->SetRowLabelValue(391, _("0391"));
    VoiceGrid->SetRowLabelValue(392, _("0392"));
    VoiceGrid->SetRowLabelValue(393, _("0393"));
    VoiceGrid->SetRowLabelValue(394, _("0394"));
    VoiceGrid->SetRowLabelValue(395, _("0395"));
    VoiceGrid->SetRowLabelValue(396, _("0396"));
    VoiceGrid->SetRowLabelValue(397, _("0397"));
    VoiceGrid->SetRowLabelValue(398, _("0398"));
    VoiceGrid->SetRowLabelValue(399, _("0399"));
    VoiceGrid->SetRowLabelValue(400, _("0400"));
    VoiceGrid->SetRowLabelValue(401, _("0401"));
    VoiceGrid->SetRowLabelValue(402, _("0402"));
    VoiceGrid->SetRowLabelValue(403, _("0403"));
    VoiceGrid->SetRowLabelValue(404, _("0404"));
    VoiceGrid->SetRowLabelValue(405, _("0405"));
    VoiceGrid->SetRowLabelValue(406, _("0406"));
    VoiceGrid->SetRowLabelValue(407, _("0407"));
    VoiceGrid->SetRowLabelValue(408, _("0408"));
    VoiceGrid->SetRowLabelValue(409, _("0409"));
    VoiceGrid->SetRowLabelValue(410, _("0410"));
    VoiceGrid->SetRowLabelValue(411, _("0411"));
    VoiceGrid->SetRowLabelValue(412, _("0412"));
    VoiceGrid->SetRowLabelValue(413, _("0413"));
    VoiceGrid->SetRowLabelValue(414, _("0414"));
    VoiceGrid->SetRowLabelValue(415, _("0415"));
    VoiceGrid->SetRowLabelValue(416, _("0416"));
    VoiceGrid->SetRowLabelValue(417, _("0417"));
    VoiceGrid->SetRowLabelValue(418, _("0418"));
    VoiceGrid->SetRowLabelValue(419, _("0419"));
    VoiceGrid->SetRowLabelValue(420, _("0420"));
    VoiceGrid->SetRowLabelValue(421, _("0421"));
    VoiceGrid->SetRowLabelValue(422, _("0422"));
    VoiceGrid->SetRowLabelValue(423, _("0423"));
    VoiceGrid->SetRowLabelValue(424, _("0424"));
    VoiceGrid->SetRowLabelValue(425, _("0425"));
    VoiceGrid->SetRowLabelValue(426, _("0426"));
    VoiceGrid->SetRowLabelValue(427, _("0427"));
    VoiceGrid->SetRowLabelValue(428, _("0428"));
    VoiceGrid->SetRowLabelValue(429, _("0429"));
    VoiceGrid->SetRowLabelValue(430, _("0430"));
    VoiceGrid->SetRowLabelValue(431, _("0431"));
    VoiceGrid->SetRowLabelValue(432, _("0432"));
    VoiceGrid->SetRowLabelValue(433, _("0433"));
    VoiceGrid->SetRowLabelValue(434, _("0434"));
    VoiceGrid->SetRowLabelValue(435, _("0435"));
    VoiceGrid->SetRowLabelValue(436, _("0436"));
    VoiceGrid->SetRowLabelValue(437, _("0437"));
    VoiceGrid->SetRowLabelValue(438, _("0438"));
    VoiceGrid->SetRowLabelValue(439, _("0439"));
    VoiceGrid->SetRowLabelValue(440, _("0440"));
    VoiceGrid->SetRowLabelValue(441, _("0441"));
    VoiceGrid->SetRowLabelValue(442, _("0442"));
    VoiceGrid->SetRowLabelValue(443, _("0443"));
    VoiceGrid->SetRowLabelValue(444, _("0444"));
    VoiceGrid->SetRowLabelValue(445, _("0445"));
    VoiceGrid->SetRowLabelValue(446, _("0446"));
    VoiceGrid->SetRowLabelValue(447, _("0447"));
    VoiceGrid->SetRowLabelValue(448, _("0448"));
    VoiceGrid->SetRowLabelValue(449, _("0449"));
    VoiceGrid->SetRowLabelValue(450, _("0450"));
    VoiceGrid->SetRowLabelValue(451, _("0451"));
    VoiceGrid->SetRowLabelValue(452, _("0452"));
    VoiceGrid->SetRowLabelValue(453, _("0453"));
    VoiceGrid->SetRowLabelValue(454, _("0454"));
    VoiceGrid->SetRowLabelValue(455, _("0455"));
    VoiceGrid->SetRowLabelValue(456, _("0456"));
    VoiceGrid->SetRowLabelValue(457, _("0457"));
    VoiceGrid->SetRowLabelValue(458, _("0458"));
    VoiceGrid->SetRowLabelValue(459, _("0459"));
    VoiceGrid->SetRowLabelValue(460, _("0460"));
    VoiceGrid->SetRowLabelValue(461, _("0461"));
    VoiceGrid->SetRowLabelValue(462, _("0462"));
    VoiceGrid->SetRowLabelValue(463, _("0463"));
    VoiceGrid->SetRowLabelValue(464, _("0464"));
    VoiceGrid->SetRowLabelValue(465, _("0465"));
    VoiceGrid->SetRowLabelValue(466, _("0466"));
    VoiceGrid->SetRowLabelValue(467, _("0467"));
    VoiceGrid->SetRowLabelValue(468, _("0468"));
    VoiceGrid->SetRowLabelValue(469, _("0469"));
    VoiceGrid->SetRowLabelValue(470, _("0470"));
    VoiceGrid->SetRowLabelValue(471, _("0471"));
    VoiceGrid->SetRowLabelValue(472, _("0472"));
    VoiceGrid->SetRowLabelValue(473, _("0473"));
    VoiceGrid->SetRowLabelValue(474, _("0474"));
    VoiceGrid->SetRowLabelValue(475, _("0475"));
    VoiceGrid->SetRowLabelValue(476, _("0476"));
    VoiceGrid->SetRowLabelValue(477, _("0477"));
    VoiceGrid->SetRowLabelValue(478, _("0478"));
    VoiceGrid->SetRowLabelValue(479, _("0479"));
    VoiceGrid->SetRowLabelValue(480, _("0480"));
    VoiceGrid->SetRowLabelValue(481, _("0481"));
    VoiceGrid->SetRowLabelValue(482, _("0482"));
    VoiceGrid->SetRowLabelValue(483, _("0483"));
    VoiceGrid->SetRowLabelValue(484, _("0484"));
    VoiceGrid->SetRowLabelValue(485, _("0485"));
    VoiceGrid->SetRowLabelValue(486, _("0486"));
    VoiceGrid->SetRowLabelValue(487, _("0487"));
    VoiceGrid->SetRowLabelValue(488, _("0488"));
    VoiceGrid->SetRowLabelValue(489, _("0489"));
    VoiceGrid->SetRowLabelValue(490, _("0490"));
    VoiceGrid->SetRowLabelValue(491, _("0491"));
    VoiceGrid->SetRowLabelValue(492, _("0492"));
    VoiceGrid->SetRowLabelValue(493, _("0493"));
    VoiceGrid->SetRowLabelValue(494, _("0494"));
    VoiceGrid->SetRowLabelValue(495, _("0495"));
    VoiceGrid->SetRowLabelValue(496, _("0496"));
    VoiceGrid->SetRowLabelValue(497, _("0497"));
    VoiceGrid->SetRowLabelValue(498, _("0498"));
    VoiceGrid->SetRowLabelValue(499, _("0499"));
    VoiceGrid->SetRowLabelValue(500, _("0500"));
    VoiceGrid->SetRowLabelValue(501, _("0501"));
    VoiceGrid->SetRowLabelValue(502, _("0502"));
    VoiceGrid->SetRowLabelValue(503, _("0503"));
    VoiceGrid->SetRowLabelValue(504, _("0504"));
    VoiceGrid->SetRowLabelValue(505, _("0505"));
    VoiceGrid->SetRowLabelValue(506, _("0506"));
    VoiceGrid->SetRowLabelValue(507, _("0507"));
    VoiceGrid->SetRowLabelValue(508, _("0508"));
    VoiceGrid->SetRowLabelValue(509, _("0509"));
    VoiceGrid->SetRowLabelValue(510, _("0510"));
    VoiceGrid->SetRowLabelValue(511, _("0511"));
    VoiceGrid->SetDefaultCellFont( VoiceGrid->GetFont() );
    VoiceGrid->SetDefaultCellTextColour( VoiceGrid->GetForegroundColour() );
    ButtonSauvegarder = new wxButton(Panel1, ID_BUTTONSAUVEGARDER, _("Sauvegarder et quitter"), wxPoint(264,488), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONSAUVEGARDER"));
    ButtonGenerer = new wxButton(Panel1, ID_BUTTONGENERER, _("Générer Fichiers"), wxPoint(152,488), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONGENERER"));

    Connect(ID_GRID1,wxEVT_GRID_SELECT_CELL,(wxObjectEventFunction)&VoiceEditFrame::OnVoiceGridCellSelect);
    Connect(ID_BUTTONSAUVEGARDER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&VoiceEditFrame::OnButtonSauvegarderClick);
    Connect(ID_BUTTONGENERER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&VoiceEditFrame::OnButtonGenererClick);
    //*)

    file = AppPath + "\\Voice_" + voice_Langue + ".csv";
    Load();
}

void VoiceEditFrame::Load()  //Load textfile language
{
    wxTextFile tfile(file);
    if (tfile.Exists()) //avoid crash if file is not found
    {
        tfile.Open(file);
        for (int j = 0; j < 512; j++ )
        {
            line = tfile.GetLine(j);
            wxStringTokenizer tokenizer(line, ";");

            wxString voiceText = tokenizer.GetNextToken();
            VoiceGrid->SetCellValue(j,0,voiceText);

            wxString voicePromp = tokenizer.GetNextToken();
            VoiceGrid->SetCellValue(j,1,voicePromp);
        }
        tfile.Close();
    }
}

VoiceEditFrame::~VoiceEditFrame()
{
    //(*Destroy(VoiceEditFrame)
//*)
}

/*void VoiceEditFrame::OnVoiceGridCellSelect(wxGridEvent& event)
{
    //voicePrompt = VoiceGrid->GetCellValue(event.GetRow(),event.GetCol());
    voicePrompt = VoiceGrid->GetCellValue(event.GetRow(),1);
    wxString quote = "\"";
    wxString VoiceCommandLine = AppPath + "\\tts.exe -f 2 -v 1 " + quote + voicePrompt + quote;
    wxExecute(VoiceCommandLine.c_str(), wxEXEC_HIDE_CONSOLE);
}*/

void VoiceEditFrame::OnButtonSauvegarderClick(wxCommandEvent& event)
{
    wxTextFile tfile(file);
    if (!tfile.Exists()) tfile.Create(); //avoid crash if file doesn't exist
    tfile.Open(file);
    tfile.Clear();
    for (int j = 0; j < 512; j++ )
    {
        voiceText = VoiceGrid->GetCellValue(j,0);
        voicePrompt = VoiceGrid->GetCellValue(j,1);
        line = voiceText + ";" + voicePrompt;
        tfile.AddLine(line);
    }
    tfile.Write();
    tfile.Close();
    Close();

}

void VoiceEditFrame::OnButtonGenererClick(wxCommandEvent& event)
{
  for (int j = 0; j < 12; j++ ) // cut for test purposes should be 512 /////////////////////////////////////////////
  {
    voicePrompt = VoiceGrid->GetCellValue(j,1);
    wxString label;
    label = VoiceGrid->GetRowLabelValue(j);
    wxString quote = "\"";
    wxString tts_o_look = " -o look ";
    wxString VoiceCommandLine = AppPath + "\\tts.exe -f 1 -v 1 " + quote + voicePrompt + quote + tts_o_look;
    wxExecute(VoiceCommandLine.c_str(), wxEXEC_HIDE_CONSOLE | wxEXEC_SYNC );
    rename ("look0.wav", label + ".wav");
    //wxExecute(sox --norm=-1 label + ".wav" silence 1 0.1 0.1% reverse highpass 300, wxEXEC_SYNC );
  }
  wxString audioFiles = AppPath + "\\_BuildAudioFiles.bat";
  wxExecute(audioFiles);
}



void VoiceEditFrame::OnVoiceGridCellSelect(wxGridEvent& event)
{
  {
    //voicePrompt = VoiceGrid->GetCellValue(event.GetRow(),event.GetCol());
    voicePrompt = VoiceGrid->GetCellValue(event.GetRow(),1);
    wxString quote = "\"";
    wxString VoiceCommandLine = AppPath + "\\tts.exe -f 2 -v 1 " + quote + voicePrompt + quote;
    wxExecute(VoiceCommandLine.c_str(), wxEXEC_HIDE_CONSOLE);
}
}
