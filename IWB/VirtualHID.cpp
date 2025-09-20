#include "stdafx.h"
//#include "headers.h"
#include <VersionHelpers.h>
#pragma comment(lib, "..\\lib\\EASI_TOUCHSCREEN_INTERFACE.lib")

#include "../inc/RtlGetVersion.h"

#include "Hidsdi.h"
#pragma comment(lib,"Hid.lib")

//extern "C"
//{
//    #include <Hidsdi.h>
//}
//#pragma comment(lib, "hid.lib")
//#pragma comment(lib, "setupapi.lib")

//#include <initguid.h>
//DEFINE_GUID(GUID_DEVINTERFACE_EASI_UMDF_TouchScreen,
//    0xecf30a3b,0xd7d1,0x4580,0x9d,0x35,0xd1,0x70,0x0b,0xbf,0xe5,0xbb);
BOOL GetPointerDeviceDevPath(
    LPCTSTR lpszPointerDeviceProductString,
    LPTSTR lpszDevPathBuf,
    UINT uBufSize)
{
    GUID hidguid;
    HDEVINFO hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    SP_DEVINFO_DATA devInfoData;

    int i;

    BOOLEAN bFound = FALSE;
    BOOLEAN bSuccess = FALSE;

    HidD_GetHidGuid(&hidguid);

    hardwareDeviceInfo = SetupDiGetClassDevs(
        (LPGUID)&hidguid,
        NULL,
        NULL, // Define no
        (DIGCF_PRESENT |
            DIGCF_INTERFACEDEVICE));

    if (INVALID_HANDLE_VALUE == hardwareDeviceInfo) {
        return  FALSE;
    }


    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);



    PSP_DEVICE_INTERFACE_DETAIL_DATA    lpDeviceInterfaceDetailData = NULL;


    // Enumerate devices of this interface class
    for (i = 0;
        SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
            0, // No care about specific PDOs
            (LPGUID)&hidguid,
            i, //
            &deviceInterfaceData);
        i++)
    {
        DWORD predictedLength = 0;
        DWORD requiredLength = 0;
        BOOL deviceFound = FALSE;


        SetupDiGetDeviceInterfaceDetail(
            hardwareDeviceInfo,
            &deviceInterfaceData,
            NULL, // probing so no output buffer yet
            0, // probing so output buffer length of zero
            &requiredLength,
            NULL
        ); // not interested in the specific dev-node

        predictedLength = requiredLength;

        lpDeviceInterfaceDetailData =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(predictedLength);

        if (!lpDeviceInterfaceDetailData)
        {
            break;
        }

        lpDeviceInterfaceDetailData->cbSize =
            sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
            hardwareDeviceInfo,
            &deviceInterfaceData,
            lpDeviceInterfaceDetailData,
            predictedLength,
            &requiredLength,
            NULL))
        {
            break;
        }

        HANDLE file = INVALID_HANDLE_VALUE;
        file = CreateFile(lpDeviceInterfaceDetailData->DevicePath,
            0,// GENERIC_READ, ע��!!!��Ҫָ���κζ�дȨ�ޣ����߻�ʧ��
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, // no SECURITY_ATTRIBUTES structure
            OPEN_EXISTING, // No special create flags
            0, // No special attributes
            NULL); // No template file
        DWORD dwError = ERROR_SUCCESS;
        if (INVALID_HANDLE_VALUE == file) {
            //Try Next Device
            dwError = GetLastError();
            goto _check_next_device_;
        }

        TCHAR szProductString[1024];
        bSuccess = HidD_GetProductString(
            file,
            szProductString,
            _countof(szProductString));

        if (!bSuccess)
        {

            goto _check_next_device_;
        }
        else
        {
            if (_tcscmp(lpszPointerDeviceProductString, szProductString) == 0)
            {
                //\\?\hid#hidclass&col01#1&4784345&38&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
                //�������豸�������й涨��
                //&col01: collection 1, touchscreen;
                //&col02: collection 2, Standard HID device
                //&col03: collection 3, Mouse
                int nDevicePathLength = _tcslen(lpDeviceInterfaceDetailData->DevicePath);

                //�ַ���ȫ����Ϊ��д
                _tcsupr_s(
                    lpDeviceInterfaceDetailData->DevicePath,
                    nDevicePathLength + 1
                );

                if (_tcsstr(lpDeviceInterfaceDetailData->DevicePath, _T("\\\\?\\HID#HIDCLASS&COL01")))
                {//ȷ����TouchScreen�豸·��
                    _tcscpy_s(lpszDevPathBuf, uBufSize, lpDeviceInterfaceDetailData->DevicePath);

                    bFound = TRUE;
                }

            }

        }

    _check_next_device_:
        if (INVALID_HANDLE_VALUE != file)
        {
            CloseHandle(file);
        }
        if (lpDeviceInterfaceDetailData)
        {
            free(lpDeviceInterfaceDetailData);
            lpDeviceInterfaceDetailData = NULL;
        }

        if (bFound)
        {
            break;
        }

    }//for

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    return bSuccess;
}


//@����:�ж�ָ���Ĵ����豸�Ƿ����ָ������Ļ����
//@����:lpszPointerDevicePath, �����豸·����ȫ����д
//      lpszMonitorDevicePath, ��ʾ���豸·����ȫ����д
BOOL IsMonitorAttachedToPointerDevice(
    LPCTSTR lpszPointerDevicePath,
    LPCTSTR lpszMonitorDevicePath
)
{
    HKEY hKey;
    BOOL bAttached = FALSE;
    LSTATUS retCode = RegOpenKey(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Microsoft\\Wisp\\Pen\\Digimon"),
        &hKey);

    if (retCode != ERROR_SUCCESS)
    {
        return FALSE;
    }

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    //  TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    //  DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;


    TCHAR achData[MAX_VALUE_NAME];
    DWORD cchData = MAX_VALUE_NAME;


    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
    {
        cchValue = MAX_VALUE_NAME;
        achValue[0] = '\0';
        retCode = RegEnumValue(
            hKey,
            i,
            achValue,
            &cchValue,
            NULL,
            NULL,
            (LPBYTE)achData,
            &cchData);


        if (retCode == ERROR_SUCCESS && _tcsicmp(lpszMonitorDevicePath, achData) == 0)
        {//��������ʾ���豸·���ˡ�

            _tcsupr_s(
                achValue,
                cchValue + 1
            );

            if (_tcsstr(achValue, lpszPointerDevicePath))
            {//�����豸·��Ҳ������
                bAttached = TRUE;
                break;
            }
        }
    }
    RegCloseKey(hKey);

    return bAttached;
}

