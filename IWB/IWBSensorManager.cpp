#include "stdafx.h"
//#include "headers.h"
CIWBSensorManager::CIWBSensorManager()
:
m_hVideoDispWnd(NULL),
m_nAutoCalibrateTryTimes(0),
m_nCurrentSensorID(-1),
m_uCurrentSelectSensordId(0)
{
    m_pSpotListProcessor = new CSpotListProcessor();

    //m_pSpotListProcessor1 = new CSpotListProcessor();

    if(theApp.GetScreenMode()>= EScreenModeDouble)
    {//˫��ģʽ
        //<<xuke
        Init(int(theApp.GetScreenMode()) + 1);
        //xuke>>
    }
    else
    {
        Init(1);
    }
 }

CIWBSensorManager::~CIWBSensorManager()
{
    Uninit();
    delete m_pSpotListProcessor;
    //delete m_pSpotListProcessor1;
}

//@����:��ʼ��
//@����: nSensorCount, CMOS��������Ŀ
//       hVideoDispWnd,��Ƶ��ʾ����
void CIWBSensorManager::Init(int nSensorCount)
{
    if(m_vecSensors.size() != nSensorCount)
    {
        m_vecSensors.resize(nSensorCount);
       for(int i=0; i<nSensorCount; i++)
       {
            m_vecSensors[i] = new CIWBSensor(i);
            //m_vecSensors[i]->GetPenPosDetector()->SetSpotListProcessor(&this->m_oSpotListProcessor);
            m_vecSensors[i]->GetPenPosDetector()->SetSpotListProcessor(this->m_pSpotListProcessor);
        }//for
    }


    m_vecVideoLayout.resize(nSensorCount);
    m_vecSplitter.resize(nSensorCount -1);

    //m_oSpotListProcessor.Init(nSensorCount);
    m_pSpotListProcessor->Init(nSensorCount);


    //int nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    //int nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
	int nLeft     = 0;
	int nTop      = 0;
	
    SplitMode splitMode(1, nSensorCount);

    /*
	if (g_tSysCfgData.globalSettings.bSupportExtendScreen)
	{
		nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		nLeft     = GetSystemMetrics(SM_XVIRTUALSCREEN);
		nTop      = GetSystemMetrics(SM_YVIRTUALSCREEN);
	}
    */
    

    m_oScreenLayoutDesigner.Init(splitMode, g_tSysCfgData.screenLayoutManger.GetScreenTargetType());

    //>>
    ApplyScreenLayout();

}


//@����:����ʼ��
void CIWBSensorManager::Uninit()
{
    for (size_t i = 0; i<m_vecSensors.size(); i++)
    {
        delete m_vecSensors[i];
        m_vecSensors[i] = NULL;
    }

    m_vecSensors.clear();
    m_oScreenLayoutDesigner.Uninit();
}


//@����:��IWBSensor�����������ͷ
//@����:Camera�豸�б�
void CIWBSensorManager::AssignCamera(const CUsbCameraDeviceList& cameraList)
{
    //����������
    int n = m_vecSensors.size();

    //�������ѱ���ֵ��־
    std::vector<bool> sensor_assigned;
    sensor_assigned.resize(n);
    for(int i=0; i < n; i++)
    {
        sensor_assigned[i] = false;
    }

    //����ͷʵ���ѱ������־
    int m = cameraList.GetDeviceInstanceCount();
    std::vector<bool> camear_instance_assigned;
    camear_instance_assigned.resize(m);
    for(int i=0; i < m; i++)
    {
        camear_instance_assigned[i] = false;
    }

    //���ȷ����豸·����ȫƥ�������ͷ
    for(int i=0; i<n; i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];
        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();
        for(int j=0; j < m; j++)
        {
            if(camear_instance_assigned[j]) continue;

            const TCaptureDeviceInstance*  pDevInst = cameraList.GetCaptureDeviceInstance(j);
            if(devInfo.m_strDevPath == pDevInst->m_strDevPath)
            {
                sensor.SetDeviceInfo(*pDevInst);
                camear_instance_assigned[j] = true;
                sensor_assigned[i] = true;
                break;
            }
        }
    }  

    //ʣ�µİ�˳�����
    for(int i=0; i < n; i++)
    {   
        if(sensor_assigned[i]) continue;//�ѷ���
        CIWBSensor& sensor = *m_vecSensors[i];
        for(int j=0; j<m; j++)
        {
            if(camear_instance_assigned[j]) continue;
            const TCaptureDeviceInstance*  pDevInst = cameraList.GetCaptureDeviceInstance(j);

            sensor.SetDeviceInfo(*pDevInst);
            camear_instance_assigned[j] = true;
            sensor_assigned[i] = true;
            break;
        }
    }

    //���"δ���䵽�豸��sensor"���豸·���Ƿ���"�ѷ��䵽�豸��sensor�����豸·����ͬ��
    //����ͬ��"δ���䵽�豸��sensor"���豸·����ա�
    //Ϊʲô��ҪҪ��������,
    //�������ֳ���, ����ģʽ��ֻ����һ������ͷ��
    //�������ֲ���
    //step1: �����������ڶ�����Ļ���ţ��������ԶԻ�����ȷ��,Ŀ���Ǹ���g_tSysCfgData, ����g_tSysCfgData�м�¼��
    //       �ڶ���sensor���豸·��Ϊ�����Ӳ��·����
    //step2: ������ģʽ�л�������ģʽ���������ԶԻ�����ȷ��,Ŀ���Ǹ���g_tSysCfgData,g_tSysCfgData�м�¼��
    //       ��һ��sensor���豸·��Ϊ�����Ӳ��·��������g_tSysCfgData�е�����sensorӵ����ͬ�豸·����
    //step3: ������ģʽ�л���˫��ģʽ��������������:
    //       ����sensor���豸·����ȫ��ͬ���������޷��������Ŵ���
    //       ����ʱһ�������������ţ�����һ��������Ƶ����ʧ��
    //
    //g_tSysCfgData.vecSensorConfig��һ��Ԫ�ظ���ΪSENSOR_NUMBER�����顣
    for (int i = 0; i < n; i++)
    {
        if (sensor_assigned[i]) continue;//����ֻ����"δ���䵽�豸��sensor",�Ѿ��������豸��������
        
        CIWBSensor& sensorUnassigned = *m_vecSensors[i];
        const TCaptureDeviceInstance& devInfoUnassigned = sensorUnassigned.GetDeviceInfo();

        for (int j = 0; j < n; j++)
        {
            if (!sensor_assigned[j]) continue;//����"δ���䵽�豸��sensor", ����ֻ��"�Ѿ����䵽�豸��sensor"�Ƚϡ�
            if (i == j) continue;//�Լ������Լ��Ƚ�

            const CIWBSensor& sensorAssigned = *m_vecSensors[j];
            const TCaptureDeviceInstance& devInfoAssigned = sensorAssigned.GetDeviceInfo();
            
            if (0 == _tcsicmp(devInfoUnassigned.m_strDevPath, devInfoAssigned.m_strDevPath))
            {
                //������"δ���䵽�豸��sensor"��"�Ѿ����䵽�豸��sensor"���豸·����ͬ
                //�����"δ���䵽�豸��sensor"���豸·����
                TCaptureDeviceInstance devPathClearedInstance = devInfoUnassigned;
                devPathClearedInstance.m_strDevPath = _T("");
                sensorUnassigned.SetDeviceInfo(devPathClearedInstance);
            }

        }
    }


}

//@����:������Ƶ��ʾ����
//@����:hDispWnd,  ��Ƶ��ʾ������
//      hNotifyWnd, ������Ϣ���մ�����
//      rcVideoDispArea, ��Ƶ��ʾ����
void CIWBSensorManager::SetVideoDispParams(HWND hDispWnd, HWND hNotifyWnd, RECT& rcVideoDispArea)
{
    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return;
    SetVideoDisplayArea(rcVideoDispArea);

    for(int i=0; i<nSensorCount; i++)
    {
        CIWBSensor* pSensor = m_vecSensors[i];
        pSensor->SetVideoDispParams(hDispWnd, m_vecVideoLayout[i], hNotifyWnd);

    }

    m_hVideoDispWnd = hDispWnd;
}


