#pragma once
#include <stdio.h>
#include <fstream>
#include <vector>
#include <deque>
#include <atlbase.h>
#include <assert.h>

#ifndef LOG_FILE_NAME
    #define LOG_FILE_NAME   _T("RunTime.log")
#endif

 inline void _cdecl MyDebug(const char* szFormat,...)
{

    char buf[1024];
    va_list vlist;
    va_start(vlist,szFormat);

    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
        
    vsprintf_s(buf, _countof(buf), szFormat,vlist);
    OutputDebugStringA(buf);
    
    va_end(vlist);
}

#define WM_LOGMSG (WM_USER+119)

 class CAsyncLog
 {

 public:
     struct TLogBuf
     {
        char szText[1024];//
     };



     int GetFreeLogBufIndex()
     {
         CComCritSecLock<CComAutoCriticalSection> lock(m_csForFreeBuf);
         if(m_queueFreeBuf.empty())
         {
             return -1;
         }
        int nIndex =  m_queueFreeBuf.front();
        m_queueFreeBuf.pop_front();
        return nIndex;
     }

     void RecycleFreeLogBuf(size_t iLogBufIndex)
     {
            CComCritSecLock<CComAutoCriticalSection> lock(m_csForFreeBuf);
            m_queueFreeBuf.push_back(iLogBufIndex);
     }

     //@����:����д�����־�����������ŷ��롰��д����־�ڴ������"������
     //@����:iLogBufIndex, ��־������������
     void QueueNewLog(size_t iLogBufIndex)
     {
         CComCritSecLock<CComAutoCriticalSection> lock(m_csForWrittenBuf);
         m_queueWrittenBuf.push_back(iLogBufIndex);

         //����"����־�¼�"
         SetEvent(m_hEventNewLog);
     }

     //@����:��ȡһ����д�������־���ڴ��������
    int FetchWrittenLogBufIndex()
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForWrittenBuf);
        if(m_queueWrittenBuf.empty())
        {
            return -1;
        }
        int nIndex = m_queueWrittenBuf.front();
        m_queueWrittenBuf.pop_front();
        return nIndex;
    }


     TLogBuf& GetLogBuf(size_t iIndex)
     {
        assert(iIndex < m_vecLogBufs.size());

        return m_vecLogBufs[iIndex];
     }

     CAsyncLog(const TCHAR* szLogFileName)
     {
        Init(szLogFileName);
     }

     CAsyncLog()
     {
#ifdef LOG_FILE_NAME
        Init(LOG_FILE_NAME);
#else
        Init(_T("RunTime.log"));
#endif
     }

     ~CAsyncLog()
     {
         SetEvent(m_hThreadExitEvent);
         WaitForSingleObject(m_hLogThread,INFINITE);
         CloseHandle(m_hThreadExitEvent);
         CloseHandle(m_hLogThread);
         CloseHandle(m_hEventNewLog);


         m_oLogFile.close();

     }

 protected:

     BOOL Init(const TCHAR* szLogFileName)
     {
             m_oLogFile.open(szLogFileName,std::ios_base::app | std::ios_base::out,_SH_DENYNO );
             if(m_oLogFile.fail())
             {

                 return FALSE;
             }
             

             int nLogBufNum = 100;
             m_vecLogBufs.resize(nLogBufNum);

             for(int i=0; i < nLogBufNum; i++)
             {
                m_queueFreeBuf.push_back(i);
             }

             m_hEventNewLog   = CreateEvent(NULL, FALSE, FALSE, NULL);
                
             m_hThreadExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

             m_hLogThread = CreateThread(NULL,0,LogThread,this,0,&m_dwThreadId);

             return TRUE;
     }

     static unsigned long _stdcall LogThread(LPVOID lParam)
     {

         CAsyncLog* pLog = (CAsyncLog*)lParam;
         
        HANDLE events[2];
        events[0] = pLog->m_hThreadExitEvent;
        events[1] = pLog->m_hEventNewLog;

         std::string strLogSeparator="=================LOG Start=======================\r\n";
         pLog->m_oLogFile.write(strLogSeparator.c_str(), strLogSeparator.length());

         for(;;)
         {

            DWORD dwWaitResult = WaitForMultipleObjects(_countof(events), &events[0], FALSE, INFINITE);

            if(WAIT_OBJECT_0 == dwWaitResult)
            {//�߳��˳��¼�
                 break;
            }
            else if(WAIT_OBJECT_0 + 1 == dwWaitResult)
            {
                int nIndex = -1;
                while((nIndex = pLog->FetchWrittenLogBufIndex()) != -1)
                {

                    TLogBuf& refLogBuf = pLog->GetLogBuf(nIndex);

                    SYSTEMTIME sysTime;
                    GetLocalTime(&sysTime);

                    char szTime[64];
                    _snprintf_s(szTime,sizeof(szTime),
                      "[%04d/%02d/%02d %02d:%02d:%02d:%03d]",
                      sysTime.wYear,
                      sysTime.wMonth,
                      sysTime.wDay,
                      sysTime.wHour,
                      sysTime.wMinute,
                      sysTime.wSecond,
                      sysTime.wMilliseconds);

                      pLog-> m_oLogFile.write(szTime,strlen(szTime));
                      pLog-> m_oLogFile.write(refLogBuf.szText,strlen(refLogBuf.szText) );
                      pLog-> m_oLogFile.flush();

                      //����Log�ڴ��
                      pLog-> RecycleFreeLogBuf(nIndex);

                }//while
                
                

            }//else if


         }//for

         strLogSeparator="=================LOG End=======================\r\n";
         pLog->m_oLogFile.write(strLogSeparator.c_str(), strLogSeparator.length());

         return 0;
     }

     