CVirtualMouse CVirtualHID::m_oVirtualMouse;


CVirtualHID::CVirtualHID()
    :
    m_hDev(INVALID_HANDLE_VALUE),
    //m_eHIDDeviceMode(E_DEV_MODE_TOUCHSCREEN)
    m_eHIDDeviceMode(E_DEV_MODE_MOUSE),
    m_hAutoOpenThread(NULL),
    m_bAutoOpenThreadExit(FALSE),
    m_aspectRatioNominator(16),
    m_aspectRatioDenominator(9),
    m_eTouchDataAdjustModel(E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO),
    m_bTouchHIDMode(true),
    m_bTouchTUIOMode(false),
    m_bSinglePointMode(false),
	m_bStartTest30Point(FALSE),
	m_bAirOperationMode(FALSE),
	m_eClickMode(E_MODE_CLICK)

{
    memset(&m_TouchPoints[0], 0, sizeof(m_TouchPoints));
    RetrievePointerDevices();    
}


CVirtualHID::~CVirtualHID()
{
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        EASI_CloseDevice(m_hDev);
    }
}


BOOL CVirtualHID::OpenDevice()
{
    UpdateAttachedMonitorInfo();

    //m_hDev = EASI_OpenDevice();
    CloseAutoOpenThread();

    //���Դ��豸
    OpenDeviceThreadSafe();

    //if(m_hDev != INVALID_HANDLE_VALUE)
    //{
    //    //���豸���������������
    //    m_oDeviceEventDetector.AddHandleMonitor(m_hDev, DeviceHandleEventCallBack, (LPVOID)this);
    //}
    //else
    if (INVALID_HANDLE_VALUE == m_hDev)
    {   //��ʱ�򲻿��豸, ��ʼ���HID�豸�¼����Ա�HID�豸�������ܹ����豸��
        AtlTrace(_T("EASI_OpenDevice rerurn INVALID_HANDLE_VALUE\n"));
        LOG_ERR("EASI_OpenDevice rerurn INVALID_HANDLE_VALUE\n");
        //GUID guid_HID;
        //HidD_GetHidGuid(&guid_HID);
        //m_oDeviceEventDetector.AddDevIntefaceMonitor(guid_HID, DeviceInterfaceEventCallBack,(LPVOID)this);
        const GUID& interface_guid = EASI_GetDeviceInterfaceGUID();
        m_oDeviceEventDetector.AddDevIntefaceMonitor(interface_guid, DeviceInterfaceEventCallBack, (LPVOID)this);

        //��Ҫ��ģʽ
        m_eDesiredHIDMode = m_eHIDDeviceMode;

        //�ڿ���������ʱ,����������豸ʧ�ܣ������̳߳����Զ���
        CreateAutoOpenThread();

        //�����豸�򲻿�����ǿ��ʹ�����ģʽ
        m_eHIDDeviceMode = E_DEV_MODE_MOUSE;
    }
    return INVALID_HANDLE_VALUE == m_hDev;
}

BOOL CVirtualHID::CloseDevice()
{
    CloseAutoOpenThread();
    return CloseDeviceThreadSafe();
}

//@����:״̬��λ
//@����:
//@˵��:�����豸���а��µĵ㽫����
//      ����豸�İ���ȫ������
void CVirtualHID::Reset()
{
    switch (m_eHIDDeviceMode)
    {
    case E_DEV_MODE_MOUSE://��λ���
        this->m_oVirtualMouse.Reset();
        break;

    case E_DEV_MODE_TOUCHSCREEN://��λ������
        if (INVALID_HANDLE_VALUE == m_hDev) return;

        //��λÿ��������
        for (int i = 0; i < _countof(m_TouchPoints); i++)
        {
            if (m_TouchPoints[i].bStatus == TIP_DOWN)
            {
                m_TouchPoints[i].bStatus = TIP_UP;
                EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[i], 1);
            }
        }//for(i)

        break;
    }//switch
}

// ת����Ļ���굽�����������ϵͳ(0-65535��Χ)
void ConvertScreenToInputCoord(int screenX, int screenY, int& inputX, int& inputY)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    inputX = (screenX * 65535) / screenWidth;
    inputY = (screenY * 65535) / screenHeight;
}

// ������������¼�
void SendMouseInput(DWORD flags, int x, int y)
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.dwFlags = flags;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    SendInput(1, &input, sizeof(INPUT));
}

// ģ����갴��(���)
void MouseDown(int x, int y)
{
    int inputX, inputY;
    ConvertScreenToInputCoord(x, y, inputX, inputY);

    // �ƶ���ָ��λ�ò�����
    SendMouseInput(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, inputX, inputY);
    Sleep(50); // �����ӳ�ȷ���ƶ����
    SendMouseInput(MOUSEEVENTF_LEFTDOWN, 0, 0);
    Sleep(50);
}

// ģ������ƶ�(���ְ���״̬)
void MouseMove(int x, int y)
{
    int inputX, inputY;
    ConvertScreenToInputCoord(x, y, inputX, inputY);

    // �ƶ����(���ְ���״̬)
    SendMouseInput(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, inputX, inputY);
}

// ģ����굯��(���)
void MouseUp()
{
    SendMouseInput(MOUSEEVENTF_LEFTUP, 0, 0);
    Sleep(50);
}

