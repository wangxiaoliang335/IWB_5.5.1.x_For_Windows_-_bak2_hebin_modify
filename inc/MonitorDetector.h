#pragma once

//@����:������չ����ʾ����Ϣ


//��ʾ����Ϣ
struct MonitorInfo
{
    POINT ptLefTop;
    int   nWidth  ;
    int   nHeigh  ;
    //BOOL  bValid  ;//��ȷ��־
};


class CDispMonitorFinder
{
public:
    CDispMonitorFinder();

    ~CDispMonitorFinder();
    
    //@��  ��:������ʾ��
    //@����֮:�ɹ�, TRUE
    //        ʧ��, FALSE
    BOOL SearchMonitor();


    //@��  ��:������ʾ��
    int GetMonitorCount()const;



    //@����:����ָ����ŵ���ʾ����Ϣ��
    //@����:nNum, ��ʾ����
    //@˵��:������ı�Ų�������ʵ�ʼ�⵽����ʾ�������򷵻�NULL
    const MonitorInfo* GetMointorInfo(int nNum)const;

protected:
    int m_nMonitorCount;
    MonitorInfo* m_pMonitorInfos; 

    static BOOL CALLBACK MonitorEnumProc(  HMONITOR hMonitor,  // handle to display monitor
              HDC hdcMonitor,     // handle to monitor DC
              LPRECT lprcMonitor, // monitor intersection rectangle
              LPARAM dwData);     // data;

private:

};