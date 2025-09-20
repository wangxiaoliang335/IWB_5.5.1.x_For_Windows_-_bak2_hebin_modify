// CameraFmtDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IWB.h"
#include "CameraFmtDialog.h"
#include "afxdialogex.h"

// CameraFmtDialog dialog

IMPLEMENT_DYNAMIC(CameraFmtDialog, CDialog)

CameraFmtDialog::CameraFmtDialog(bool IsVirtualDriverOpen, EHIDDeviceMode HIDMode, bool TouchHIDMode, bool TouchTUIOMode, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_ADANCEDSETTINGS, pParent)
	, m_sCurrentCameraResution(_T(""))
	, m_SelectedCameraFmtValue(_T(""))
	, m_bHIDMode(0)
	//, m_bTUIOMode(FALSE)
	, m_IPAddress(0)
	, m_strPort(_T(""))
	, m_nTUIOScreenWidth(0)
	, m_nTUIOScreenHeight(0)
{
	m_IsVirtualDriverOpen = IsVirtualDriverOpen;
	m_eDeviceMode = HIDMode;
	m_TouchHIDMode = TouchHIDMode;
	m_TouchTUIOMode = TouchTUIOMode;

	if (m_TouchHIDMode)
	{
		if (m_IsVirtualDriverOpen)
		{
			EHIDDeviceMode eDeviceMode = m_eDeviceMode;
			if (eDeviceMode == E_DEV_MODE_MOUSE)
			{
				m_bHIDMode = 1;
			}
			else if (eDeviceMode == E_DEV_MODE_TOUCHSCREEN)
			{
				m_bHIDMode = 0;
			}
		}
		else
		{//当"虚拟驱动"未打开, 则强制使用Windows API模拟鼠标消息。
		 //强制使用鼠标模式
			m_bHIDMode = 1;
		}
	}
	else
	{
		m_bHIDMode = 2;
	}

	////////////////add by zhaown if是HID模式，按原来的走，如果只是TUIO模式，那么鼠标和触屏选项要灰掉
	bool bHIDMode = m_TouchHIDMode;
	bool bTUIOMode = m_TouchTUIOMode;
	//只有TUIO模式，HID模式选择全部灰掉
	if (!bHIDMode && bTUIOMode)
	{
	}
}

CameraFmtDialog::~CameraFmtDialog()
{
}

void CameraFmtDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERAFMT_COMBO, m_CCameraFmt);
	DDX_CBString(pDX, IDC_CAMERAFMT_COMBO, m_SelectedCameraFmtValue);
	DDX_Radio(pDX, IDC_RADIO_TOUCHPAD, m_bHIDMode);
	//DDX_Radio(pDX, IDC_RADIO_TUIO, m_bTUIOMode);
	DDX_IPAddress(pDX, IDC_IPADDRESS_IP, m_IPAddress);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strPort);
	DDX_Control(pDX, IDC_IPADDRESS_IP, CIPAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, CPortEdit);
	DDX_Text(pDX, IDC_EDIT_SCREENWIDTH, m_nTUIOScreenWidth);
	DDX_Text(pDX, IDC_EDIT_SCREENHEIGHT, m_nTUIOScreenHeight);
}


BEGIN_MESSAGE_MAP(CameraFmtDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CameraFmtDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CameraFmtDialog::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_CAMERAFMT_COMBO, &CameraFmtDialog::OnCbnSelchangeCamerafmtCombo)
	ON_BN_CLICKED(IDC_RADIO_HID, &CameraFmtDialog::OnBnClickedCheckHid)
	ON_BN_CLICKED(IDC_RADIO_TUIO, &CameraFmtDialog::OnBnClickedCheckTuio)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_IP, &CameraFmtDialog::OnIpnFieldchangedIpaddressIp)
	ON_EN_CHANGE(IDC_EDIT_PORT, &CameraFmtDialog::OnEnChangeEditPort)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CameraFmtDialog::OnBnClickedButtonDefault)
	ON_BN_CLICKED(IDC_RADIO_TOUCHPAD, &CameraFmtDialog::OnBnClickedRadioTouchpad)
END_MESSAGE_MAP()