protected:
     //��־������
     std::vector<TLogBuf> m_vecLogBufs;

     //��¼���п�����־�������Ķ���
     std::deque<int> m_queueFreeBuf;


     CComAutoCriticalSection m_csForFreeBuf;
  
     //��¼��д����־�Ķ���
     std::deque<int> m_queueWrittenBuf;

     CComAutoCriticalSection m_csForWrittenBuf;

     //����־�¼�
     HANDLE m_hEventNewLog;

     //��־�ļ�
     std::ofstream m_oLogFile;

     //��־�߳̾��
     HANDLE m_hLogThread;

     //��־�߳�ID
     DWORD  m_dwThreadId;

     //
     HANDLE m_hThreadExitEvent;//�߳��˳�֪ͨ�¼�
 };


//_declspec(selectany)
//CAsyncLog*  g_pAsyncLog = NULL;
//
//inline void _cdecl  AsyncLogInit(const TCHAR * szLogFileName)
//{
//    assert(g_pAsyncLog == NULL);
//    g_pAsyncLog = new CAsyncLog(szLogFileName);
//    
//}
//
//inline void _cdecl  AsyncLogUninit()
//{
//    if(g_pAsyncLog != NULL)
//    {
//        delete g_pAsyncLog;
//        g_pAsyncLog = NULL;
//    }
//
//    
//}

class CAsyncLog_Init
{
public:
    CAsyncLog_Init()
    {

    }

    ~CAsyncLog_Init()
    {

    }
    static CAsyncLog s_oAsyncLog;
};

_declspec(selectany) CAsyncLog CAsyncLog_Init::s_oAsyncLog;

//����:ȷ���κ�һ������Log.h��cpp�ļ����ܳ�ʼ�� CAsyncLog_Init::s_oAsyncLog.
static CAsyncLog_Init  s_AsyncLog_Init;

 inline void _cdecl AsyncLog(const char* szFormat,...)
 {

     //if(g_pAsyncLog == NULL) return;
     int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetFreeLogBufIndex();

     BOOL bRet   = FALSE;
     DWORD dwErr = 0;
     if(nBufIndex >=0 )
     {
         CAsyncLog::TLogBuf& refLogBuf = s_AsyncLog_Init.s_oAsyncLog.GetLogBuf(nBufIndex);

         sprintf_s(refLogBuf.szText, "[pid:0x%x][tid:0x%x]", GetCurrentProcessId(), GetCurrentThreadId());

         size_t nOffset = strlen(refLogBuf.szText);

         size_t textBufferSize = sizeof(refLogBuf.szText) - nOffset;
         va_list args;
        va_start(args, szFormat);


        vsnprintf_s(&refLogBuf.szText[nOffset], textBufferSize, textBufferSize, szFormat, args);

        s_AsyncLog_Init.s_oAsyncLog.QueueNewLog(nBufIndex);

        //����:��־��¼�������Ϣ�������ڵ��������̻߳�δ����, ��Ϣ����δ���ɣ������־��ʧ
        /*
         bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);

         if(!bRet)
         {

              dwErr = GetLastError();

         }
         */
     }

 };


