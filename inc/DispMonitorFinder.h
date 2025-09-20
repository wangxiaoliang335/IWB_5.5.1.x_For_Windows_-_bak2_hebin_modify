#pragma once

#include <vector>
#include <wingdi.h>

//@����:��ʾ����������
struct DiplayAdpaterInfo
{
    DISPLAYCONFIG_PATH_TARGET_INFO   targetInfo;//Ŀ���������Ϣ
    DISPLAYCONFIG_TARGET_MODE        targetMode;//�Կ�����������ĸ�ʽ
    DISPLAYCONFIG_SOURCE_MODE        sourceMode;//����������Դ��ʽ
                        TCHAR        monitorDevicePath[128];//��ʾ���豸·��
                        TCHAR        moniotrName[CCHDEVICENAME];//��ʾ������
};
//��ʾ����Ϣ
struct DisplayDevInfo
{
	HMONITOR hMonitor;
    RECT   rcMonitor;
    TCHAR  szDevice[CCHDEVICENAME];
	BOOL   bIsPrimary             ;//�Ƿ���������־
//	DWORD  physicalMaxWidthInMM   ;//��Ļ������ߴ������, ��λ:mm
//	DWORD  physicalMaxHeightInMM  ;//��Ļ������ߴ����߶�, ��λ:mm

	DWORD  MaxWidthInPixel        ;//������ؿ��
	DWORD  MaxHeightInPixel       ;//������ظ߶�

	DEVMODE CurrentSetings        ;//��ǰ��ʾģʽ

	DWORD OptimalWidthInPixel     ;//���ſ��
	DWORD OptimalHeightInPixel    ;//���Ÿ߶�

	//DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName;
	//DISPLAYCONFIG_TARGET_DEVICE_NAME targetName;

	//<<2017/10/27,�µļ��������ݽṹ
	DISPLAYCONFIG_PATH_TARGET_INFO   targetInfo;//Ŀ���������Ϣ
	DISPLAYCONFIG_TARGET_MODE        targetMode;//�Կ�����������ĸ�ʽ
	DISPLAYCONFIG_SOURCE_MODE        sourceMode;//����������Դ��ʽ
	//>>

    std::vector<DiplayAdpaterInfo> displayAdapterInfos;

};

struct MonitorSignalFormat
{
	SIZE ActiveSize;
	SIZE PrimSurfSize;
};

class CDispDevFinder
{
public:
    CDispDevFinder();

    ~CDispDevFinder();
    
    //@��  ��:������ʾ��SearchDisplayDev
    //@����֮:�ɹ�, TRUE
    //        ʧ��, FALSE
    BOOL SearchDisplayDev();

    //@��  ��:������ʾ��
    int GetDisplayDevCount()const;


    //@����:����ָ����ŵ���ʾ����Ϣ��
    //@����:nNum, ��ʾ����
    //@˵��:������ı�Ų�������ʵ�ʼ�⵽����ʾ�������򷵻�NULL
    const DisplayDevInfo* GetDisplayDevInfo(int nNum)const;


	//@����:��ȡ�����������Ļ����Ϣ
	const DisplayDevInfo* GetDisplayDevInfo(int x, int y) const;


    //@����:ʹ����Ļƴ��ģʽ
    //@����:bEnable,�Ƿ�ʹ��ƴ��ģʽ
    //void EnableScreenMergeMode(BOOL bEnable);

	//@����:��ȡ��չ������Ϣ
	const DisplayDevInfo* GetFirstExtendMonitorInfo()const;

	//@����:��ȡ����Ļ��Ϣ��
	const DisplayDevInfo* GetPrimaryMonitorInfo()const;

protected:
    

    std::vector<DisplayDevInfo> m_vecDisplayDevInfo;

    static BOOL CALLBACK MonitorEnumProc(  HMONITOR hMonitor,  // handle to display monitor
              HDC hdcMonitor,     // handle to monitor DC
              LPRECT lprcMonitor, // monitor intersection rectangle
              LPARAM dwData);     // data;

private:
    BOOL m_bScreenMergeMode;
};