//@����:ģ��HID����
//@����:pPenInfos, ָ���������ָ��
//      nPenCount, ���֧��
BOOL CVirtualHID::InputPoints(TContactInfo* pPenInfos, int nPenCount, TContactInfo* pPenInfos1, int nPenCount1, DWORD dwCameraId, int nMonitorId)
{
    TSensorModeConfig* pSensorModeConfig = NULL;

    //g_tSysCfgData.vecSensorConfig[nMonitorId].nXCoordOffset;
    //pSensorModeConfig = &m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

    BOOL bRet = FALSE;
    //�豸δ�򿪣������Զ����߳�δ����
    if (m_hDev == INVALID_HANDLE_VALUE && this->m_hAutoOpenThread == NULL)
    {
        if (!OpenDevice())
        {
            return FALSE;
        }
    }

    //char szBuf[1024] = { 0 };
    //sprintf(szBuf, "aaaa InputPoints nMonitorId:%d, sendParams [0]:%d, [1]:%d\n", nMonitorId, sendParams[0], sendParams[1]);
    //OutputDebugStringA(szBuf);

    //if (nMonitorId >= 0)   //wxl modify 2024_12_3
    {
        for (int ii = 0; ii < nPenCount; ii++)
        {
            //if (m_bStart_Offset)
            //{
            //    pPenInfos[ii].pt.x -= sendParams[0];
            //    pPenInfos[ii].pt.y -= sendParams[1];

            //    //if (m_bTouchTUIOMode)
            //    //{
            //    //    // 1. ����ʼλ�ð������
            //    //    MouseDown(pPenInfos[ii].pt.x, pPenInfos[ii].pt.y);
            //    //    Sleep(50);
            //    //    // 4. �������
            //    //    MouseUp();
            //    //}
            //}
           
            //pPenInfos[ii].pt.x += g_tSysCfgData.vecSensorConfig[nMonitorId].nXCoordOffset/2 /** 5*/;
            //pPenInfos[ii].pt.y += g_tSysCfgData.vecSensorConfig[nMonitorId].nYCoordOffset/2 /** 5*/;
        }
    }

	if (m_bStartTest30Point)
	{
		if (nPenCount>0)
		{
		    m_oTouchTester.Process(pPenInfos);
		    const TContactInfo *pAllContactInfo = m_oTouchTester.GetContactInfo();

		    int nCount = m_oTouchTester.GetContactCount();

		    bRet = InputTouchPoints(pAllContactInfo, nCount, dwCameraId);
		}

		/////˵���Ѿ������ˣ���Reset����
		if (pPenInfos[0].ePenState == E_PEN_STATE_UP)
		{
            OutputDebugString(L" CVirtualHID InputPoints m_oTouchTester Reset\n");
			m_oTouchTester.Reset();
		}
	}
	else
	{
       if (m_bTouchHIDMode)
       {
          switch (m_eHIDDeviceMode)
          {
            case E_DEV_MODE_MOUSE:
                 //�������Ϊ0�ı���Ϣ
				if(m_bAirOperationMode)
				{
					for (int i = 0; i < nPenCount; i++)
					{
						if (pPenInfos[i].uId == 0)
						{
							m_oVirtualMouse.Input_AirOperate(pPenInfos[i].ePenState == E_PEN_STATE_DOWN, &pPenInfos[i].pt, m_eClickMode);
							break;
						}
					}
				}
				else
				{
                   for (int i = 0; i < nPenCount; i++)
                   {
                      if (pPenInfos[i].uId == 0 )
                      {
                          m_oVirtualMouse.Input(pPenInfos[i].ePenState == E_PEN_STATE_DOWN, &pPenInfos[i].pt, TRUE);
                          break;
                      }
                   }
				}

                break;
            case E_DEV_MODE_TOUCHSCREEN:
                 ////���ѡ����ǵ��㴥�صĻ���ֻ��Ӧһ����Ϳ�����
                 if (m_bSinglePointMode)
                 {
                     //�������Ϊ0�ı���Ϣ
                     for (int i = 0; i < nPenCount; i++)
                     {
                         if (pPenInfos[i].uId == 0)
                         {
                             InputTouchPoints(&pPenInfos[i], 1, dwCameraId);
                             break;
                         }
					 }
                  }
                  else
                  {
                      bRet = InputTouchPoints(pPenInfos, nPenCount, dwCameraId);
                  }
                  break;
            } //switch  
        }

        if (m_bTouchTUIOMode)
        {
            //ģ�������TUIO
             m_oVirtualTUIOTouch.InputTUIOPoints(pPenInfos, nPenCount, pPenInfos1, nPenCount1, dwCameraId);
        }
	}
    return bRet;
}