//@����:��ʼ����
//@����:
//		nSensorID, ���������(0~n), -1ʱָȫ�崫����
BOOL CIWBSensorManager::StartRunning(int nSensorID)
{
    LOG_FUNC_CALL;

    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return FALSE;

    if(IsAllSensorStopped())
    {//�������д�����ֹͣ����״̬,��Ҫ������ߴ�����

        //��ߴ�������ʼ����
        //<<added by toxuke@gmail.com,2015/02/15
        //this->m_oSpotListProcessor.StartProcess();
        this->m_pSpotListProcessor->StartProcess();
        //>>


        //����ϵͳ������ʶ��
        g_oWinGR.Start();
        /*
        ����windows������ʶ��͹���ִ��������
        */
        g_oWinGR.AttachExceContext((IWGRUtilContext *)&m_execContext);
    }

    if(nSensorID != -1 && nSensorID >= 0 && nSensorID < nSensorCount)
    {
        if(nSensorID < 0 || nSensorID >= nSensorCount) return FALSE;
        CIWBSensor* pSensor = m_vecSensors[nSensorID];
        if(!pSensor->IsDetecting())pSensor->Run();
    }
    else
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];

            if(!pSensor->IsDetecting())pSensor->Run();
        }
    }
    return TRUE;
}


//@����:ֹͣ����
//@����:nSensorID, ���������(0~n), -1ʱָȫ�崫���� 
BOOL CIWBSensorManager::StopRunning(int nSensorID)
{
	GetCfgData(g_tSysCfgData);

    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return FALSE;

    if(nSensorID != -1 && nSensorID >= 0 && nSensorID < nSensorCount)
    {
        if(nSensorID < 0 || nSensorID >= nSensorCount) return FALSE;
        CIWBSensor* pSensor = m_vecSensors[nSensorID];
        pSensor->Stop();
    }
    else
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];
            pSensor->Stop();
        }
    }
    

    if(IsAllSensorStopped())
    {//���д������Ѿ��ر���, �ر����д��������õĶ���
        //�ر�windows������ʶ��
        g_oWinGR.Stop();

        //��ߴ�����ֹͣ����
        //<<added by toxuke@gmail.com,2015/02/15
        //this->m_oSpotListProcessor.StopProcess();
        this->m_pSpotListProcessor->StopProcess();
        //>>
    }


    //���½�����ʾ
    InvalidateRect(m_hVideoDispWnd, NULL, TRUE);
    UpdateWindow(m_hVideoDispWnd);
    return TRUE;
}

//@����:�ı���Ƶ��ʾ����ĳߴ�
void CIWBSensorManager::SetVideoDisplayArea(const RECT& rcNewDisplayArea)
{
    int nSensorCount = (int)m_vecSensors.size();

    if(nSensorCount == 0) return;

    UpdateVideoLayout(rcNewDisplayArea);

    for(int i=0; i<nSensorCount; i++)
    {
        CIWBSensor* pSensor = m_vecSensors[i];
        const RECT& rcNewArea = this->m_vecVideoLayout[i];
        pSensor->GetVideoPlayer()->SetDisplayArea(rcNewArea);
    }

}


//@����:�л�����ͷ��ָ����ģʽ
void CIWBSensorManager::SwitchToMode(ESensorLensMode eMode, int nSensorID)
{
    int nSensorCount = (int)m_vecSensors.size();

    if(nSensorID == -1)
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];
            pSensor->SwitchLensMode(eMode);
        }
    }
    else  if(0 <= nSensorID && nSensorID < nSensorCount)
    {
        m_vecSensors[nSensorID]->SwitchLensMode(eMode);
    }

}

//@����:��ȡ����ģʽ
//@˵��:ȫ�ֵĹ���ģʽ���һ���������һ��
ESensorLensMode CIWBSensorManager::GetLensMode()const
{
    ESensorLensMode eMode = E_NORMAL_USAGE_MODE;

     int nSensorCount = (int)m_vecSensors.size();
     
     if(nSensorCount > 0)
     {
         eMode = m_vecSensors[0]->GetLensMode();
     }

     return eMode;
}


//@����:�ж��Ƿ��д�������ʹ��ģʽ�µ����
BOOL CIWBSensorManager::HasNormalUsageCamera()const
{
    int nSensorCount = (int)m_vecSensors.size();
    BOOL bYes = FALSE;

    for (int i = 0; i < nSensorCount; i++)
    {
        if (m_vecSensors[i]->GetLensMode() == E_NORMAL_USAGE_MODE)
        {
            bYes = TRUE;
            break;
        }
    }

    return bYes;
}


void CIWBSensorManager::DrawSelectBound(HWND hWnd)
{

	HDC hDC = GetDC(hWnd);
	CBrush brushBlack;
	 brushBlack.CreateSolidBrush(RGB(0, 0, 0));

	CBrush* pBrushHalftone = CDC::GetHalftoneBrush();

	HBRUSH hBrushOld = NULL;

	UINT nSensorCount = m_vecSensors.size();
	for (UINT uSensorIdx = 0; uSensorIdx< nSensorCount; uSensorIdx++)
	{
		const RECT& rcArea = m_vecVideoLayout[uSensorIdx];
		RECT  rcInternal  = rcArea;
		RECT  rcExternal  = rcArea;
		InflateRect(&rcExternal, SELECT_BOUND_WIDTH, SELECT_BOUND_WIDTH);


		HRGN rgn = ::CreateRectRgnIndirect(&rcExternal);
		HRGN rgnInternal = ::CreateRectRgnIndirect(&rcInternal);
		::CombineRgn(rgn, rgnInternal, rgn, RGN_XOR);


		SelectClipRgn(hDC, rgn);

		RECT rcClipBox;
		GetClipBox(hDC, &rcClipBox);


		if (uSensorIdx == m_uCurrentSelectSensordId)
		{
			hBrushOld = (HBRUSH)::SelectObject(hDC, pBrushHalftone->GetSafeHandle());
			//https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createpatternbrush
			//A brush created by using a monochrome (1 bit per pixel) bitmap has the text and background colors of the device context to which it is drawn. 
			//Pixels represented by a 0 bit are drawn with the current text color; pixels represented by a 1 bit are drawn with the current background color.
			SetTextColor(hDC, RGB(0, 0, 0));
			SetBkColor(hDC, RGB(238,149,114)); //RGB(255, 255, 255)
		}
		else
		{
			hBrushOld = (HBRUSH)::SelectObject(hDC, brushBlack.GetSafeHandle());
		}

		PatBlt(
			hDC,
			rcClipBox.left,
			rcClipBox.top,
			rcClipBox.right - rcClipBox.left,
			rcClipBox.bottom - rcClipBox.top,
			PATCOPY);

		 SelectClipRgn(hDC, NULL);
		::SelectObject(hDC, hBrushOld);

		::DeleteRgn(rgn);
		::DeleteRgn(rgnInternal);


	}

	ReleaseDC(hWnd, hDC);
}

//@����:���Ʋ��ֿ��
//@����:hWnd, ������
void CIWBSensorManager::DrawLayoutFrame(HWND hWnd)
{   
    if(hWnd == NULL)
    {
        hWnd = m_hVideoDispWnd;
    }

    if(NULL == hWnd) return;

    int nSensorCount = m_vecSensors.size();

    HDC hDC = GetDC(hWnd);

    COLORREF clrBkgnd   = RGB(16,0,16);//key color for overlay surface
    COLORREF clrFace    = GetSysColor(COLOR_3DFACE);
    COLORREF clrShadow  = GetSysColor(COLOR_3DDKSHADOW);
    COLORREF clrHilight = GetSysColor(COLOR_3DHILIGHT);
    for(int i=0; i<nSensorCount; i++)
    {
        RECT& rcArea = m_vecVideoLayout[i];
        CIWBSensor* pSensor = m_vecSensors[i];

        //���ڸ��²�ˢ�������������˸
        if(!pSensor->GetVideoPlayer()->IsDetecting())
        {         
            //����CDC:FillSolidRect
            ::SetBkColor(hDC, clrBkgnd);
            ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcArea, NULL, 0, NULL);
        }

        //����Splitter
        if(i != nSensorCount - 1)
        {
            RECT& rcSplitter = m_vecSplitter[i];
            //Draw3dRect
            ::SetBkColor(hDC, clrFace);
            ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcSplitter, NULL, 0, NULL);

            ::SetBkColor(hDC, clrHilight);

            RECT rc;
            //top highlight line
            rc.left   = rcSplitter.left;
            rc.top    = rcSplitter.top ;
            rc.right  = rcSplitter.right - 1;
            rc.bottom = rcSplitter.top  + 1;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            //right vertical highlight line
            rc.left   = rcSplitter.left;
            rc.top    = rcSplitter.top ;
            rc.right  = rcSplitter.left   + 1;
            rc.bottom = rcSplitter.bottom - 1;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            ::SetBkColor(hDC, clrShadow);
            rc.left   = rcSplitter.right - 1;
            rc.top    = rcSplitter.top      ;
            rc.right  = rcSplitter.right    ;
            rc.bottom = rcSplitter.bottom   ;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            rc.left   = rcSplitter.left   ;
            rc.top    = rcSplitter.bottom-1;
            rc.right  = rcSplitter.right   ;
            rc.bottom = rcSplitter.bottom  ;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
        }

    }

    ReleaseDC(hWnd, hDC);

	DrawSelectBound(hWnd);
}


