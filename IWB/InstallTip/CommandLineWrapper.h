#pragma once

#include <tchar.h>
#include <Windows.h>

class CCommandLineWrapper
{
public:
    CCommandLineWrapper(void);
    ~CCommandLineWrapper(void);

public:
    TCHAR *GetWrappedCommandLine() {return m_commandLine;};

public:
    bool WrapLangCode(const TCHAR *langCode);
    bool WrapServerWnd(const DWORD &serverWnd);
    /*
    @���ܣ���װֵΪscreenMode����Ļֵ��CommandLine
    @������screenMode����Ļ���͡���0��������1����˫��
    */
    bool WrapScreenMode(const byte &screenMode);

protected:
    bool WrapString(const TCHAR *str, const TCHAR *tag);

protected:
    static const unsigned  KMAXBUFFSIZE = 256;
    TCHAR           m_commandLine[KMAXBUFFSIZE];

    unsigned        m_hadUsed;  //ΪCommandLine�ṩ��Buff���Ѿ���ռ�õĴ�С
};