//@����:ģ�ⴥ������
//@����:pPenInfos, ָ���������ָ��
//      nPenCount, ���֧��
BOOL CVirtualHID::InputTouchPoints(const TContactInfo* pPenInfos, int nPenCount, int dwCameraId)
{
    //static int temp_debug = 0;
    //if (temp_debug < 10)
    //{
    //    LOG_INF("InputTouchPoints\n");
    //}

    if (nPenCount > _countof(m_TouchPoints))
    {
        nPenCount = _countof(m_TouchPoints);
    }

    if (nPenCount == 0) return FALSE;

    if (m_hDev == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    TContactInfo aryContactInfos[MAX_TOUCH_POINT_COUNT];

    if (nPenCount > _countof(aryContactInfos))
    {
        nPenCount = _countof(aryContactInfos);
    }
    memcpy(&aryContactInfos[0], pPenInfos, nPenCount * sizeof(TContactInfo));

    //λ����չ���еĴ��ص�
    TContactInfo aryExtendScreenContactInfos[MAX_TOUCH_POINT_COUNT];
    size_t PtCountInExtendScreen = 0;

    //˫�����
    //<<2018/08/03
    //ƽ���˲��㹻������˫�����.
    //˫�����ĺ���ǣ��ڻ����н���������ʣ������ʻ�����������
    this->m_oDblClickChecker.DoChecker(aryContactInfos, nPenCount);
    //>>

    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
    int nScreenLeft = 0;
    int nScreenTop = 0;

    int nPrimaryContactCount = 0;
    for (int i = 0; i < nPenCount; i++)
    {
        const POINT& ptContact = aryContactInfos[i].pt;

        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(ptContact.x, ptContact.y);
        //POINTER_DEVICE_INFO* pPointerDeviceInfo = GetPointerDevice();
        if (NULL == pDisplayDevInfo) return FALSE;

        if (!pDisplayDevInfo->bIsPrimary)
        {//��չ��
            aryExtendScreenContactInfos[PtCountInExtendScreen] = aryContactInfos[i];
            PtCountInExtendScreen++;
            continue;
        }

        EASI_TouchPoint& touchPoint = m_TouchPoints[nPrimaryContactCount];
        nPrimaryContactCount++;

        touchPoint.ContactId = aryContactInfos[i].uId;
        touchPoint.bStatus   = aryContactInfos[i].ePenState == E_PEN_STATE_DOWN ? TIP_DOWN : TIP_UP;
        touchPoint.iCameraId = dwCameraId;

 


        //if (NULL == pPointerDeviceInfo) return FALSE;

        LONG nMonitorPixelLeft = pDisplayDevInfo->rcMonitor.left;
        LONG nMonitorPixelTop = pDisplayDevInfo->rcMonitor.top;
        LONG nMonitorPixelWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
        LONG nMonitorPixelHeight = pDisplayDevInfo->rcMonitor.bottom - pDisplayDevInfo->rcMonitor.top;


        //ȷ�������Ŀ�߱�
        int aspectRatioNominator = 16;
        int aspectRatioDenominator = 9;
        ETouchScreenAspectRatio eRatio;

        eRatio = g_tSysCfgData.globalSettings.eTouchScreenAspectRatio;

        switch (eRatio)
        {
        case E_TOUCH_SCREEN_ASPECT_RATIO_AUTO:
            aspectRatioNominator = m_aspectRatioNominator;
            aspectRatioDenominator = m_aspectRatioDenominator;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_16_9:
            aspectRatioNominator = 16;
            aspectRatioDenominator = 9;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_16_10:
            aspectRatioNominator = 16;
            aspectRatioDenominator = 10;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_4_3:
            aspectRatioNominator = 4;
            aspectRatioDenominator = 3;
            break;
        }//switch



        POINT contactPos = aryContactInfos[i].pt;

        long x = contactPos.x;
        long y = contactPos.y;
        long temp = 0;


        //�����Ļ������ת�򽫴��ص���Ļ����ת��Ϊδ��תʱ����Ļ����
        //switch (pPointerDeviceInfo->displayOrientation)
        switch (pDisplayDevInfo->targetInfo.rotation)
        {
        case DISPLAYCONFIG_ROTATION_IDENTITY:
            //Keep No Change
            break;

        case DISPLAYCONFIG_ROTATION_ROTATE90:

            temp = nMonitorPixelWidth;
            nMonitorPixelWidth = nMonitorPixelHeight;
            nMonitorPixelHeight = temp;

            contactPos.x = y;
            contactPos.y = nMonitorPixelHeight - x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

            break;

        case DISPLAYCONFIG_ROTATION_ROTATE180:
            contactPos.x = nMonitorPixelWidth - x;
            contactPos.y = nMonitorPixelHeight - y;
            break;


        case DISPLAYCONFIG_ROTATION_ROTATE270:
            temp = nMonitorPixelWidth;
            nMonitorPixelWidth = nMonitorPixelHeight;
            nMonitorPixelHeight = temp;

            contactPos.x = nMonitorPixelWidth - y;
            contactPos.y = x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //>>

            break;

        }//switch


        BOOL bDone = FALSE;


        if (DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX == pDisplayDevInfo->targetInfo.scaling
            ||
            DISPLAYCONFIG_SCALING_IDENTITY == pDisplayDevInfo->targetInfo.scaling
            ||
            DISPLAYCONFIG_SCALING_CENTERED == pDisplayDevInfo->targetInfo.scaling)
        {
            WORD wXData = 0, wYData = 0;
            OSVERSIONINFOEX osvinfex;
            if (IsWin10OrGreater())
            {
                /*Version OS build
                1909    18363.535
                1903    18362.535
                1809    17763.914
                1803    17134.1184
                1709    16299.1565
                */

                m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;

                if (RtlGetVersionWrapper(&osvinfex))
                {
                    if (osvinfex.dwBuildNumber <= 16299)
                    {//�汾1709(OS�ڲ��汾 16299.125)
                        //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                    }
                    else if (osvinfex.dwBuildNumber == 17134)
                    {//�汾1803(OS�ڲ��汾 17134.1184)
                        if (pDisplayDevInfo->displayAdapterInfos.size() >= 2)
                        {//��Ļ����ģʽ
                            //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                        }
                        else
                        {//��ĻǶ���ڴ����ڲ���ģ��
                            m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL;
                        }
                    }
                    //else if (osvinfex.dwBuildNumber == 17763)
                    //{   //�汾1809(OS�ڲ��汾 17763.253)
                        //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                    //}

                }
            }
            else
            {
                m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
            }


            switch (m_eTouchDataAdjustModel)
            {
            case E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO:
            {//������ָ����߱�����ģ��
             //Windows�ڲ�������Ļ�������߱ȶԴ���λ����������
             //��Ļ�������ؿ�Ⱥ����ظ߶Ⱦ��ǰ���ָ���Ŀ�߱���������������Ⱥ͸߶ȣ�
                int nMonitorVirtualPixelWidth = nMonitorPixelWidth;
                int nMonitorVirtualPixelHeight = nMonitorPixelHeight;

                if (aspectRatioNominator * nMonitorPixelHeight > aspectRatioDenominator * nMonitorPixelWidth)
                { //�����߱ȴ���ʵ�����صĿ�߱�,��ֱ������Ŀ���ֲ���, ˮƽ������������
                    nMonitorVirtualPixelWidth = nMonitorVirtualPixelHeight * aspectRatioNominator / aspectRatioDenominator;
                }
                else if (aspectRatioNominator * nMonitorPixelHeight < aspectRatioDenominator * nMonitorPixelWidth)
                {//�����߱�С��ʵ�����صĿ�߱�, ˮƽ������Ŀ���ֲ��䣬��ֱ������������
                    nMonitorVirtualPixelHeight = nMonitorVirtualPixelWidth * aspectRatioDenominator / aspectRatioNominator;
                }

                wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nMonitorVirtualPixelWidth - nMonitorPixelWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nMonitorVirtualPixelWidth);
                wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nMonitorVirtualPixelHeight - nMonitorPixelHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nMonitorVirtualPixelHeight);

                LOG_INF("aspectRatioNominator=%d,aspectRatioDenominator=%d, nMonitorVirtualPixelWidth=%d,nMonitorVirtualPixelHeight=%d\r\n",
                    aspectRatioNominator,
                    aspectRatioDenominator,
                    nMonitorVirtualPixelWidth,
                    nMonitorVirtualPixelHeight);



            }
            break;

            case E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL:
            {//��ʾ������Ƕ���ڴ�������ģ��
                const int& nSourceWidth = pDisplayDevInfo->sourceMode.width;
                const int& nSourceHeight = pDisplayDevInfo->sourceMode.height;
                const int& nTargetWidth = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cx;
                const int& nTargetHeight = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cy;

                wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nTargetWidth - nSourceWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nTargetWidth);
                wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nTargetHeight - nSourceHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nTargetHeight);

                //if (temp_debug < 10)
                //{
                //    LOG_INF("nSourceWidth=%d,nSourceHeight=%d, nTargetWidth=%d,nTargetHeight=%d\r\n",
                //        nSourceWidth, nSourceHeight, nTargetWidth, nTargetHeight);

                //}

            }
            break;
            }//switch(m_eTouchDataAdjustModel)

            touchPoint.wXData = wXData;
            touchPoint.wYData = wYData;
            bDone = TRUE;
        }

        if (!bDone)
        {
            touchPoint.wXData = USHORT((contactPos.x - nMonitorPixelLeft) * EASI_TOUCH_MAXIMUM_X / nCxScreen);
            touchPoint.wYData = USHORT((contactPos.y - nMonitorPixelTop) * EASI_TOUCH_MAXIMUM_Y / nCyScreen);
        }


        
    }//for

    BOOL bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPrimaryContactCount);

    if (PtCountInExtendScreen > 0)
    {//��չ��ע��ģ��
        m_oTouchInjector.Input(aryExtendScreenContactInfos, PtCountInExtendScreen);
    }

    return bRet;
}