//@����:������Ƶ����
void CIWBSensorManager::UpdateVideoLayout(const RECT& rcDisplayArea)
{
    int nSensorCount = m_vecSensors.size();

    if(nSensorCount == 0) return;

    int nDispWndWidth  = rcDisplayArea.right   - rcDisplayArea.left;
    int nDispWndHeight = rcDisplayArea.bottom  - rcDisplayArea.top ;

    int nEachVideoHeight   = nDispWndHeight;
    int nVideoDisplayWidth = nDispWndWidth - (nSensorCount - 1)*SPLITTER_WIDTH;

    int nEachVideoWidth          = nVideoDisplayWidth/nSensorCount;
    int nVideoWidthResidualInc   = nVideoDisplayWidth%nSensorCount;  
    int nVideoWidthResidualCount = 0;

    int left = rcDisplayArea.left;
    int top  = rcDisplayArea.top ;

    for(int i=0; i<nSensorCount; i++)
    {
        RECT& rcArea = m_vecVideoLayout[i];

        rcArea.left   = left;
        rcArea.top    = top;
        rcArea.right  = left + nEachVideoWidth;
        rcArea.bottom = top  + nEachVideoHeight;

        //�ұ߽�ֵ��������
        nVideoWidthResidualCount += nVideoWidthResidualInc;
        if(nVideoWidthResidualCount >= nSensorCount)
        {
            rcArea.right  ++;
            nVideoWidthResidualCount -= nSensorCount;

        }


        left += nEachVideoWidth;

        if(i != nSensorCount - 1)
        {
            RECT& rcSplitter = m_vecSplitter[i];
            rcSplitter.left   = left;
            rcSplitter.top    = top;
            rcSplitter.right  = left + SPLITTER_WIDTH;
            rcSplitter.bottom = top  + nEachVideoHeight;

            left += SPLITTER_WIDTH;
        }


		//
		::InflateRect(&rcArea, -SELECT_BOUND_WIDTH, -SELECT_BOUND_WIDTH);

    }//for

}

//@����:������������
//@����:allCfgData, ����ͼ�񴫸�����������Ϣ
void CIWBSensorManager::SetCfgData( TSysConfigData& sysCfgData)
{
    for(size_t i=0; i < sysCfgData.vecSensorConfig.size(); i++)
    {
        if(i >= m_vecSensors.size()) break;

        m_vecSensors[i]->SetCfgData(sysCfgData.vecSensorConfig[i], &sysCfgData.globalSettings);
        //���ù���ģʽ
        //m_vecSensors[i]->SetLensMode(sysCfgData.globalSettings.eLensMode);
        m_vecSensors[i]->SetLensMode(sysCfgData.vecSensorConfig[i].eLensMode);
    } 
    //
	if (theApp.GetUSBKeyTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD || theApp.GetUSBKeyTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
	{
		sysCfgData.globalSettings.bTouchHIDMode = true;
		sysCfgData.globalSettings.bTouchTUIOMode = false;
	}
    this->m_pSpotListProcessor->GetVirtualHID().SetHIDMode(sysCfgData.globalSettings.eHIDDeviceMode);

	this->m_pSpotListProcessor->GetVirtualHID().SetSinglePointMode(sysCfgData.globalSettings.bSinglePointMode?true:false);

	this->m_pSpotListProcessor->GetVirtualHID().SetAirOperateMode(sysCfgData.globalSettings.bAirOperatePermission?true:false, sysCfgData.globalSettings.eClickMode);
	//////add by zhaown 2019.9.25
	////�������ģʽ��Ϊfalse�Ļ�����ôǿ��HIDģʽ��Ӧ
	if ((!sysCfgData.globalSettings.bTouchHIDMode) && (!sysCfgData.globalSettings.bTouchTUIOMode))
	{
		this->m_pSpotListProcessor->GetVirtualHID().SetTouchHIDMode(true);
		sysCfgData.globalSettings.bTouchHIDMode = true;
	}
	else
	{
    	this->m_pSpotListProcessor->GetVirtualHID().SetTouchHIDMode(sysCfgData.globalSettings.bTouchHIDMode?true:false);
	}
	this->m_pSpotListProcessor->GetVirtualHID().SetTouchTUIOMode(sysCfgData.globalSettings.bTouchTUIOMode?true:false);


    //<Added by Jiqw 201412041914
    //<Added Reason: �������ģʽ�£�windows������������windows�����Ƶĳ�ͻ����/>
    g_oWGRConfig.SetHIDMode(E_DEV_MODE_TOUCHSCREEN == sysCfgData.globalSettings.eHIDDeviceMode);
    g_oGLBoardGR.SetIsTouchPadMode(E_DEV_MODE_TOUCHSCREEN == sysCfgData.globalSettings.eHIDDeviceMode);


    //if (theApp.GetScreenMode() >= EScreenModeDouble)
    if(1)
    {
        //����������Ļ������Ϣ�Ͷ������ӵ���Ŀһ�£���������Ļ������Ϣ
        //ѡ�������ļ�����Ļ�����ڵ�ǰʵ����Ļ���Ĳ���,
        int nSensorCount = m_vecSensors.size();

        SplitMode splitMode;
        
        bool bRet =  sysCfgData.screenLayoutManger.GetSplitMode(nSensorCount, splitMode);

        if (bRet)
        {
            const TScreenLayout* pScreenLayout = sysCfgData.screenLayoutManger.GetScreenLayout(splitMode);
            if (pScreenLayout)
            {
                this->m_oScreenLayoutDesigner.SetScreenLayout(*pScreenLayout);
                this->ApplyScreenLayout();
            }
        }
    }
}

void CIWBSensorManager::SetGlobalCfgData(TSysConfigData& sysCfgData)
{
	for (size_t i = 0; i < sysCfgData.vecSensorConfig.size(); i++)
	{
		if (i >= m_vecSensors.size()) break;
		m_vecSensors[i]->SetGlobalCfgData(&sysCfgData.globalSettings);
	}
}


//@����:��ȡ��������
//@����:allCfgData, ����ͼ�񴫸�����������Ϣ
BOOL CIWBSensorManager::GetCfgData(TSysConfigData& sysCfgData)
{
    sysCfgData.globalSettings.eHIDDeviceMode = this->m_pSpotListProcessor->GetVirtualHID().GetHIDMode();

    size_t nSensorCount = m_vecSensors.size();
    if(nSensorCount == 0)return FALSE;
//  sysCfgData.vecSensorConfig.resize(nSensorCount);

    for(size_t i=0; i < nSensorCount; i++)
    {
        sysCfgData.vecSensorConfig[i] = m_vecSensors[i]->GetCfgData();
    }

    //ȫ�ֹ���ģʽ�͵�һ�����������Ĺ���ģʽ����һ�¡�
    sysCfgData.globalSettings.eLensMode = m_vecSensors[0]->GetLensMode();
    
    //if (theApp.GetScreenMode() >= EScreenModeDouble)
    //Modifed by xuke, 2024/06/28
    if(1)
    {
		const TScreenLayout& screenLayout    = m_oScreenLayoutDesigner.GetScreenLayout();
        sysCfgData.screenLayoutManger.SetSplitMode(nSensorCount, screenLayout.GetSplitMode());
        sysCfgData.screenLayoutManger.SetScreenLayout(screenLayout);
    }

    return TRUE;
}


//@����:USB����ͷ�����¼���Ӧ
//@����:lpszDevicePath, �����USB����ͷ���豸·��
//void CIWBSensorManager::OnCameraPlugIn(LPCTSTR lpszDevicePath)
void CIWBSensorManager::OnCameraPlugIn(const TCaptureDeviceInstance& devInst)
{
    LOG_INF("CIWBSensorManager::OnCameraPlugIn(%s)\r\n", (const char*)CT2A(devInst.m_strDevPath));
    //���ȷ����豸·����ȫƥ�������ͷ
    BOOL bMatched = FALSE;
    int  nCandidateIndex  = -1;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];

        if(sensor.IsDetecting()) continue;
		//����Ǵ��²�������ͷ�����Ҽ��ܹ��б仯
		sensor.UpdateTouchTypeFromUSBKey();

        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();
        ////�����·����ȵĻ���Ҫ�Ƚ��ǲ���PID��VID���//modify by zhaown 
        if( (_tcsicmp(devInfo.m_strDevPath, devInst.m_strDevPath) == 0) &&(devInfo.m_nPID == devInst.m_nPID)&&(devInfo.m_nVID == devInst.m_nVID))
        {
			sensor.SetDeviceInfo(devInst);
            //sensor.Run();
            StartRunning(sensor.GetID());

            bMatched = TRUE;
            break;
        }
        else if(nCandidateIndex == -1)
        {
            nCandidateIndex = i;
        }

    }//for

    if(!bMatched && nCandidateIndex != -1)
    {
        m_vecSensors[nCandidateIndex]->SetDeviceInfo(devInst);
        //m_vecSensors[nCandidateIndex]->Run();
        StartRunning(m_vecSensors[nCandidateIndex]->GetID());

    }
    //
}


