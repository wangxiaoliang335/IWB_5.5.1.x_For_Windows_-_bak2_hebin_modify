#pragma once
//@����:�ڸ�����Ļ��ͨ����ʾ������ʶ��ͬ����Ļ��
class CScreenRecognition
{
public:
    //@����:clrText, ȱʡ��������ɫ��
	CScreenRecognition(COLORREF clrText = RGB(255, 0, 0));
	~CScreenRecognition();

	LRESULT  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void DoRecoginition(const CIWBSensorManager* pIWBSensorManager);

    void StopRecognition()
    {
        ShowWindow(this->m_hWnd, SW_HIDE);
    }


protected:
    HBRUSH m_hbrBackground;
    HWND  m_hWnd          ;


    int   m_nStepTimeout  ;//��������ʱ���
    RECT  m_rcWndArea     ;//
    HFONT m_hFont         ;//������ʾ������
    HFONT m_hFontOld      ;//
    COLORREF m_clrText    ;//������ɫ

    //static 	const TCHAR* m_szClassName;

    //static const int DEFAULT_WIDTH  = 100;
    //static const int DEFAULT_HEIGHT = 100;
    HINSTANCE m_hInst;
    HDC       m_hMemDC;
    HBITMAP   m_hBitmap;
    HBITMAP   m_hBitmapOld;
    HPEN      m_hPen;
    HPEN      m_hPenOld;
    //CDispMonitorFinder m_oDispMonitorFinder;
    static const LONG DEFAULT_PEN_WIDTH = 1;

};