BOOL CVirtualHID::InputTouchPoints_AirOperate(const TContactInfo* pPenInfos, int nPenCount, EAIROPERATE_CLICKMODE  eClickMode)
{

	if (nPenCount > _countof(m_TouchPoints))
	{
		nPenCount = _countof(m_TouchPoints);
	}

	if (nPenCount == 0) return FALSE;

	if (m_hDev == INVALID_HANDLE_VALUE)
	{
		return FALSE;

	}

	TContactInfo aryContactInfos[MAX_TOUCH_POINT_COUNT];

	if (nPenCount > _countof(aryContactInfos))
	{
		nPenCount = _countof(aryContactInfos);
	}
	memcpy(&aryContactInfos[0], pPenInfos, nPenCount * sizeof(TContactInfo));
	
	int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
	int nScreenLeft = 0;
	int nScreenTop = 0;

	for (int i = 0; i < nPenCount; i++)
	{
		m_TouchPoints[i].ContactId = aryContactInfos[i].uId;
		m_TouchPoints[i].bStatus = aryContactInfos[i].ePenState == E_PEN_STATE_DOWN ? TIP_UP : TIP_DOWN;

		const POINT& ptContact = aryContactInfos[i].pt;

		const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(ptContact.x, ptContact.y);
		//POINTER_DEVICE_INFO* pPointerDeviceInfo = GetPointerDevice();
		if (NULL == pDisplayDevInfo) return FALSE;
		//if (NULL == pPointerDeviceInfo) return FALSE;

		LONG nMonitorPixelLeft = pDisplayDevInfo->rcMonitor.left;
		LONG nMonitorPixelTop = pDisplayDevInfo->rcMonitor.top;
		LONG nMonitorPixelWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
		LONG nMonitorPixelHeight = pDisplayDevInfo->rcMonitor.bottom - pDisplayDevInfo->rcMonitor.top;


		//ȷ�������Ŀ�߱�
		int aspectRatioNominator = 16;
		int aspectRatioDenominator = 9;
		ETouchScreenAspectRatio eRatio;

		eRatio = g_tSysCfgData.globalSettings.eTouchScreenAspectRatio;

		switch (eRatio)
		{
		case E_TOUCH_SCREEN_ASPECT_RATIO_AUTO:
			aspectRatioNominator = m_aspectRatioNominator;
			aspectRatioDenominator = m_aspectRatioDenominator;
			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_16_9:
			aspectRatioNominator = 16;
			aspectRatioDenominator = 9;

			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_16_10:
			aspectRatioNominator = 16;
			aspectRatioDenominator = 10;

			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_4_3:
			aspectRatioNominator = 4;
			aspectRatioDenominator = 3;
			break;
		}//switch



		POINT contactPos = aryContactInfos[i].pt;

		long x = contactPos.x;
		long y = contactPos.y;
		long temp = 0;


		//�����Ļ������ת�򽫴��ص���Ļ����ת��Ϊδ��תʱ����Ļ����
		//switch (pPointerDeviceInfo->displayOrientation)
		switch (pDisplayDevInfo->targetInfo.rotation)
		{
		case DISPLAYCONFIG_ROTATION_IDENTITY:
			//Keep No Change
			break;

		case DISPLAYCONFIG_ROTATION_ROTATE90:

			temp = nMonitorPixelWidth;
			nMonitorPixelWidth = nMonitorPixelHeight;
			nMonitorPixelHeight = temp;

			contactPos.x = y;
			contactPos.y = nMonitorPixelHeight - x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

			break;

		case DISPLAYCONFIG_ROTATION_ROTATE180:
			contactPos.x = nMonitorPixelWidth - x;
			contactPos.y = nMonitorPixelHeight - y;
			break;

		case DISPLAYCONFIG_ROTATION_ROTATE270:
			temp = nMonitorPixelWidth;
			nMonitorPixelWidth = nMonitorPixelHeight;
			nMonitorPixelHeight = temp;

			contactPos.x = nMonitorPixelWidth - y;
			contactPos.y = x;


            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

			break;

		}//switch


		BOOL bDone = FALSE;


		if (DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX == pDisplayDevInfo->targetInfo.scaling
			||
			DISPLAYCONFIG_SCALING_IDENTITY == pDisplayDevInfo->targetInfo.scaling
			||
			DISPLAYCONFIG_SCALING_CENTERED == pDisplayDevInfo->targetInfo.scaling)
		{
			WORD wXData = 0, wYData = 0;
			OSVERSIONINFOEX osvinfex;
			if (IsWin10OrGreater())
			{
				/*Version OS build
				1909    18363.535
				1903    18362.535
				1809    17763.914
				1803    17134.1184
				1709    16299.1565
				*/

				m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;

				if (RtlGetVersionWrapper(&osvinfex))
				{
					if (osvinfex.dwBuildNumber <= 16299)
					{//�汾1709(OS�ڲ��汾 16299.125)
					 //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
					}
					else if (osvinfex.dwBuildNumber == 17134)
					{//�汾1803(OS�ڲ��汾 17134.1184)
						if (pDisplayDevInfo->displayAdapterInfos.size() >= 2)
						{//��Ļ����ģʽ
						 //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
						}
						else
						{//��ĻǶ���ڴ����ڲ���ģ��
							m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL;
						}
					}
					//else if (osvinfex.dwBuildNumber == 17763)
					//{   //�汾1809(OS�ڲ��汾 17763.253)
					//m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
					//}

				}
			}
			else
			{
				m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
			}

			switch (m_eTouchDataAdjustModel)
			{
			case E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO:
			{//������ָ����߱�����ģ��
			 //Windows�ڲ�������Ļ�������߱ȶԴ���λ����������
			 //��Ļ�������ؿ�Ⱥ����ظ߶Ⱦ��ǰ���ָ���Ŀ�߱���������������Ⱥ͸߶ȣ�
				int nMonitorVirtualPixelWidth = nMonitorPixelWidth;
				int nMonitorVirtualPixelHeight = nMonitorPixelHeight;

				if (aspectRatioNominator * nMonitorPixelHeight > aspectRatioDenominator * nMonitorPixelWidth)
				{ //�����߱ȴ���ʵ�����صĿ�߱�,��ֱ������Ŀ���ֲ���, ˮƽ������������
					nMonitorVirtualPixelWidth = nMonitorVirtualPixelHeight * aspectRatioNominator / aspectRatioDenominator;
				}
				else if (aspectRatioNominator * nMonitorPixelHeight < aspectRatioDenominator * nMonitorPixelWidth)
				{//�����߱�С��ʵ�����صĿ�߱�, ˮƽ������Ŀ���ֲ��䣬��ֱ������������
					nMonitorVirtualPixelHeight = nMonitorVirtualPixelWidth * aspectRatioDenominator / aspectRatioNominator;
				}

				wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nMonitorVirtualPixelWidth - nMonitorPixelWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nMonitorVirtualPixelWidth);
				wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nMonitorVirtualPixelHeight - nMonitorPixelHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nMonitorVirtualPixelHeight);
			}
			break;

			case E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL:
			{//��ʾ������Ƕ���ڴ�������ģ��
				const int& nSourceWidth = pDisplayDevInfo->sourceMode.width;
				const int& nSourceHeight = pDisplayDevInfo->sourceMode.height;
				const int& nTargetWidth = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cx;
				const int& nTargetHeight = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cy;

				wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nTargetWidth - nSourceWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nTargetWidth);
				wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nTargetHeight - nSourceHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nTargetHeight);
			}
			break;
			}//switch(m_eTouchDataAdjustModel)

			m_TouchPoints[i].wXData = wXData;
			m_TouchPoints[i].wYData = wYData;
			bDone = TRUE;
		}

		if (!bDone)
		{
			m_TouchPoints[i].wXData = USHORT((contactPos.x - nMonitorPixelLeft) * EASI_TOUCH_MAXIMUM_X / nCxScreen);
			m_TouchPoints[i].wYData = USHORT((contactPos.y - nMonitorPixelTop) * EASI_TOUCH_MAXIMUM_Y / nCyScreen);
		}
	}//for

	BOOL bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPenCount);
	for(int j = 0; j < nPenCount; j++)
	{
		if(m_TouchPoints[j].bStatus == TIP_DOWN)
		{
			m_TouchPoints[j].bStatus = TIP_UP;
		}
	}
    bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPenCount);

	return bRet;
}