//@����:USB����ͷ�γ��¼���Ӧ,  �����USB����ͷ���豸·��
void CIWBSensorManager::OnCameraPlugOut(LPCTSTR lpszDevicePath)
{
    LOG_INF("CIWBSensorManager::OnCameraPlugOut(%s)\r\n", (const char*)CT2A(lpszDevicePath));
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];
        if(!sensor.IsDetecting()) continue;

        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();

        if(_tcsicmp(devInfo.m_strDevPath, lpszDevicePath) == 0)
        {
            //������ڽ��й�߲ɼ��У�����ʾ�Ƿ��˳���߲ɼ�
            if(sensor.GetPenPosDetector()->IsCollectingSpot())
            {
                this->m_wndLightSpotSampling.OnDeviceMissing();
            }

            //�����豸��ʧ��Ӧ����
            sensor.OnDeviceIsMissing();

            //ֹͣ����
            this->StopRunning(sensor.GetID());

            sensor.ShowMissStatusInfo();

            break;
        }
    }


}


//@����:����λ��ȷ��ͼ���������ĸ���������
//@����:�ǿգ�����ָ�򴫸��������ָ��
//      NULL,δ�ҵ���
CIWBSensor* CIWBSensorManager::SensorFromPt(const POINT& ptPos)
{
    CIWBSensor* pSensor = NULL;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        if(::PtInRect(&m_vecVideoLayout[i], ptPos))
        {
            pSensor = m_vecSensors[i];
            break;
        }
    }

    return pSensor;
}


void CIWBSensorManager::SelectAsCurrentSensor(CIWBSensor* pSensor)
{
	m_uCurrentSelectSensordId = pSensor->GetID();

	DrawSelectBound(m_hVideoDispWnd);
}

int  CIWBSensorManager::CurrentSelectSensor()
{
	return m_uCurrentSelectSensordId;
}

//CIWBSensor* CIWBSensorManager::GetSensor0()
//{
//	if (m_vecSensors.size() == 0) return NULL;
//	return m_vecSensors[0];
//}

CIWBSensor* CIWBSensorManager::GetSensor()
{
    
	if (m_vecSensors.size() == 0) return NULL;

	if (0 < m_uCurrentSelectSensordId && m_uCurrentSelectSensordId < m_vecSensors.size())
	{
		return m_vecSensors[m_uCurrentSelectSensordId];
	}
	else
	{
		return m_vecSensors[0];
	}
}

const CIWBSensor* CIWBSensorManager::GetSensor(int nID) const
{
    if ((size_t)nID >= m_vecSensors.size()) return NULL;
    return m_vecSensors[nID];	
}

CIWBSensor* CIWBSensorManager::GetSensor(int nID)
{
	if ((size_t)nID >= m_vecSensors.size()) return NULL;
	return m_vecSensors[nID];
}
//@����:ʹ�ܹ��
//@����:bEnable,ʹ�ܱ�־
void CIWBSensorManager::EnableOpticalPen(BOOL bEnable)
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    { 
         if(!bEnable)//����ǽ��ù��, ��������������
         {
             m_vecSensors[i]->EnableOpticalPen(bEnable);
         }
         else
         {
             //�����ʹ�ܹ�ʣ�ֻ����"����ʹ��ģʽ��"����ʹ�ܹ��, �������Ա�������ģʽ��,���Ź����ɵĹ��������
             if(m_vecSensors[i]->GetLensMode() == E_NORMAL_USAGE_MODE)
             {
                  m_vecSensors[i]->EnableOpticalPen(bEnable);
             }
         }
    }
}

//@����:�жϹ���Ƿ��ڿ�����
//@˵��:ֻҪ��һ֧����ڿ�����,�򷵻�TRUE
BOOL CIWBSensorManager::IsOpticalPenControlling()
{
   // BOOL bIsControlling = FALSE;
	BOOL bIsControlling = TRUE;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
//      bIsControlling |= m_vecSensors[i]->IsOpticalPenControlling();
		bIsControlling &= m_vecSensors[i]->IsOpticalPenControlling();
    }

    return bIsControlling;
}

//@����:��ʼͼ�񴫸������Զ�У������
//@����:ePattern, У��ͼ��ģʽ
//      hNotifWindow, У���������֪ͨ��Ϣ"WM_AUTO_CALIBRATE_DONE"�Ľ��մ���
//      nSensorID, -1,ȫ��������У��
void  CIWBSensorManager::StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifyWindow, int nSensorID)
{
    if(nSensorID == -1)
    {
        //��ʼ��У���������
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for(size_t i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //���д��������ù��
        for(size_t i=0; i<m_vecSensors.size(); i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);
            m_vecSensors[i]->Reset_Start_Offset();
        }

        if(m_vecSensors.size() >= 1)
        {
            for(size_t i=0; i<m_vecSensors.size(); i++)
            {
                if(m_vecSensors[i]->IsDetecting())
                {
                    m_nCurrentSensorID = i;
                    m_vecSensors[i]->StartAutoCalibrate(ePattern, hNotifyWindow);
                    
                    break;
                }
                else
                {
                    m_vecCalibrateResults[i] = FALSE;
                }
            }
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        if(m_vecSensors[nSensorID]->IsDetecting())
        {
            m_nCurrentSensorID = nSensorID;
            m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
            m_vecSensors[nSensorID]->Reset_Start_Offset();
            m_vecSensors[nSensorID]->StartAutoCalibrate(ePattern, hNotifyWindow);

            m_eOperationMode = E_MODE_SINGLE_SENSOR;
            
            m_vecCalibrateResults.resize(1);
            m_vecCalibrateResults[0] = FALSE;
        }

    }

    m_nAutoCalibrateTryTimes = 0;
    m_hNotifyWindow = hNotifyWindow;
    m_eAutoCalibratePattern = ePattern;
}


//@����:һ��ͼ�񴫸����Զ�У����ɺ���¼���Ӧ����
//@����:bSuccess, �ɹ�/ʧ�ܱ�־
//      bSimulateMode,ģ��У��ģʽ
void CIWBSensorManager::OnIWBSensorAutoCalibrateDone(BOOL bSuccess, BOOL bSimulateMode)
{
    if(!bSuccess)//ʧ����
    {
        if (bSuccess != E_AUTO_CALIBRATOR_ERROR_NOT_FOUND_DEVICE)
        {
            m_nAutoCalibrateTryTimes ++;
            if(!bSimulateMode && m_nAutoCalibrateTryTimes < MAX_AUTOCALIBRATE_TRY_TIMES)
            {
                 //�ٽ���һ��У��
                 m_vecSensors[m_nCurrentSensorID]->StartAutoCalibrate(
                      m_eAutoCalibratePattern,
                      m_hNotifyWindow);
                 return;
           }
        }
    }
    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        while(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;
            
            if(m_vecSensors[m_nCurrentSensorID]->IsDetecting())
            {
                m_vecSensors[m_nCurrentSensorID]->StartAutoCalibrate(
                    m_eAutoCalibratePattern,
                    m_hNotifyWindow);

                return;//������һ�����������Զ�У����
            }
            
        }//while
         
        for(unsigned int i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->OnAutoCalibrateDone(m_vecCalibrateResults[i]);
        }

    }
    else if(m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->OnAutoCalibrateDone(m_vecCalibrateResults[0]);

    }

    //����У������ֲ�
    UpdateCalibrateToleranceDistribute();
}


