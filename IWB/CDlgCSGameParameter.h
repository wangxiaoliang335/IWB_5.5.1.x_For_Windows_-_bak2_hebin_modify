#pragma once


// CDlgCSGameParameter dialog

class CDlgCSGameParameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCSGameParameter)

public:
	CDlgCSGameParameter(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgCSGameParameter();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CS_GAME_PARAMETER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