//@����:�ж����������Ƿ��Ǵ򿪵ġ�
BOOL CVirtualHID::IsVirtualDriverOpen()
{
    return this->m_hDev != INVALID_HANDLE_VALUE ? TRUE : FALSE;
}


void  CVirtualHID::DeviceHandleEventCallBack(const DeviceHandleMonitorEntry* pEntry, WPARAM dbtEvent)
{
    if (!pEntry) return;

    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(pEntry->lpCtx);

    const TCHAR* lpszEvent = _T("Unkown Event");
    switch (dbtEvent)
    {
    case DBT_DEVICEQUERYREMOVE:

        if (lpThis->m_hDev != INVALID_HANDLE_VALUE)
        {
            lpThis->CloseDevice();

            //�������ر���������ģʽ
            lpThis->m_eHIDDeviceMode = E_DEV_MODE_MOUSE;


            //��ʼ�����豸�ӿ�״̬��
            const GUID& interface_guid = EASI_GetDeviceInterfaceGUID();
            lpThis->m_oDeviceEventDetector.AddDevIntefaceMonitor(interface_guid, CVirtualHID::DeviceInterfaceEventCallBack, (LPVOID)lpThis);
        }

        lpszEvent = _T("DBT_DEVICEQUERYREMOVE");
        break;

    case DBT_DEVICEQUERYREMOVEFAILED:
        lpszEvent = _T("DBT_DEVICEQUERYREMOVEFAILED");
        break;

    case DBT_DEVICEREMOVEPENDING:
        lpszEvent = _T("DBT_DEVICEREMOVEPENDING");
        break;

    case DBT_DEVICEREMOVECOMPLETE:
        lpszEvent = _T("DBT_DEVICEREMOVECOMPLETE");
        break;

    case DBT_DEVICEARRIVAL:
        lpszEvent = _T("DBT_DEVICEARRIVAL");
        break;
    };

    AtlTrace(_T("Device Event 0x%x:%s\n"), dbtEvent, lpszEvent);
    LOG_INF("Device Event 0x%x:%s\n", dbtEvent, lpszEvent);
}


