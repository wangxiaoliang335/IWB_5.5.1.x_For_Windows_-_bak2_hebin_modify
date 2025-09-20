#pragma once
#include "..\inc\GlobalDefine.h"
#include "..\inc\EASI_TOUCHSCREEN_INTERFACE.h"
#include "DeviceEventMonitor.h"
#include "TouchScreenDblChecker.h"
#include "VirtualTUIO.h"
#include "TouchTester.h"
#include "TouchInjector.h"

//�ο�:"Processing a Request to Remove a Device"ʵʱ����豸������״̬��
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa363215(v=vs.85).aspx

//����HID�豸
class CVirtualHID
{
public:

	CVirtualHID();

	~CVirtualHID();

	BOOL OpenDevice();

	BOOL CloseDevice();

	//@����:״̬��λ
	//@����:
	//@˵��:�����豸���а��µĵ㽫����
	//      ����豸�İ���ȫ������
	void Reset();

	//@����:ģ��HID����
	//@����:penInfos, ָ���������ָ��
	//      nPenCount, ���֧��
	BOOL InputPoints(TContactInfo* pPenInfos, int nPenCount, TContactInfo* pPenInfos1, int nPenCount1, DWORD dwCameraId = -1, int nMonitorId = -1);

	//@����:�������/����ģʽ
	void SetHIDMode(EHIDDeviceMode eMode) 
	{ 
		m_eHIDDeviceMode = eMode; 
		m_eDesiredHIDMode = eMode;
	}
	EHIDDeviceMode GetHIDMode()const { return m_eHIDDeviceMode; }

	//@���ܣ�����HID����ģʽ
	void SetTouchHIDMode(bool  eMode);
	bool GetTouchHIDMode()const {return m_bTouchHIDMode;}
	//@���ܣ�����TUIO����ģʽ
	void SetTouchTUIOMode(bool  eMode);
	bool GetTouchTUIOMode()const { return m_bTouchTUIOMode; }
	////@���ܣ����ô�������ģʽ
	void SetSinglePointMode(bool eMode);
	bool GetSinglePointMode()const { return m_bSinglePointMode; }

	/////////////////
	void    SetTUIOParams(DWORD IP, int nPort, int nScreenWindth, int nScreenHeight);
	void    OpenTUIOServer(bool bStart);

	void	OpenTUIOServer1(bool bStart);

	DWORD   GetIPadress();
	int     GetPort();
	int     GetScreenWidth();
	int     GetScreenHeight();
	void    SetTUIOScreenDisplayChange(int nScreenX, int nScreenY);

	static CVirtualMouse& GetVirtualMouse() { return m_oVirtualMouse; }

	//@����:�ж����������Ƿ��Ǵ򿪵ġ�
	BOOL IsVirtualDriverOpen();

	//@����:���ô����ĳߴ��¼���Ӧ����
	//@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
	//      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
	void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

    //@����:���´�����������Ļ��Ϣ
    void UpdateAttachedMonitorInfo();
	//@����
	void SetTest30Point(BOOL bStart);
	BOOL GetTest30Point();

	void SetAirOperateMode(BOOL eMode, EAIROPERATE_CLICKMODE  eClickMode);

protected:
	//@����:ģ�ⴥ������
	//@����:penInfos, ָ���������ָ��
	//      nPenCount, ���֧��
	BOOL InputTouchPoints(const TContactInfo* pPenInfos, int nPenCount, int dwCameraId);

	BOOL InputTouchPoints_AirOperate(const TContactInfo* pPenInfos, int nPenCount, EAIROPERATE_CLICKMODE  eClickMode);

	//@����:����ָ���豸��Ϣ
	BOOL RetrievePointerDevices();

	//@����:���ݲ�Ʒ�ַ�����ȡ��Ӧ��ָ���豸��Ϣ��
	//@����:lpszProductString, ��Ʒ�ַ���
	//@����ֵ:NULL, δ�ҵ���
	//        ������ָ��POINTER_DEVICE_INFO������ָ��
	POINTER_DEVICE_INFO* GetPointerDevice(LPCTSTR lpszProductString = NULL);
    std::vector<POINTER_DEVICE_INFO> m_vecPointerDeviceInfos;

	HANDLE m_hDev;
	EHIDDeviceMode m_eHIDDeviceMode;

	//////////////add by zhaown 2019.09.26
	CVirtualTUIO  m_oVirtualTUIOTouch;
	TouchInjector m_oTouchInjector;//Windows APIע�봥���¼���

	bool     m_bTouchHIDMode  ;
	bool     m_bTouchTUIOMode ;

	bool     m_bSinglePointMode;  //�Ƿ���������Ӧ
	BOOL     m_bStartTest30Point; //�����������ϵͳ�Ƿ�֧����ʮ�㴥��
	CTouchTester  m_oTouchTester;

	BOOL     m_bAirOperationMode;
	EAIROPERATE_CLICKMODE  m_eClickMode;


	static const int MAX_TOUCH_POINT_COUNT = MAX_SUPPORT_TOUCH_COUNT;
	EASI_TouchPoint m_TouchPoints[MAX_TOUCH_POINT_COUNT];

	static CVirtualMouse m_oVirtualMouse;//����������


	//����:�豸��������¼��ص��ӿ�
	static void  DeviceHandleEventCallBack(const DeviceHandleMonitorEntry* pEntry, WPARAM dbtEvent);

	//@����:HID�豸�ӿڹ����¼��ص�����
	static void  DeviceInterfaceEventCallBack(const DeviceIntetfaceMonitorEntry* pEntry, WPARAM dbtEvent);

	CDeviceEventMonitor m_oDeviceEventDetector;

	//<<added by toxuke@gmail.com, 205/01/30
	CTouchScreenDblClickChecker<MAX_TOUCH_POINT_COUNT> m_oDblClickChecker;//˫�������
	//>>

	//{{
	//2018/01/04
	//Ϊ�˽���ڿ����Զ�����ʱ, ���ⴥ���豸��ʧ�ܵ�����
	//�ڴ��豸�豸ʧ��ʱ, ����һ���̲߳��ϳ��Դ򿪴����豸��ֱ���ɹ����ߴﵽ�������Ŀ
	CComAutoCriticalSection m_csForVirtualDevice;
	BOOL OpenDeviceThreadSafe();

	BOOL CloseDeviceThreadSafe();


	static DWORD WINAPI AutoOpenThreadProc(LPVOID lpCtx);
	HANDLE m_hAutoOpenThread;
	BOOL   m_bAutoOpenThreadExit;

	EHIDDeviceMode m_eDesiredHIDMode;//��Ҫ��ģʽ

	BOOL CreateAutoOpenThread();
	void CloseAutoOpenThread();

	//}}

    //������߱�
    int m_aspectRatioNominator   = 16;
    int m_aspectRatioDenominator = 9;

    //���������ݵ���ģ��
    enum
    {   //����ָ���Ŀ�߱Ƚ�������
        E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO,
        //��ʾ��Ƕ���ڴ�����
        E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL
    }m_eTouchDataAdjustModel;
};

inline CVirtualMouse& GetVirtualMouse()
{
    return  CVirtualHID::GetVirtualMouse();
}