//@����:��ʼͼ�񴫸������ֶ�У������
//@����:
//      hNotifWindow, У���������֪ͨ��Ϣ"WM_FINISH_MAUNUAL_CALIBRATING"�Ľ��մ���
//      nPtsInRow, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
//      nPtsInCol, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
//      nSensorID, -1,ȫ��������У��
void  CIWBSensorManager::StartManualCalibrate(HWND hNotifyWindow, int nPtsInRow, int nPtsInCol, int nSensorID, EManualCalibrateType eManualCalibType)
{
    if(nSensorID == -1)
    {
        //��ʼ��У���������
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for(size_t i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //���д��������ù��
        for(size_t i=0; i<m_vecSensors.size(); i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);
        }


        if(m_vecSensors.size() >= 1)
        {
            m_nCurrentSensorID = 0;
            m_vecSensors[0]->StartManualCalibrate(hNotifyWindow, nPtsInRow, nPtsInCol, eManualCalibType);
            
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->StartManualCalibrate(hNotifyWindow, nPtsInRow, nPtsInCol, eManualCalibType);

        m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID   = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;

    }
    m_hNotifyWindow = hNotifyWindow;
}

//@����:��ʼͼ�񴫸������ֶ�У������
//@����:
//      hNotifWindow, У���������֪ͨ��Ϣ"WM_FINISH_MAUNUAL_CALIBRATING"�Ľ��մ���
//      nPtsInRow, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
//      nPtsInCol, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
//      nSensorID, -1,ȫ��������У��
void  CIWBSensorManager::StartShootCalibrate(HWND hNotifyWindow, int nPtsInRow, int nPtsInCol, int nSensorID, EManualCalibrateType eManualCalibType)
{
    if (nSensorID == -1)
    {
        //��ʼ��У���������
        //m_vecCalibrateResults.resize(m_vecSensors.size());

        //for (size_t i = 0; i < m_vecCalibrateResults.size(); i++)
        //{
        //    m_vecCalibrateResults[i] = FALSE;
        //}

        //���д��������ù��
        //for (size_t i = 0; i < m_vecSensors.size(); i++)
        //{
        //    m_vecSensors[i]->EnableOpticalPen(FALSE);
        //}


        if (m_vecSensors.size() >= 1)
        {
            //m_nCurrentSensorID = 0;
            m_vecSensors[0]->StartShootCalibrate(hNotifyWindow/*, nPtsInRow, nPtsInCol, eManualCalibType*/);
            m_vecSensors[0]->Reset_Start_Offset();
        }

        //m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if (0 <= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        //m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->StartShootCalibrate(hNotifyWindow/*, nPtsInRow, nPtsInCol, eManualCalibType*/);
        m_vecSensors[nSensorID]->Reset_Start_Offset();

        /*m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;*/

    }
    //m_hNotifyWindow = hNotifyWindow;
}

void  CIWBSensorManager::LeaveShootCalibrateMode(int nSensorID)
{
    if (nSensorID == -1)
    {
        //��ʼ��У���������
        //m_vecCalibrateResults.resize(m_vecSensors.size());

        //for (size_t i = 0; i < m_vecCalibrateResults.size(); i++)
        //{
        //    m_vecCalibrateResults[i] = FALSE;
        //}

        //���д��������ù��
        //for (size_t i = 0; i < m_vecSensors.size(); i++)
        //{
        //    m_vecSensors[i]->EnableOpticalPen(FALSE);
        //}


        if (m_vecSensors.size() >= 1)
        {
            //m_nCurrentSensorID = 0;
            m_vecSensors[0]->LeaveShootCalibrateMode();

        }

        //m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if (0 <= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        //m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->LeaveShootCalibrateMode();

        /*m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;*/

    }
    //m_hNotifyWindow = hNotifyWindow;
}

//@����:һ��ͼ�񴫸����ֶ�У������ɺ���¼���Ӧ����
//@����:bSuccess, �ɹ�/ʧ�ܱ�־    
void CIWBSensorManager::OnIWBSensorManualCalibrateDone(BOOL bSuccess, DWORD dwCtxData)
{
    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        if(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;

            m_vecSensors[m_nCurrentSensorID]->StartManualCalibrate(
                m_hNotifyWindow,
                dwCtxData & 0xFF,//rows
                (dwCtxData >> 8) & 0x0FF //cols
                );
            return;
        }
    }

    if(m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->OnManualCalibrateDone(m_vecCalibrateResults[0]);
    }
    else
    {
        for(unsigned int i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->OnManualCalibrateDone(m_vecCalibrateResults[i]);
        }
    }

    //����У������ֲ�
    UpdateCalibrateToleranceDistribute();
}


//@����:�����Զ���������
//@����:hNotifWindow, У���������֪ͨ��Ϣ"WM_SEARCH_SCREEN_AREA_DONE"�Ľ��մ���
//      nSensorID, ������ID; -1ʱ,Ϊȫ��������У����
void CIWBSensorManager::StartSearchMaskArea(HWND hNotifyWindow, int nSensorID)
{
    size_t nSensorCount = m_vecSensors.size();
    if(nSensorID == -1)
    {
        //���д��������ù��
        for(size_t i=0; i < nSensorCount; i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);

            m_vecSensors[i]->SwitchLensMode(E_LASER_TUNING_MODE);
        }

        if(m_vecSensors.size() >= 1)
        {//???���������δ������ô��?
             for(size_t i = 0; i < nSensorCount; i++)
            {  
                if(m_vecSensors[i]->IsDetecting())
                {//��һ����������������ͼ�񴫸��������Զ���������
                    m_nCurrentSensorID = i;
                    m_vecSensors[i]->StartAutoMasking(hNotifyWindow);
                    break;
                }
             }
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        if(m_vecSensors[nSensorID]->IsDetecting())
        {
            //�Զ��������ʱ, �ڶ���ģʽ�����м��������д򿪣��������ɵ�����ͼ
            //ֻ���β��ָ��š�
            m_vecLastSensorLensMode.resize(nSensorCount);

            for (size_t i = 0; i < nSensorCount; i++)
            {
                m_vecLastSensorLensMode[i] = m_vecSensors[i]->GetCfgData().eLensMode;
                m_vecSensors[i]->SwitchLensMode(E_LASER_TUNING_MODE);
            }

            //
            m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
            m_vecSensors[nSensorID]->StartAutoMasking(hNotifyWindow);

            m_eOperationMode    = E_MODE_SINGLE_SENSOR;
            m_nCurrentSensorID = nSensorID;
        }
    }
    
    m_hNotifyWindow = hNotifyWindow;
}

