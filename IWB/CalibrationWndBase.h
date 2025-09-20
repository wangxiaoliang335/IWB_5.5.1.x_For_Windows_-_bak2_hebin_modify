#pragma once
class CCalibrationWndBase
{
public:
	CCalibrationWndBase();

	virtual ~CCalibrationWndBase();

	//�������ɺ���
	virtual BOOL Create();


	//@����:ȫ����ʾ����
	void FullScreen(BOOL bFull);

	BOOL IsFullScreen()const{ return m_bFullScreen;}

	HWND GetNotifyWnd()const;

	void SetNotifyWnd(HWND hWnd);

	static LRESULT CALLBACK StaticWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	operator HWND()const{return m_hWnd;}


protected:
	static const TCHAR* m_szClassName;
	static ATOM  m_atomClass;
	static long   m_lInstanceCount;
	HWND m_hWnd;
	HWND m_hNotifyWnd;//֪ͨ����,�������У����������Ϣ��

	WINDOWPLACEMENT m_OldWndPlacement;//����ԭʼλ��
	BOOL m_bFullScreen;//ȫ����־
	
};