//@����:HID�豸�ӿڹ����¼��ص�����
void  CVirtualHID::DeviceInterfaceEventCallBack(const DeviceIntetfaceMonitorEntry* pEntry, WPARAM dbtEvent)
{
    if (!pEntry) return;
    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(pEntry->lpCtx);
    const TCHAR* lpszEvent = _T("Unkown Event");


    switch (dbtEvent)
    {
    case DBT_DEVICEARRIVAL:
    {
        int nTryTimes = 0;
        //���Է����������豸��ʧ�� ��˳��Զ��
        while (lpThis->m_hDev == INVALID_HANDLE_VALUE && nTryTimes < 10)
        {
            lpThis->OpenDevice();

            if (lpThis->m_hDev == INVALID_HANDLE_VALUE)
            {
                Sleep(10);//10ms
            }
            nTryTimes++;
        }
    }
    lpThis->RetrievePointerDevices();
    lpszEvent = _T("DBT_DEVICEARRIVAL");
    break;

    case DBT_DEVICEREMOVECOMPLETE:
        lpThis->RetrievePointerDevices();

        lpszEvent = _T("DBT_DEVICEREMOVECOMPLETE");
        break;

    case DBT_DEVICEREMOVEPENDING:
        lpszEvent = _T("DBT_DEVICEREMOVEPENDING");
        break;

    };

    AtlTrace(_T("Device Event %d:%s\n"), dbtEvent, lpszEvent);

}


//@����:���ô����ĳߴ��¼���Ӧ����
//@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
//      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
void CVirtualHID::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    //�����˫�������������Ļ����ߴ����Ļ�ֱ���
    this->m_oVirtualMouse.GetDoubleClickChecker().OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);

    //������˫�������������Ļ����ߴ����Ļ�ֱ���
    this->m_oDblClickChecker.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);

}


#include "..\inc\Win32UDllLibrary.h"


BOOL CVirtualHID::RetrievePointerDevices()
{
    static CWin32UDllLibrary m_oWin32UDllLibrary;
    //Windows�汾С��Win8.0
    if (!IsWindows8OrGreater()) return FALSE;


    m_vecPointerDeviceInfos.clear();

    UINT32 dwCount = 0;
    BOOL bRet = m_oWin32UDllLibrary.GetPointerDevices(&dwCount, NULL);
    if (!bRet) return FALSE;

    if (dwCount == 0) return FALSE;

    m_vecPointerDeviceInfos.resize(dwCount);


    if (!m_oWin32UDllLibrary.GetPointerDevices(&dwCount, &m_vecPointerDeviceInfos[0]))
    {
        return FALSE;
    }

    return TRUE;
}


const TCHAR* g_szUMDFTocuhProductString = _T("UMDF Virtual hidmini device Product string");
//@����:���ݲ�Ʒ�ַ�����ȡ��Ӧ��ָ���豸��Ϣ��
//@����:lpszProductString, ��Ʒ�ַ���
//@����ֵ:NULL, δ�ҵ���
//        ������ָ��POINTER_DEVICE_INFO������ָ��
POINTER_DEVICE_INFO* CVirtualHID::GetPointerDevice(LPCTSTR lpszProductString)
{
    if (lpszProductString == NULL)
    {
        lpszProductString = g_szUMDFTocuhProductString;
    }
    for (size_t i = 0; i < m_vecPointerDeviceInfos.size(); i++)
    {
        if (_tcsicmp(m_vecPointerDeviceInfos[i].productString, lpszProductString) == 0)
        {
            return  &m_vecPointerDeviceInfos[i];
        }

    }//for

    return NULL;

}


BOOL CVirtualHID::OpenDeviceThreadSafe()
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForVirtualDevice);
    m_hDev = EASI_OpenDevice();
    m_oVirtualMouse.SetDeviceHandle(m_hDev);
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        //���豸���������������
        m_oDeviceEventDetector.AddHandleMonitor(m_hDev, DeviceHandleEventCallBack, (LPVOID)this);

        return TRUE;
    }

    return FALSE;
}

BOOL CVirtualHID::CloseDeviceThreadSafe()
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForVirtualDevice);
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        BOOL bRet = EASI_CloseDevice(m_hDev);

        //���豸����Ƴ��������
        m_oDeviceEventDetector.RemoveHandleMonitor(m_hDev);

        m_hDev = INVALID_HANDLE_VALUE;
        m_oVirtualMouse.SetDeviceHandle(INVALID_HANDLE_VALUE);

        return bRet;
    }
    return FALSE;
}