#define LOG_TYPE_INF  0
#define LOG_TYPE_WRN  1
#define LOG_TYPE_ERR  2
#define LOG_TYPE_DBG  3
inline BOOL _cdecl AsyncTypedLog(int nType, const char* szFormat,...)
{
    BOOL   bRet  = FALSE;
     DWORD dwErr = 0;
    // if(g_pAsyncLog == NULL) return bRet;
    int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetFreeLogBufIndex();
    if(nBufIndex < 0 ) return bRet;

    CAsyncLog::TLogBuf& refLogBuf = s_AsyncLog_Init.s_oAsyncLog.GetLogBuf(nBufIndex);
    
    sprintf_s(refLogBuf.szText, "[pid:0x%x][tid:0x%x]", GetCurrentProcessId(), GetCurrentThreadId());

    size_t nOffset = strlen(refLogBuf.szText);
    size_t textBufferSize = sizeof(refLogBuf.szText) - nOffset;

    switch(nType)
    {
    case LOG_TYPE_INF://������Ϣ
        strcpy_s(&refLogBuf.szText[nOffset], textBufferSize, "[INF]");
        break;

    case LOG_TYPE_WRN://������Ϣ
        strcpy_s(&refLogBuf.szText[nOffset], textBufferSize, "[WRN]");
        break;

    case LOG_TYPE_ERR://������Ϣ
        strcpy_s(&refLogBuf.szText[nOffset], textBufferSize, "[ERR]");
        break;

    case LOG_TYPE_DBG:
        strcpy_s(&refLogBuf.szText[nOffset], textBufferSize,"[DBG]");
        break;

    default:
        strcpy_s(&refLogBuf.szText[nOffset], textBufferSize,"[UNK]");
        break;
    }
    va_list args;
    va_start(args, szFormat);
    
    nOffset = strlen(refLogBuf.szText);
    textBufferSize = sizeof(refLogBuf.szText) - nOffset;

    vsnprintf_s( &refLogBuf.szText[nOffset], textBufferSize, textBufferSize, szFormat, args);
    
    
    strcat_s(refLogBuf.szText, sizeof(refLogBuf.szText), "\r\n");



   s_AsyncLog_Init.s_oAsyncLog.QueueNewLog(nBufIndex);

   /*
    bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);
    if(!bRet)
    {
        dwErr = GetLastError();

    }
    */

    return bRet;
}

class CAutoFuncLog
{
public:
    CAutoFuncLog(const char* szFuncName)
    {
        strcpy_s(m_szFuncName, _countof(m_szFuncName), szFuncName);
        AsyncTypedLog(LOG_TYPE_INF, "Enter,%s", m_szFuncName);
    }

    ~CAutoFuncLog()
    {
        AsyncTypedLog(LOG_TYPE_INF, "Leave,%s", m_szFuncName);
    }
protected:
    char m_szFuncName[1024];
};
 
#define LOG_FUNC_CALL  CAutoFuncLog __log__(__FUNCTION__)

#ifdef LOG_ENABLED

//#define LOG_INIT(FileName)  AsyncLogInit(FileName)
//#define LOG_UNINIT          AsyncLogUninit

//#define LOG(Text)           AsyncLog Text

//������Ϣ
#define LOG_INF(format, ... )\
        AsyncTypedLog(LOG_TYPE_INF, format, __VA_ARGS__ )

//������Ϣ
#define LOG_WRN(format, ... )\
        AsyncTypedLog(LOG_TYPE_WRN, format, __VA_ARGS__ )

//������Ϣ
#define LOG_ERR(format, ... )\
        AsyncTypedLog(LOG_TYPE_ERR, format, __VA_ARGS__ )

//������Ϣ
#define LOG_DBG(format,...)\
        AsyncTypedLog(LOG_TYPE_DBG, format, __VA_ARGS__)
#else if

#define LOG_INIT(FileName) 
#define LOG_UNINIT 

//#define LOG(Text)

//������Ϣ
#define LOG_INF(format, ... ) 

//������Ϣ
#define LOG_WRN(format, ... ) 

//������Ϣ
#define LOG_ERR(format, ... ) 



//������Ϣ
#define LOG(format,...) 



#endif