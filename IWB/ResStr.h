#ifndef H_INCLUDE_RegStr
#define H_INCLUDE_RegStr

#include <assert.h>
#include <string>
//#include <hash_map>
#include <unordered_map>
using namespace std;
using namespace stdext;
class CResStr{
public:
    CResStr(HINSTANCE hInst=NULL):m_hInst(hInst)
    {
       

    }
    
    //����Դ���ҳ������ַ�����Դ
    void Initialize()
    {

        if(m_hInst == NULL) return;
        BOOL bResult = EnumResourceTypes(
                        m_hInst,//HMODULE hModule,
                        EnumResTypeProc,// ENUMRESTYPEPROC lpEnumFunc,
                        (LONG_PTR)this     //LONG_PTR lParam
                        );
     

    }
    
    
    static BOOL CALLBACK EnumResTypeProc(
                    HMODULE hModule,
                    LPTSTR lpszType,
                    LONG_PTR lParam
                    )
    {
        if(lpszType == RT_STRING)
        {

            BOOL bResult=
                EnumResourceNames(
                hModule,//HMODULE hModule,
                RT_STRING,//LPCTSTR lpszType,
                EnumResNameProc,//ENUMRESNAMEPROC lpEnumFunc, StringTable 
                (LONG_PTR)lParam); 
        }
        return TRUE;
    }

    static BOOL CALLBACK EnumResNameProc(
                HMODULE hModule,
                LPCTSTR lpszType,
                LPTSTR lpszName,
                LONG_PTR lParam)
    {   
        CResStr* pThis = (CResStr*)lParam;
        assert(lpszType ==RT_STRING);
        if(!IS_INTRESOURCE(lpszName))
        {
            return TRUE;
        }
        UINT uFirstID = ((UINT)(lpszName) - 1)<<4;
        HRSRC hStringTable = FindResource(hModule,lpszName,RT_STRING);

        HGLOBAL hGlobal =  LoadResource(
                            hModule, //HMODULE hModule,
                            hStringTable //HRSRC hResInfo
                            );

        LPVOID pVoid = LockResource(hGlobal);
        WCHAR* lpsz = (WCHAR*)pVoid;
        int cch;

		
		//����ÿ��Buddle, EnumResNameProc����һ��
		//ÿ��Buddle�����16���ַ���

        for(int i=0; i<16;i++)
        {
             cch = *((WORD*)lpsz ++);//�ַ�������
            if(cch != 0)//NULL�����ָ��ַ������������������ַ���
            {
					CAtlString str;
					TCHAR* pDest = str.GetBuffer(cch);
					memcpy(pDest, lpsz, cch*sizeof(WCHAR));
					str.ReleaseBuffer(cch);

					assert(str.GetLength() == cch);

				    
					DwordString_Pair pair(uFirstID,str);
					pThis->m_stringHashTable.insert(pair);
					lpsz += cch;            
			}
            uFirstID ++;        
      
       }
        return TRUE;
    }


    void SetResInst(HINSTANCE hInst)//��CWinApp::InitInstance();����øú����趨��Դģ��
    {
        m_hInst = hInst;
        Initialize();

    }
    const LPCTSTR operator[](UINT uStrResID)const
    {
        
        //hash_map<DWORD,CAtlString>::const_iterator it = m_stringHashTable.find(uStrResID);
		unordered_map<DWORD, CAtlString>::const_iterator it = m_stringHashTable.find(uStrResID);
        if(it != m_stringHashTable.end())
        {
            return (LPCTSTR)it->second;
        }
        
        
        return  _T("");
    }
private:
    HINSTANCE  m_hInst               ;//��Դ����ģ��ľ��
    //hash_map<DWORD,CAtlString>  m_stringHashTable;//
	unordered_map<DWORD, CAtlString>  m_stringHashTable;
    typedef pair <DWORD, CAtlString> DwordString_Pair;


};



_declspec(selectany) CResStr g_oResStr;
#endif