BOOL CVirtualHID::CreateAutoOpenThread()
{
    if (m_hAutoOpenThread == NULL)
    {
        DWORD dwThreadId = 0;
        m_bAutoOpenThreadExit = FALSE;
        m_hAutoOpenThread = CreateThread(
            NULL,
            0,
            AutoOpenThreadProc,
            (LPVOID)this,
            0,
            &dwThreadId);
    }

    return m_hAutoOpenThread == NULL ? FALSE : TRUE;

}
void CVirtualHID::CloseAutoOpenThread()
{
    m_bAutoOpenThreadExit = TRUE;
    WaitForSingleObject(m_hAutoOpenThread, 1000);
    m_hAutoOpenThread = NULL;

}

DWORD WINAPI CVirtualHID::AutoOpenThreadProc(LPVOID lpCtx)
{
    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(lpCtx);
    const int nMaxTryNumber = 60;
    const int nTryInterval = 1000;//ms,���Լ��
    int nTryCount = 0;
    while (!lpThis->m_bAutoOpenThreadExit
        &&
        lpThis->m_hDev == INVALID_HANDLE_VALUE
        &&
        nTryCount < nMaxTryNumber)
    {

        if (lpThis->OpenDeviceThreadSafe())
        {
            LOG_INF("Open device succeeded in AutoOpenThreadProc.\n");
            lpThis->SetHIDMode(lpThis->m_eDesiredHIDMode);
            break;
        }

        nTryCount++;
        Sleep(nTryInterval);
    }
    lpThis->m_hAutoOpenThread = NULL;

    CloseHandle(lpThis->m_hAutoOpenThread);
    return 0U;
}

//@����:���´�����������Ļ��Ϣ
void CVirtualHID::UpdateAttachedMonitorInfo()
{
    TCHAR pointerDevicePath[MAX_DEVICE_ID_LEN];

    BOOL bRet = GetPointerDeviceDevPath(
        g_szUMDFTocuhProductString,
        pointerDevicePath,
        _countof(pointerDevicePath));

    if (!bRet)
    {
        return;
    }

    int nDisplayDevCount = theApp.GetMonitorFinder().GetDisplayDevCount();

    UINT32 nMaxActiveCx = 0, nMaxActiveCy = 0;
    BOOL bMatched = FALSE;
    for (int i = 0; i < nDisplayDevCount && !bMatched; i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);

        if (pDisplayDevInfo == NULL) continue;
        int nDiplayAdpaterInfoCount = pDisplayDevInfo->displayAdapterInfos.size();

        for (int j = 0; j < nDiplayAdpaterInfoCount; j++)
        {
            const DiplayAdpaterInfo& inf = pDisplayDevInfo->displayAdapterInfos[j];
            //Monitor 1: 1920*1080
            //Minitor 2: 1920*1200
            //ϵͳѡ��1920:1080��Ϊ�����Ŀ�߱�

            //if (IsWin10OrGreater())
            {
                if (IsMonitorAttachedToPointerDevice(
                    pointerDevicePath,
                    inf.monitorDevicePath))
                {
                    m_aspectRatioNominator   = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
                    m_aspectRatioDenominator =  inf.targetMode.targetVideoSignalInfo.activeSize.cy;

                    bMatched = TRUE;
                    break;
                 }
            }

            if (nMaxActiveCx < inf.targetMode.targetVideoSignalInfo.activeSize.cx)
            {
                nMaxActiveCx = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
                nMaxActiveCy = inf.targetMode.targetVideoSignalInfo.activeSize.cy;
            }

        }//fo(j)

    }//for(i)

    //const DiplayAdpaterInfo& inf = theApp.GetMonitorFinder().GetDisplayDevInfo(0)->displayAdapterInfos[0];
    //m_aspectRatioNominator = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
    //m_aspectRatioDenominator = inf.targetMode.targetVideoSignalInfo.activeSize.cy;

    if (!bMatched)
    {
        m_aspectRatioNominator = nMaxActiveCx;
        m_aspectRatioDenominator = nMaxActiveCy;
    }
}

void CVirtualHID::SetTouchTUIOMode(bool  eMode)
{
    m_bTouchTUIOMode = eMode;
}
void CVirtualHID::SetTouchHIDMode(bool  eMode)
{
    /////m_bTouchHIDMode = true; 
    /////eMode = false ;
    if (m_bTouchHIDMode && !eMode)
    {
        ///ͣ�õ�HID����ģʽ��ҪReset();
        Reset();
    }
    m_bTouchHIDMode = eMode;
}
void CVirtualHID::SetSinglePointMode(bool eMode)
{
    m_bSinglePointMode = eMode;
}

void  CVirtualHID::SetTUIOParams(DWORD IP, int nPort, int nScreenWindth, int nScreenHeight)
{
    m_oVirtualTUIOTouch.SetTUIOParams(IP, nPort, nScreenWindth, nScreenHeight);
}

void CVirtualHID::OpenTUIOServer(bool bStart)
{
	m_oVirtualTUIOTouch.OpenTUIOServer(bStart);
}

void CVirtualHID::OpenTUIOServer1(bool bStart)
{
    m_oVirtualTUIOTouch.OpenTUIOServer(bStart);
}

DWORD CVirtualHID::GetIPadress()
{
    return m_oVirtualTUIOTouch.GetIPadress();
}

int CVirtualHID::GetPort()
{
    return m_oVirtualTUIOTouch.GetPort();
}
int CVirtualHID::GetScreenWidth()
{
	return m_oVirtualTUIOTouch.GetScreenWidth();
}

int CVirtualHID::GetScreenHeight()
{
	return m_oVirtualTUIOTouch.GetScreenHeight();
}

void CVirtualHID::SetTUIOScreenDisplayChange(int nScreenX ,int nScreenY)
{
	m_oVirtualTUIOTouch.SetTUIOScreenDisplayChange(nScreenX, nScreenY);
}

void CVirtualHID::SetTest30Point(BOOL bStart)
{
	m_bStartTest30Point = bStart;
}

BOOL CVirtualHID::GetTest30Point()
{
	return m_bStartTest30Point;
}

void CVirtualHID::SetAirOperateMode(BOOL eMode,EAIROPERATE_CLICKMODE  eClickMode)
{
	m_bAirOperationMode = eMode;
	m_eClickMode = eClickMode;
}