#pragma once

#include "../../inc/InstallTip/IPCPublic.h"

#include <string>
#include <atlstr.h>

class CTipProcServer
{
public:
    CTipProcServer(void);
    ~CTipProcServer(void);

public:
    //void SetClientAppName(const char *clientAppName) {if (clientAppName) m_clientAppName = clientAppName;}
    void Attached2TheWnd(HWND hWnd) {m_attachWnd = hWnd;}
    void SetSceenMode(E_SCREEN_MODE eMode) {m_screenMode = eMode;}
    void SetCurrentLangCode(CAtlString strLangCode) {if (strLangCode != "") m_langCode = strLangCode;}

public:
    bool Start();
    bool Stop();

    void OnAutoCalibrateDone(bool bDoneSucced);
    void OnSpotSampleDone(bool bDoneSucced);

    bool IsStarted() {return m_hadStarted;}

    /*
    @���ܣ���⵲���Ƿ���ʧ��������ݣ�����nTotalSpotArea / nTotalSpots��ֵ������С��0������Ϊ��ʧ������δ��ʧ
    @˵���������ǰ�������㣺
          ��1����װ�򵼿�ʼ
          ��2����������������������
    */
    void OnDetectBackSlashVanish(const int &nTotalSpotArea, const int &nTotalSpots);

protected:
    bool SendConnectACKCommand(const CONN_ACK_INFO &ackInfo);
    bool SendRelayoutACKCommand(const RELAYOUT_ACK_INFO &ackInfo);
    bool SendTuningWebcamACKCommand(const TUNINGWEBCAM_ACK_INFO &ackInfo);
    bool SendTuningLaserACKCommand(const TUNINGLASER_ACK_INFO &ackInfo);
    bool SendAutoCaliACKCommand(const AUTOCALI_ACK_INFO &ackInfo);
    bool SendSampleACKCommand(const SAMPLE_ACK_INFO &ackInfo);
    bool SendTestExistedACKCommand(const EXIST_ACK_INFO &ackInfo);
    bool SendCloseCommand();
    bool SendBackSplashVanishedCommand(const BACKSPLASHVANISHED_INFO &info);

protected:
    void ParseAndProcRecieveMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool WrapAndSendCommand(const IPC_CMD_INFO &cmdInfo);          

private:
    LRESULT NewAttachWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPostProcRecCommand();
    bool    OnTimer(WPARAM idTimer);

    void    KillAllTimer();

private:    
    HWND               m_attachWnd;
    HWND               m_clientWnd;

    WNDPROC            m_oldAttachWndProc;

    DWORD              m_dwAuthToken;  

    IPC_CMD_INFO       m_curRecCMDInfo;

    DWORD              m_dwTimeOutDelay;

    bool               m_hadStarted;

    E_SCREEN_MODE      m_screenMode;

    CAtlString         m_langCode;

    bool               m_bOnEliminateBackSplashStep;
};
