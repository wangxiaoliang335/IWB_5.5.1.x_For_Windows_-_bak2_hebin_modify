// OpticalPen.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "UsbCameraDeviceList.h"
// CIWBApp:
// See OpticalPen.cpp for the implementation of this class

#include "../inc/EasiSDKReg_Interface.h"

#include <unordered_map>
//@����:����ʵ�ʵĴ�������
EDeviceTouchType GetActualTouchType();

//@��  ��:����ʵ�ʵ���Ļ��������
//@��  ��:
//@����ֵ:
SIZE GetActualScreenControlSize();

struct USBKeyInformation
{
    EDeviceTouchType        eUSBKeyTouchType     ;//
    EPalmTouchControlType   ePalmTouchControlType;//
    EFingerTouchControlType eFingerTouchControlType;
    EScreenMode             eScreenModeFromUsbKey;//
    BOOL                    bEnableFourPointCalibrate;

    USBKeyInformation()
    {
        eUSBKeyTouchType        = E_DEVICE_NOT_FOUND;
        ePalmTouchControlType   = E_PLAM_TOUCHCONTROL_UNKNOWN;
        eFingerTouchControlType = E_FINGER_TOUCHCONTROL_UNKNOWN;
        eScreenModeFromUsbKey   = EScreenModeSingle;
        bEnableFourPointCalibrate = FALSE;
    }
};


struct TAutoCalibrateCompensateData
{
    TAutoCalibCompCoefs coefs;//�Զ�����ϵ��
    double throwRatioOfLens  ;//������ͷ��Ͷ���	
};

extern char szUSBBuffer[1024];
extern wchar_t szUSBRegBuffer[1024];
extern HRESULT g_hr13;

class CIWBApp : public CWinApp
{
public:
    CIWBApp();

    ~CIWBApp();
    // Overrides

    virtual BOOL InitInstance();

    BOOL IsStartFromAutoRun()const
    {
        return m_bAutoRunMode;
    }

    BOOL IsForAllUsers()const
    {
        return m_bForAllUser;
    }


    EDeviceTouchType  GetUSBKeyTouchType() const;
    EPalmTouchControlType GetPalmTouchType()const;
    EFingerTouchControlType GetFingerTouchType() const;

    BOOL IsFourPointCalibrateEnabled()const;
    //int  GetAllUSBKeyTouchTypeCount();
    //const USBKeyInformation* GetAllUSBKeyInformations() const;
    const  std::unordered_map<std::string, USBKeyInformation>& GetAllUSBKeyInformations() const;
    //int GetDoubleScreenMerge() const
    //{
    //  return m_nDoubleScreenType ;
    //}

    //���ٰ汾
    EScreenMode GetScreenMode() const ;

    //���ٰ汾
    EScreenMode ScreenMode() const
    {
        return m_eScreenMode;
    }

    EScreenMode GetScreenModeFromUSBKey()const;

    int GetScreenCount() const;
    int GetScreenModeFromUSBKeyCount()const;

    BOOL IsHardwareKeyExist()const
    {
        //return m_bFoundHardwareUSBKey;
        return m_uHardwareUSBKeyCount > 0 ? TRUE : FALSE;
    }

    BOOL IsOnlineRegistered()const
    {
        return m_bIsOnlineRegistered;
    }
    LPCTSTR GetLangeCode()const
    {
        return (LPCTSTR)m_strLanguageCode;
    }
    // Implementation
    BOOL ParseCmdLine(LPCTSTR lpCmdLine);

    //@����: ����OEM��Դ��
    //@˵��:OEM, Original Equipment Manufacture
    //OEM,  manufactures products or components that are purchased by a company 
    //      and retailed under that purchasing company's brand name.
    //
    HINSTANCE LoadOEMResource();


    //<<added by toxuke@gmail.com, 2013/10/10
    //@����:��ʼ������������·������
    //      ���������ļ���Ŀ¼·���͵�������ļ������Ŀ¼·����
    void  InitDirectoryInformation();

    CDispDevFinder& GetMonitorFinder() { return m_oDispMonitorFinder; }

    CString m_strSettingsDir;//�����ļ����Ŀ¼
    CString m_strSystemDir; //��װ������ϵͳĿ¼��λ��
    CString m_strIntermediatOutputDir;//��������ļ����Ŀ¼
    CString m_strFirmwareDirectory;//�̼��ļ�Ŀ¼

    //@����:��USBKey�ж�ȡ��Ϣ
    //@����:bFirstTime, ��һ�μ��UsbKey�Ĵ���
    //@˵��:��һ�μ��UsbKeyʱ�������Ի���, ����¼��־��Ϣ��
    //      �ڶ��μ��Ժ����ٵ����һ���
    BOOL ReadUSBKey(BOOL bFirstTime = FALSE);

    BOOL ReadUSBKeyData(UINT uKeyIndex);

    void OnPlugOutUSBKey(const char* szDevPath);
    void OnPlugInUSBKey(const char* szDevPath);

    //@����:�����豸·�������Զ�У׼����ϵ��
    const TAutoCalibrateCompensateData* GetCompensateData(const char* strDevPath)const;

    //@����:�����Զ�У׼����ϵ��
    void  ResetCompensateData();

    //@����:��ȡ�����Զ�����У��ϵ��
    void GetAllCompensateData(std::vector<TAutoCalibrateCompensateData>& compensateData);

    UINT& GetBetweenInstanceMsg()  { return  m_uBetweenInstanceMsg; }

protected:
    DECLARE_MESSAGE_MAP()


    HANDLE m_hMutex;
    BOOL   m_bAutoRunMode;
    BOOL   m_bForAllUser ;//�����û���־

    //int    m_nUSBKeyTouchType;////0:Ϊ3DTouch���Ӱװ�,1:Ϊ��ָ���ص��Ӱװ壬2:Ϊ������ָ���أ�3:Ϊ���ƻ���  
    //EDeviceTouchType m_eUSBKeyTouchType;//

   // EPalmTouchControlType    m_ePalmTouchControlType;   //���ƻ�������
    //EFingerTouchControlType  m_eFingerTouchControlType; //��ָ��������

    //std::vector<USBKeyInformation>  m_VecAllUsbKeyTouchType ;

    std::unordered_map<std::string, USBKeyInformation> m_AllUSbKeyInformations;
    
    //EScreenType    m_eScreenType; //0:Ϊ������1��Ϊ˫��

    //EScreenMode    m_eScreenModeFromUsbKey;//�ӻ��ܹ�ע����Ϣ�еõ�����Ļģʽ
   

    //BOOL           m_bFoundHardwareUSBKey;//����Ӳ��USB Key��־
    UINT           m_uHardwareUSBKeyCount;//����Ӳ��USB Key��־
    BOOL           m_bIsOnlineRegistered;//�Ƿ�����ע���ˡ�
    CString        m_strLanguageCode;//���Ա���

    CDispDevFinder m_oDispMonitorFinder;

    



    std::unordered_map<std::string, TAutoCalibrateCompensateData> m_allCompensateCoefs;
    
    
    
    //const static   int     COMPENSATE_NUM = 6;
    //double    m_pParams[COMPENSATE_NUM];
    //double    m_nCollectType;
    //static const int m_nStartDelayTime = 5000;//10000ms,��������ӳ�ʱ��

    UINT m_uBetweenInstanceMsg;

    EScreenMode m_eScreenMode;

};

extern CIWBApp theApp;