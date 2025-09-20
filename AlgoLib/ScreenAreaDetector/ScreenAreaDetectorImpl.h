#pragma once
#include "../../inc/ImageFrame.h"
#include "../../inc/NewFrameSink.h"
class CScreenAreaDetectorImpl:public INewFrameEventSink
{
public:
	CScreenAreaDetectorImpl();
	~CScreenAreaDetectorImpl();

	//@�� ��:�������,ͬʱ��ʾȫ������
	//@�� ��:hNotifyWnd, ����������֪ͨ�Ĵ�����
	//@����ֵ;	
	BOOL StartDetect(HWND hNotifyWnd);
	
	//@��  ��:������⹦��,
	//@��  ��:
	//@����ָ:
	BOOL End();


	//@����:������Ļ��������λͼ
	const CImageFrame& GetMaskFrame()const;


	//@����:�µ�ͼƬ֡�¼���Ӧ
	virtual void OnNewFrame(CImageFrame* pFrame);



protected:
	//BOOL m_bIsDetecting;//��������־
	//�װ崰����غ���
	BOOL (CreateWindow)();

	//@����:�жϰװ崰���Ƿ�ɼ�
	BOOL IsBoardWindowVisible();

	HRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//״̬��״̬
	enum E_STAGE
	{
		E_STAGE_INITIAL = 0      ,//��ʼ�׶�
        E_STAGE_WHITE_FRAME      ,//������ʼ
		E_STAGE_BLACK_FRAME      ,//�����׶�
		E_STAGE_CALC             ,//����׶�
		E_STAGE_END              ,//������
	}m_eStage;

	int  m_nStageCount;//�׶μ�����
	static const int S_WAITCOUNT = 50;//

	BYTE m_cBrightness;//ȫ��������������, 0~255.

	//CImageFrame m_oGayFrame   ;//�Ҷ�ͼƬ֡
	CImageFrame m_oBlackFrame    ;//����ͼƬ֡
	CImageFrame m_oWiteFrame     ;//����ͼƬ֡


	CImageFrame m_oMaskFrame    ;//����λͼ
	HWND        m_hBoardWnd     ;//
	HWND        m_hNotifyWnd    ;//��Ϣ֪ͨ����
	HINSTANCE   m_hInst         ;//��Դʵ�����

	const TCHAR* m_szClassName   ;//��������

};