//@����:��"�������������"�¼�����Ӧ����
//@����:bSuccess, �ɹ�/ʧ�ܱ�־
void CIWBSensorManager::OnIWBSensorSearchMaskAreaDone(BOOL bSuccess)
{
    m_vecSensors[m_nCurrentSensorID]->OnAutoSearchMaskAreaDone(bSuccess);

    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//���д������Զ�����ģʽ

        if(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;
            m_vecSensors[m_nCurrentSensorID]->StartAutoMasking(
                m_hNotifyWindow);
            return;
        }
    }
    else
    {
        size_t nSensorCount = m_vecSensors.size();
        //�ָ�����Sensor��״̬
        for (size_t nSensorID = 0; nSensorID < nSensorCount; nSensorID++)
        {
            if (nSensorID == m_nCurrentSensorID) continue;
            
            m_vecSensors[nSensorID]->SwitchLensMode(m_vecLastSensorLensMode[nSensorID]);
        }

    }
}



//@����:�ж�У���Ƿ�ɹ�
BOOL CIWBSensorManager::IsCalibarateOk()
{
    if(E_MODE_ALL_SENSOR == m_eOperationMode)
    {
        for(UINT i=0; i < m_vecCalibrateResults.size(); i++)
        {
            if(m_vecCalibrateResults[i] == FALSE)
            {
                return FALSE;
            }
        }
    }
    else
    {
        return m_vecCalibrateResults[0];
    }

    return TRUE;

}


//@����:������߲ɼ�����
void CIWBSensorManager::StartLightSpotSampling(HWND hNotifyWindow, int nSensorID)
{
	if(nSensorID > -1)
	{
		m_nCurrentSensorID = nSensorID;
	}
	else
	{
        m_nCurrentSensorID = 0;

		//���д��������ù��
		for (size_t i = 0; i<m_vecSensors.size(); i++)
		{
			m_vecSensors[i]->EnableOpticalPen(FALSE);
		}
	}
    RECT rcArea;
    m_vecSensors[m_nCurrentSensorID]->GetAttachedScreenArea(rcArea);

	TCHAR szProfileFullPath[MAX_PATH];
	m_vecSensors[m_nCurrentSensorID]->GetCollectSpotShowPath(szProfileFullPath,_countof(szProfileFullPath));

    m_wndLightSpotSampling.StartCollectSpotSize(&rcArea, 1, hNotifyWindow, 3,3,nSensorID, szProfileFullPath, m_vecSensors.size());

    //�����������߲���״̬
    m_vecSensors[m_nCurrentSensorID]->StartLightSpotSampling(m_wndLightSpotSampling.m_hWnd);


    m_hNotifyWindow = hNotifyWindow;
}

//@����:��߲ɼ������¼�����Ӧ����
//@����:bSuccess, �ɹ�ʧ�ܱ�־
void CIWBSensorManager::OnIWBSensorLightSpotSamplingDone(BOOL bSuccess, int nSensorId)
{

    const ALL_LIGHTSPOT_SAMPLE_SIZE&  screenSamples = m_wndLightSpotSampling.GetScreenSamples();
    m_vecSensors[m_nCurrentSensorID]->OnLightSpotSamplingDone(screenSamples, bSuccess);
	if (nSensorId >-1)
	{
		//˵���ǵ����ɼ���
		return;
	}

    m_nCurrentSensorID  ++;
	if (m_nCurrentSensorID == m_vecSensors.size())
	{
		//���д��������ù��
		for (size_t i = 0; i<m_vecSensors.size(); i++)
		{
			m_vecSensors[i]->EnableOpticalPen(TRUE);
		}
		return;
	}
    RECT rcArea;
    m_vecSensors[m_nCurrentSensorID]->GetAttachedScreenArea(rcArea);
	TCHAR szProfileFullPath[MAX_PATH];
	m_vecSensors[m_nCurrentSensorID]->GetCollectSpotShowPath(szProfileFullPath, _countof(szProfileFullPath));

    m_wndLightSpotSampling.StartCollectSpotSize(&rcArea, 1, m_hNotifyWindow, 3, 3, nSensorId, szProfileFullPath, m_vecSensors.size());

    //�����������߲���״̬
    m_vecSensors[m_nCurrentSensorID]->StartLightSpotSampling(m_wndLightSpotSampling.m_hWnd);

}


//@����:�ж����еĲ����Ѿ����
BOOL CIWBSensorManager::AllSamplingIsDone()
{
    return TRUE;
}



//@����:�жϴ������豸�Ƿ�������
//@����;��
//@˵��:ֻҪ��һ�������������У�����Ϊ��������״̬
BOOL CIWBSensorManager::IsRunning()const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        if(m_vecSensors[i]->IsDetecting())
        {
            return TRUE;
        }
    }//for

    return FALSE;
}

//@����:�ж����д������豸ֹͣ����
BOOL CIWBSensorManager::IsAllSensorStopped()const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        if(m_vecSensors[i]->IsDetecting())
        {
            return FALSE;
        }
    }//for

    return TRUE;
}


//���Թ�����غ���
//@����:�ж��Ƿ�����¼��
BOOL CIWBSensorManager::IsRecording() const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   

        //ֻҪ��һ����������¼����Ϊ����¼��״̬��
        if(m_vecSensors[i]->GetInterceptFilter()->IsRecording())
        {
            return TRUE;
        }

    }//for

    return FALSE;
}

//���Թ��ߺ���
//@����:��ʼ¼��
BOOL CIWBSensorManager::StartRecording()
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
            m_vecSensors[i]->GetInterceptFilter()->StartRecording();
    }//for
    return TRUE;
}


//���Թ��ߺ���
//@����:ֹͣ¼��
BOOL CIWBSensorManager::StopRecording()
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        m_vecSensors[i]->GetInterceptFilter()->StopRecording();

    }//for
    
    return TRUE;
}

//���Թ��ߺ���
//@����:������Ƶ����ΪAVI¼���ļ�
//@����:lpszVideoPath1,������1��·��
//      lpszVideoPath2,������2��·��
void CIWBSensorManager::LoadSrcFromAVI(LPCTSTR lpszVideoPath1, LPCTSTR lpszVideoPath2)
{
	int nSensorID = 0;
    if(lpszVideoPath1 && _tcslen(lpszVideoPath1))
    {
		CString strpath1 = lpszVideoPath1;
		int pos = strpath1.Find(_T("Sensor"));
		if (pos > -1)
		{
			strpath1 = strpath1.Mid(pos + _tcslen(_T("Sensor")), 1);
		    nSensorID = _ttoi(strpath1);
		}

        if(m_vecSensors[nSensorID] && m_vecSensors[nSensorID]->IsDetecting())
        {
            m_vecSensors[nSensorID]->GetInterceptFilter()->InputFromAVIFile(lpszVideoPath1);
        }
    }

    if (m_vecSensors.size() > 1)//2019/10/15
    {
	    nSensorID = 1;
        if (lpszVideoPath2 && _tcslen(lpszVideoPath2))
        {
			CString strpath2 = lpszVideoPath1;
			int pos = strpath2.Find(_T("Sensor"));
			if (pos > -1)
			{
				strpath2 = strpath2.Mid(pos + _tcslen(_T("Sensor")), 1);
			    nSensorID = _ttoi(strpath2);
			}
            if (m_vecSensors[nSensorID] && m_vecSensors[nSensorID]->IsDetecting())
            {
                m_vecSensors[nSensorID]->GetInterceptFilter()->InputFromAVIFile(lpszVideoPath2);
            }
        }
    }
}

////@����:��������ͼ�񴫸�������ʾ����
//void CIWBSensorManager::SwapSensorImage()
//{
//    //��������Ŀ����2,�����κβ�����
//    if(m_vecSensors.size() < 2) return;
//
//    TSensorConfig  cfgForSensor0 = this->m_vecSensors[0]->GetCfgData();
//    TSensorConfig  cfgForSensor1 = this->m_vecSensors[1]->GetCfgData();
//    
//    //����������������������
//    //˵��:�ؼ�����ͨ�������������ݣ�ʹ������������������豸·����������, 
//    this->m_vecSensors[0]->SetCfgData(cfgForSensor1);
//    this->m_vecSensors[1]->SetCfgData(cfgForSensor0);
//    //ֹͣ
//    StopRunning();
//    //��������
//    StartRunning();
//}


