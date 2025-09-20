#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CameraFmtDialog dialog

class CameraFmtDialog : public CDialog
{
	DECLARE_DYNAMIC(CameraFmtDialog)

public:
	CameraFmtDialog(bool IsVirtualDriverOpen, EHIDDeviceMode HIDMode, bool TouchHIDMode, bool TouchTUIOMode, CWnd* pParent = NULL);   // standard constructor
	virtual ~CameraFmtDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADANCEDSETTINGS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeCamerafmtCombo();
	afx_msg void OnBnClickedCheckHid();
	afx_msg void OnBnClickedCheckTuio();
	afx_msg void OnIpnFieldchangedIpaddressIp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPort();

	void    SetCameraResolution(std::vector<CAtlString>& CameraInfo, CAtlString  CurrentCameraInfo);
	CAtlString  GetSelectComboxvalue();
	bool    GetTouchHIDMode();
	bool    GetTouchTUIOMode();
	EHIDDeviceMode GetDeviceMode();

	void   SetTUIOParams(DWORD IP, int nPort,int nScreenWindth ,int nScreenHeight);
	DWORD  GetIPAddress();
	int    GetPort();
	int    GetScreenWidth();
	int    GetScreenHeight();
	void   InitDeviceUseMode();
	void   InitDeviceUseModeExtern(bool IsVirtualDriverOpen, EHIDDeviceMode HIDMode, bool TouchHIDMode, bool TouchTUIOMode);
	DECLARE_MESSAGE_MAP()

public:
	CComboBox  m_CCameraFmt;
	std::vector<CAtlString> CameraAllResution;
	CAtlString   m_sCurrentCameraResution;
	CString m_SelectedCameraFmtValue;

	bool m_IsVirtualDriverOpen;
	//EHIDDeviceMode m_HIDMode;
	bool m_TouchHIDMode;
	bool m_TouchTUIOMode;

	int m_bHIDMode;
	//BOOL m_bTUIOMode;
	DWORD m_IPAddress;
	CString m_strPort;
	CIPAddressCtrl CIPAddress;
	CEdit CPortEdit;
	EHIDDeviceMode m_eDeviceMode;

	int m_nTUIOScreenWidth;
	int m_nTUIOScreenHeight;
	afx_msg void OnBnClickedButtonDefault();
	afx_msg void OnBnClickedRadioTouchpad();
};
