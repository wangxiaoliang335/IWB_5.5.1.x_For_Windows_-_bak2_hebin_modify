#include "stdafx.h"
#include "headers.h"

//�м��ļ���Ŀ¼
CAtlString g_strIntermediatRootDirName = _T(".\\Intermediate");


void ChangeImageDebugSubDir(const TCHAR* szSubDirName)
{

    g_strImageDebugRoot = g_strIntermediatRootDirName + _T("\\");
    g_strImageDebugRoot += szSubDirName;
    
    //��������Ŀ¼Ŀ¼
	CreateFullDirectory(g_strImageDebugRoot);

}


void SetImageDebugDir(const TCHAR* szDirName)
{
    g_strImageDebugRoot = szDirName;
    //��������Ŀ¼Ŀ¼
	CreateFullDirectory(g_strImageDebugRoot);

}