//@����:��������ͼ�񴫸�������ʾ����
//@����:��һ��ͼ�񴫸�����Id
//      �ڶ���ͼ�񴫸�����id
void CIWBSensorManager::SwapSensorImage(UINT nFirstSensorId, UINT nSecondSensorId)
{
    LOG_INF("CIWBSensorManager::SwapSensorImage(%d,%d).\r\n", nFirstSensorId, nSecondSensorId);
    if (nFirstSensorId < 0  || nFirstSensorId  >= m_vecSensors.size()) return;
    if (nSecondSensorId < 0 || nSecondSensorId >= m_vecSensors.size()) return;


    TSensorConfig  cfgForSensorFirst  = this->m_vecSensors[nFirstSensorId ]->GetCfgData();
    TSensorConfig  cfgForSensorSecond = this->m_vecSensors[nSecondSensorId]->GetCfgData();
	
	TCaptureDeviceInstance devInstFirst = this->m_vecSensors[nFirstSensorId]->GetDeviceInfo();
	TCaptureDeviceInstance devInstSecond = this->m_vecSensors[nSecondSensorId]->GetDeviceInfo();

    //����������������������
    //˵��:�ؼ�����ͨ�������������ݣ�ʹ������������������豸·����������, 
    this->m_vecSensors[nFirstSensorId ]->SetCfgData (cfgForSensorSecond);
    this->m_vecSensors[nSecondSensorId]->SetCfgData (cfgForSensorFirst );

	
	this->m_vecSensors[nFirstSensorId ]->SetDeviceInfo(devInstSecond);
	this->m_vecSensors[nSecondSensorId]->SetDeviceInfo(devInstFirst);


	//��ȡ����ͼ�񴫸�����������Ϣ
	GetCfgData(g_tSysCfgData);

    //ֹͣ
    StopRunning();

    //��������
    StartRunning();
}




//@����:ͨ����ȡ¼���ļ������Զ�У������
//@����:nSensorId, ������Id
//      hNotifyWnd, У����Ϻ����Ϣ֪ͨ����
//      lpszAviFilePath, ¼���ļ�
//      cBrightness, У��ʱ�Ļ���ƽ������
//      eGridNumber, У�����̸�����
BOOL CIWBSensorManager::DoSimulateAutoCalibrate(int nSensorId, HWND hNotifyWnd, LPCTSTR lpszAviFilePath, SIZE szScreen, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber)
{
    if(nSensorId >= int(m_vecSensors.size())) return FALSE;

     m_eOperationMode      = E_MODE_SINGLE_SENSOR;
     m_nCurrentSensorID   = nSensorId;

    m_vecCalibrateResults.resize(1);
    m_vecCalibrateResults[0] = FALSE;

    m_vecSensors[nSensorId]->GetAutoCalibrator()->DoSimulateCalibrate(
        lpszAviFilePath,
        hNotifyWnd,
        szScreen.cx,
        szScreen.cy,
        cBrightness,
        eGridsNumber,
        E_CALIB_DEBUG_LEVEL_VERBOSE);

    return TRUE;
}

void CIWBSensorManager::ApplyScreenLayout()
{
    UINT nScreenAreaCount = 0;
    const RECT* pScreenAreas = m_oScreenLayoutDesigner.GetScreenAbsoluteLayouts(&nScreenAreaCount);


    for (UINT i = 0; i < m_vecSensors.size(); i++)
    {
        CIWBSensor* pSensor = m_vecSensors[i];
        
        UINT nDesiredAreaNo = pSensor->GetScreenAreaNo();

        if (nDesiredAreaNo < nScreenAreaCount)
        {
            m_vecSensors[i]->SetAttachedScreenArea(pScreenAreas[nDesiredAreaNo]);
        }
        else
        {
            //ȱʡ�������һ����Ļ����
            m_vecSensors[i]->SetAttachedScreenArea(pScreenAreas[nScreenAreaCount - 1]);
            m_vecSensors[i]->SetScreenAreaNo(nScreenAreaCount - 1);

        }
                
        //pScreenAreas++;
    }


    //
    const SplitMode&  splitMode = m_oScreenLayoutDesigner.GetScreenLayout().GetSplitMode();

    BOOL bAutoMerge = FALSE;

    if (splitMode.cols == 1 || splitMode.rows == 1)
    {

        for (UINT i = 0; i < m_vecSensors.size(); i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];
            if (pSensor->GetID() != pSensor->GetScreenAreaNo())
            {
                break;
            }

        }

        bAutoMerge = TRUE;
    }

    UINT nMergeAreaCount;
    const RECT* pMergeAreas = m_oScreenLayoutDesigner.GetAbsoluteMergeAreas(&nMergeAreaCount);
    if (pMergeAreas && nMergeAreaCount)
    {
        //this->m_oSpotListProcessor.GetSpotMerger().SetMergeAreas(pMergeAreas, nMergeAreaCount);
        this->m_pSpotListProcessor->GetSpotMerger().SetMergeAreas(pMergeAreas, nMergeAreaCount);
    }

    bAutoMerge = FALSE;
    this->m_pSpotListProcessor->EnableAutoMerge(bAutoMerge);

    //������Ļ��������
    g_tSysCfgData.screenLayoutManger.SetScreenLayout(m_oScreenLayoutDesigner.GetScreenLayout());
    g_tSysCfgData.screenLayoutManger.SetScreenTargetType(m_oScreenLayoutDesigner.GetScreenTargetType());
}

//@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
//@����:nScreenWidth, �µ���Ļ���
//      nScreenHeight,�µ���Ļ�߶�
void CIWBSensorManager::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    //for(size_t i=0; i< m_vecSensors.size(); i++)
    //{
    //    //const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
    //    //if(pDisplayDevInfo)
    //    //{
    //    //    m_vecSensors[i]->OnMonitorResolutionChange(pDisplayDevInfo->rcMonitor);
    //    //}
    //    m_vecSensors[i]->OnMonitorResolutionChange();
    //}

    //this->m_oSpotListProcessor.OnDisplayChange(nScreenWidth, nScreenHeight);
    this->m_pSpotListProcessor->OnDisplayChange(nScreenWidth, nScreenHeight);

    //֪ͨ��߲���ģ��, ��Ļ�ֱ��ʷ����˱仯
    this->m_wndLightSpotSampling.OnDisplayChange(nScreenWidth, nScreenHeight);

    //��������ʶ�����Ļ�ֱ��ʺ�����ߴ�
    m_execContext.OnPhysicalLengthChange(g_tSysCfgData.globalSettings.fScreenDiagonalPhysicalLength);

    m_execContext.OnDisplayChange(nScreenWidth, nScreenHeight);

    //�ع���Ļ����
    m_oScreenLayoutDesigner.OnDisplayChange(nScreenWidth, nScreenHeight);

    ApplyScreenLayout();
}

/*
@���ܣ��������ܼ�⵲���Ƿ���ʧ
*/
void CIWBSensorManager::StartDetectBackSplashVanished()
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
        if(pDisplayDevInfo)
        {
            m_vecSensors[i]->OnStartDetectBackSplashVanished();
        }
    }
}


/*
@���ܣ�ֹͣ���ܼ�⵲���Ƿ���ʧ
*/
void CIWBSensorManager::StopDetectBackSplashVanished()
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
        if(pDisplayDevInfo)
        {
            m_vecSensors[i]->OnStopDetectBackSplashVanished();
        }
    }
}


//@����:�鿴У������λ��
//@����:bShow,TRUE,��ʾ����
//            FALSE, ���ط���
//@Author:15077727@qq.com, 2015/09/09
BOOL CIWBSensorManager::CheckCalibrateSymbols(BOOL bShow)
{
    theApp.GetMonitorFinder().SearchDisplayDev();
    BOOL bRet = FALSE;
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        bRet |= m_vecSensors[i]->CheckCalibrateSymbolPos(bShow);
    }

    return bRet;
}


//�ж��Ƿ�У�������Ƿ��Ѿ���ʾ
BOOL CIWBSensorManager::IsCalibrateSymbolVisible()const
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        if(m_vecSensors[i]->IsCalibrateSymbolVisible()) return TRUE;
    }
    
    return FALSE;

}


BOOL CIWBSensorManager::CalibrateSymbolManualResample()
{
    if(m_vecSensors.size() == 0) return FALSE;

    m_vecSensors[0]->CalibrateSymbolManualResample();

    return TRUE;
}