BOOL CameraFmtDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	if (CameraAllResution.empty())
	{
		m_CCameraFmt.AddString(_T("--------"));
		m_CCameraFmt.SetCurSel(0);
		m_SelectedCameraFmtValue = "";
	}
	else
	{
		for (UINT i = 0 ; i <CameraAllResution.size();i++ )
		{
			m_CCameraFmt.AddString((LPCTSTR)CameraAllResution[i]);
	    }
	    int index = m_CCameraFmt.SelectString(0, (LPCTSTR)m_sCurrentCameraResution);
	    m_CCameraFmt.SetCurSel(index);
	    m_SelectedCameraFmtValue = m_sCurrentCameraResution;
	}

	unsigned   char   *pIP = (unsigned   char*)&m_IPAddress;	
	CIPAddress.SetAddress(*pIP ,*(pIP + 1), *(pIP + 2),*(pIP + 3) );

	if (theApp.GetUSBKeyTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD || theApp.GetUSBKeyTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
	{
		GetDlgItem(IDC_RADIO_TUIO)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_HID)->EnableWindow(false);
		GetDlgItem(IDC_IPADDRESS_IP)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_SCREENWIDTH)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_SCREENHEIGHT)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_DEFAULT)->EnableWindow(false);
	}
	else
	{
	    //((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(g_tSysCfgData.globalSettings.bTouchHIDMode);
	    ((CButton*)GetDlgItem(IDC_RADIO_TUIO))->SetCheck(g_tSysCfgData.globalSettings.bTouchTUIOMode);
		if(!g_tSysCfgData.globalSettings.bTouchTUIOMode)
		{
			GetDlgItem(IDC_IPADDRESS_IP)->EnableWindow(false);
			GetDlgItem(IDC_EDIT_PORT)->EnableWindow(false);
		}
	}
	if (!g_tSysCfgData.globalSettings.bModifyResolution)
	{
		GetDlgItem(IDC_CAMERAFMT_COMBO)->EnableWindow(false);
	}

	InitDeviceUseMode();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CameraFmtDialog::SetCameraResolution(std::vector<CAtlString>& CameraInfo, CAtlString  CurrentCameraInfo)
{
	CameraAllResution = CameraInfo;
	m_sCurrentCameraResution = CurrentCameraInfo;
}

void CameraFmtDialog::SetTUIOParams(DWORD IP, int nPort, int nScreenWindth, int nScreenHeight)
{
	m_IPAddress = IP;
	m_strPort.Format(_T("%d"), nPort);
	m_nTUIOScreenWidth =  nScreenWindth;
	m_nTUIOScreenHeight = nScreenHeight;
}

void CameraFmtDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDialog::OnOK();
}


void CameraFmtDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CameraFmtDialog::OnCbnSelchangeCamerafmtCombo()
{
	// TODO: Add your control notification handler code here
	m_SelectedCameraFmtValue =_T("");
	m_CCameraFmt.GetLBText(m_CCameraFmt.GetCurSel(), m_SelectedCameraFmtValue);
	m_CCameraFmt.SetWindowTextW(m_SelectedCameraFmtValue);
}

CAtlString  CameraFmtDialog::GetSelectComboxvalue()
{
	return  m_SelectedCameraFmtValue;
}

DWORD CameraFmtDialog::GetIPAddress()
{
	return m_IPAddress;
}

int CameraFmtDialog::GetPort()
{
	return  _ttoi(m_strPort);
}

int CameraFmtDialog::GetScreenWidth()
{
	return m_nTUIOScreenWidth;
}

int CameraFmtDialog::GetScreenHeight()
{
	return m_nTUIOScreenHeight;
}

void CameraFmtDialog::OnBnClickedCheckHid()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_HID))->GetCheck())
	{
     	((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(true);
		m_bHIDMode = 1;
		m_eDeviceMode = E_DEV_MODE_MOUSE;

		CIPAddress.EnableWindow(FALSE);
		CPortEdit.EnableWindow(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(false);
		m_bHIDMode = 1;
	}
}


void CameraFmtDialog::OnBnClickedCheckTuio()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_TUIO))->GetCheck())
	{
	     ((CButton*)GetDlgItem(IDC_RADIO_TUIO))->SetCheck(true);
		 //m_bTUIOMode = TRUE;
		 m_bHIDMode = 2;
		 CIPAddress.EnableWindow(true);
		 CPortEdit.EnableWindow(true);
	}
	else {
		((CButton*)GetDlgItem(IDC_RADIO_TUIO))->SetCheck(false);
		//m_bTUIOMode = FALSE;
		m_bHIDMode = 0;

		//////配置IP和端口都要灰掉
		CIPAddress.EnableWindow(false);
		CPortEdit.EnableWindow(false);

	}
}