//Added by 15077726@qq.com
//2015/11/9
//����:�ж��Ƿ��Ѿ����У��
BOOL CIWBSensorManager::IsCalibrated()const
{
     if(m_vecSensors.size() == 0) return FALSE;
     for(size_t i=0; i < m_vecSensors.size(); i++)
     {
         if(!m_vecSensors[i]->IsCalibrated()) return FALSE;
     }

     return TRUE;
}

//����:�ж��Ƿ���У���ɹ����������
BOOL CIWBSensorManager::HasCalibratedCamera()const
{
    for (size_t i = 0; i < m_vecSensors.size(); i++)
    {
        if (m_vecSensors[i]->IsCalibrated()) return TRUE;
    }

    return FALSE;
}


void CIWBSensorManager::OnTimer(LPVOID lpCtx)
{
    for (size_t i = 0; i < m_vecSensors.size(); i++)
    {
        if (m_vecSensors[i]->IsDetecting() && E_NORMAL_USAGE_MODE == m_vecSensors[i]->GetLensMode())
        {
            m_vecSensors[i]->OnTimer(lpCtx);

        }//if

    }//for-each(i)
}

//@����:��ʼ�ĵ�궨
//@����:hNotifyWindow, ��Ϣ���մ���
//      nSensorID, -1, ȫ���������궨
void  CIWBSensorManager::Start4BasePointMarking(HWND hNotifyWindow, int nSensorID)
{
    if (nSensorID == -1)
    {
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for (size_t i = 0; i < m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //���д��������ù��
        for (size_t i = 0; i < m_vecSensors.size(); i++)
        {
            m_vecSensors[i]->EnableOpticalPen(FALSE);
        }

        if (m_vecSensors.size() >= 1)
        {
            m_nCurrentSensorID = 0;
            m_vecSensors[0]->Start4BasePointMarking(hNotifyWindow);
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if (0 <= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->Start4BasePointMarking(hNotifyWindow);
        
        m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;
   }

}

//@����:4��궨������Ӧ����
//@����:bSuccess, TRUE,�ɹ�
void CIWBSensorManager::OnIWBSensor4BasePointMarkingDone(BOOL bSuccess)
{
    if (m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        if (m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {//������һ��Ļ��4��궨
            m_nCurrentSensorID ++;
            m_vecSensors[m_nCurrentSensorID]->Start4BasePointMarking(m_hNotifyWindow);
            return;
        }
    }

    if (m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->On4BasePointMarkingDone(bSuccess);
    }
    else
    {
        for (unsigned int i = 0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->On4BasePointMarkingDone(m_vecCalibrateResults[i]);
        }
    }

}
/////////////////////
void CIWBSensorManager::EnableOnlineScreenArea(BOOL bEnable)
{
	for (size_t i = 0; i < m_vecSensors.size(); i++)
	{
		m_vecSensors[i]->SetOnlineScreenArea(bEnable?true:false);
	}
}

BOOL CIWBSensorManager::IsEnableOnlineScreenArea()
{
	BOOL bIsControlling = TRUE;
	for (size_t i = 0; i < m_vecSensors.size(); i++)
	{
		//�ж������еģ������оͲ�������
		if (m_vecSensors[i]->IsDetecting()) 
		{
		    bIsControlling &= m_vecSensors[i]->IsEnableOnlineScreenArea();
		}
	}
	return bIsControlling;
}

//@����:����У������ֲ���
void CIWBSensorManager::UpdateCalibrateToleranceDistribute()
{
    const UINT nRows = 10;
    const UINT nCols = 10;
    const UINT nPtCount = nRows*nCols;
#if 0
    int nCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#else
    int nCx = GetSystemMetrics(SM_CXSCREEN);
    int nCy = GetSystemMetrics(SM_CYSCREEN);
#endif

    std::vector<TPoint2D> vecPtsOnScreen;
    vecPtsOnScreen.resize(nPtCount);
    
    //Y���������
    std::vector<double> vecY;
    vecY.resize(nRows);
   
    int interval  = nCy / nRows;
    int remainder = nCy % nRows;
    int y0 = 0;
    int remainderCount = 0;
    for (UINT r = 0; r < nRows; r++)
    {
        int y1 = y0 + interval;
        
        remainderCount += remainder;

        if (remainderCount > nRows)
        {
            remainderCount -= nRows;
            y1 ++;
        }

        vecY[r] = double(y0 + y1) / 2.0;

        y0 = y1;
    }
    
    //X���������
    std::vector<double> vecX;
    vecX.resize(nCols);
    interval  = nCx / nCols;
    remainder = nCx % nCols;
    remainderCount = 0;
    int x0 = 0;
    for (UINT c = 0; c < nCols; c++)
    {
        int x1 = x0 + interval;

        remainderCount += remainder;
        if (remainderCount > nCols)
        {
            x1 ++;
            remainderCount -= nCols;
        }

        vecX[c] = double(x0 + x1) / 2.0;

        x0 = x1;
    }


    for (UINT r = 0; r < nRows; r++)
    {
        TPoint2D pt;
        pt.d[1] = vecY[r];

        for (UINT c = 0; c < nCols; c++)
        {            
            pt.d[0] = vecX[c];
            vecPtsOnScreen[r*nCols + c] = pt;
        }
    }

    std::vector<double> vecTolX;//X���귽��Ķ�λ����
    std::vector<double> vecTolY;//Y���귽��Ķ�λ����
    vecTolX.resize(nPtCount);
    vecTolY.resize(nPtCount);

    for (size_t i = 0; i < nPtCount; i++)
    {
        vecTolX[i] = 0.0;
        vecTolY[i] = 0.0;
    }

    //����ÿ�������㴦��У������
    for (UINT i = 0; i < vecPtsOnScreen.size(); i++)
    {
        TPoint3D  pt3DWorld;
        pt3DWorld.d[0] = vecPtsOnScreen[i].d[0];
        pt3DWorld.d[1] = vecPtsOnScreen[i].d[1];
        pt3DWorld.d[2] = 0;

        for (UINT j = 0; j < m_vecSensors.size(); j++)
        {
            CIWBSensor* pSensor = m_vecSensors[j];
            if (pSensor == NULL)
            {
                continue;
            }
            RECT rcMonitorArea = {0, 0, 0, 0};
            pSensor->GetAttachedScreenArea(rcMonitorArea);

            POINT pt;
            pt.x = pt3DWorld.d[0];
            pt.y = pt3DWorld.d[1];

            if (!::PtInRect(&rcMonitorArea, pt))
            {
                continue;
            }

            //��Ļ�ϵĵ�λ�ڴ������Ĺ�Ͻ��Χ��,�ɸô������������
            CalibrateAlgo&  calibrateAlgo = pSensor->GetPenPosDetector()->GetVideoToScreenMap().GetCalibAlog();

            TPoint2D pt2DImage;
            calibrateAlgo.MapWorldToImage(&pt3DWorld, 1, &pt2DImage, 0);

            
            TPoint2D pt2DScreen;
            calibrateAlgo.GetScreenCoord(pt2DImage, &pt2DScreen,0, TRUE/*bWithoutAutoCalibCompensate*/);

            TPoint2D pt2DImageDeviate;

            //
            double deviation = 1.0;

            //
            //���ͼ��������ˮƽ�ʹ�ֱ����ֱ�ƫ��0.5������
            pt2DImageDeviate.d[0] = pt2DImage.d[0] + deviation;
            pt2DImageDeviate.d[1] = pt2DImage.d[1] + deviation;


            TPoint2D pt2DScreenDeviate;
            calibrateAlgo.GetScreenCoord(pt2DImageDeviate, &pt2DScreenDeviate,0, TRUE/*bWithoutAutoCalibCompensate*/);

            vecTolX[i] = fabs(pt2DScreenDeviate.d[0] - pt2DScreen.d[0]);
            vecTolY[i] = fabs(pt2DScreenDeviate.d[1] - pt2DScreen.d[1]);
            break;
        }

    }

    CToleranceDistribute& toleranceDistribute = m_pSpotListProcessor->GetToleranceDistribute();

    toleranceDistribute.UpdateToleranceDistribute(nRows, nCols, &vecTolX[0], &vecTolY[0]);

}