void CameraFmtDialog::InitDeviceUseMode()
{
	if (m_TouchHIDMode)
	{
		if (m_IsVirtualDriverOpen)
		{
			////pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_ENABLED);
			//GetDlgItem(IDC_RADIO_HID)->EnableWindow(true);
			////pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_ENABLED);
			//GetDlgItem(IDC_RADIO_TOUCHPAD)->EnableWindow(true);
			EHIDDeviceMode eDeviceMode = m_eDeviceMode;
			if (eDeviceMode == E_DEV_MODE_MOUSE)
			{
				//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_CHECKED);
				//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_UNCHECKED);
				((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(TRUE);
				((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(FALSE);
				m_bHIDMode = 1;
			}
			else if (eDeviceMode == E_DEV_MODE_TOUCHSCREEN)
			{
				//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_UNCHECKED);
				//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_CHECKED);
				((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(FALSE);
				((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(TRUE);
				m_bHIDMode = 0;
			}
		}
		else
		{//当"虚拟驱动"未打开, 则强制使用Windows API模拟鼠标消息。
		 //强制使用鼠标模式
			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_CHECKED);
			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_UNCHECKED);

			((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(TRUE);
			((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(FALSE);
			m_bHIDMode = 1;

			//////       pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND| MF_GRAYED);
			////pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_GRAYED);
			//((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->EnableWindow(FALSE);
		}
	}
	else
	{
		m_bHIDMode = 2;
	}

	////////////////add by zhaown if是HID模式，按原来的走，如果只是TUIO模式，那么鼠标和触屏选项要灰掉
	bool bHIDMode = m_TouchHIDMode;
	bool bTUIOMode = m_TouchTUIOMode;
	//只有TUIO模式，HID模式选择全部灰掉
	if (!bHIDMode && bTUIOMode)
	{
		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_GRAYED);
		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_GRAYED);
		//GetDlgItem(IDC_RADIO_HID)->EnableWindow(FALSE);
		//((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->EnableWindow(FALSE);
	}
}

//void CameraFmtDialog::InitDeviceUseMode()
//{
//	if (m_IsVirtualDriverOpen)
//	{
//		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_ENABLED);
//		GetDlgItem(IDC_RADIO_HID)->EnableWindow(true);
//		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_ENABLED);
//		GetDlgItem(IDC_RADIO_TOUCHPAD)->EnableWindow(true);
//		EHIDDeviceMode eDeviceMode = m_HIDMode;
//		if (eDeviceMode == E_DEV_MODE_MOUSE)
//		{
//			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_CHECKED);
//			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_UNCHECKED);
//			((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(TRUE);
//			((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(FALSE);
//		}
//		else if (eDeviceMode == E_DEV_MODE_TOUCHSCREEN)
//		{
//			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_UNCHECKED);
//			//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_CHECKED);
//			((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(FALSE);
//			((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(TRUE);
//		}
//	}
//	else
//	{//当"虚拟驱动"未打开, 则强制使用Windows API模拟鼠标消息。
//	 //强制使用鼠标模式
//		//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_CHECKED);
//		//pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_UNCHECKED);
//
//		((CButton*)GetDlgItem(IDC_RADIO_HID))->SetCheck(TRUE);
//		((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(FALSE);
//
//		////       pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND| MF_GRAYED);
//		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_GRAYED);
//		((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->EnableWindow(FALSE);
//	}
//
//	////////////////add by zhaown if是HID模式，按原来的走，如果只是TUIO模式，那么鼠标和触屏选项要灰掉
//	bool bHIDMode = m_TouchHIDMode;
//	bool bTUIOMode = m_TouchTUIOMode;
//	//只有TUIO模式，HID模式选择全部灰掉
//	if (!bHIDMode && bTUIOMode)
//	{
//		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_GRAYED);
//		//pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_GRAYED);
//		GetDlgItem(IDC_RADIO_HID)->EnableWindow(FALSE);
//		((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->EnableWindow(FALSE);
//	}
//}

void CameraFmtDialog::InitDeviceUseModeExtern(bool IsVirtualDriverOpen, EHIDDeviceMode HIDMode, bool TouchHIDMode, bool TouchTUIOMode)
{
	m_IsVirtualDriverOpen = IsVirtualDriverOpen;
	m_eDeviceMode = HIDMode;
	m_TouchHIDMode = TouchHIDMode;
	m_TouchTUIOMode = TouchTUIOMode;
}

bool CameraFmtDialog::GetTouchHIDMode()
{
	return (m_bHIDMode==0|| m_bHIDMode==1)?true:false;
}

bool CameraFmtDialog::GetTouchTUIOMode()
{
	return (m_bHIDMode==2)?true:false;
}

EHIDDeviceMode CameraFmtDialog::GetDeviceMode()
{
	return m_eDeviceMode;
}

void CameraFmtDialog::OnIpnFieldchangedIpaddressIp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true);
	*pResult = 0;
}


void CameraFmtDialog::OnEnChangeEditPort()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(true);
}


void CameraFmtDialog::OnBnClickedButtonDefault()
{
	// TODO: Add your control notification handler code here

	CString  strScreen;
	strScreen.Format(_T("%d"), GetSystemMetrics(SM_CXSCREEN));
	GetDlgItem(IDC_EDIT_SCREENWIDTH)->SetWindowTextW(strScreen);

	strScreen.Format(_T("%d"), GetSystemMetrics(SM_CYSCREEN));
	GetDlgItem(IDC_EDIT_SCREENHEIGHT)->SetWindowTextW(strScreen);

}


void CameraFmtDialog::OnBnClickedRadioTouchpad()
{
	// TODO: 在此添加控件通知处理程序代码
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(true);
		m_bHIDMode = 0;
		m_eDeviceMode = E_DEV_MODE_TOUCHSCREEN;

		CIPAddress.EnableWindow(FALSE);
		CPortEdit.EnableWindow(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_TOUCHPAD))->SetCheck(false);
		m_bHIDMode = 1;
	}
}
