#pragma once
#include "MyGDI.h"
//�Ի��˵�
class COwnerDrawMenuBase
{
public:
	explicit COwnerDrawMenuBase(HMENU hMenu = NULL)
	{
		m_hMenu = hMenu;

		HDC hDesktopDC = GetDC(GetDesktopWindow());
		m_hMemDC = ::CreateCompatibleDC(hDesktopDC);
		ReleaseDC(GetDesktopWindow(), hDesktopDC);
	}

	virtual ~COwnerDrawMenuBase()
	{

		DeleteDC(m_hMemDC);	

	}

	//@����:����һ���˵����
	BOOL Attach (HMENU hMenu)
	{
		m_hMenu = hMenu;
		return TRUE;
	}

	//����:������Ĳ˵�������룬 �����ظò˵����
	HMENU Detach()
	{
		return m_hMenu;
	}

	//@����:�趨�Ի��˵��������λͼ���
	//@����:uID, �˵���ID
	//      hBmp, �˵��������λͼ�����
	//    clrTransparent, ͸����ɫ
	//@˵��: �������˵�����Ϊ�Ի���
	BOOL SetMenuOwnerDrawBitmap(UINT uID, HBITMAP hBmp, COLORREF clrTransparent)
	{

		 HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return FALSE;
		//���ͼƬ������ɫΪ�˵�������ɫ
		COLORREF clrMenu = GetSysColor(COLOR_MENU); 
		ChangeBmpPixelColor(hBmp, clrTransparent, clrMenu);

		//���ò˵�����
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask  = MIIM_FTYPE/* |MIIM_ID|MIIM_BITMAP*/;
		

		BOOL bRet = TRUE;
		bRet = GetMenuItemInfo(hMenu, uID,  FALSE, &mii);
		if(!bRet) return FALSE;

		mii.fMask |=  MIIM_FTYPE | MIIM_DATA;/*| MIIM_BITMAP*/;//Retrieves or sets the fType and dwTypeData members

		
		mii.dwItemData = DWORD(hBmp);
		mii.fType   |=  MFT_OWNERDRAW;

		//MFT_OWNERDRAW
		//If this value is specified, the dwTypeData member contains an application-defined value.


		bRet = SetMenuItemInfo(hMenu, uID, FALSE, &mii);
		if(!bRet) 
		{
			DWORD dwError = GetLastError(); //0x00000057, ��������ȷ

			return FALSE;
		}

		return bRet;
	}

		//@����:�����Զ���˵���
	virtual void OnDrawMenuItem(LPDRAWITEMSTRUCT lpDrawItem)
	{
		//lpDrawItem
	}

	//@����:
	virtual void OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItem)
	{

	}

	HMENU GetMenuHandle()const
	{
		return m_hMenu;
	}

protected:


	SIZE GetMenuCaptionSize(UINT uID)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;



		//1.����˵���ߴ�
		//��ȡ�˵�����
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		SIZE sizeText    = {0,0};
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return sizeText;
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_STRING ;
		
			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			if(mii.cch == 0)//�ַ�������Ϊ0
			{
				break;
			}

			TCHAR szItemText[MAX_PATH];
			memset(szItemText, 0, sizeof(szItemText));

			
			mii.dwTypeData = szItemText;
			mii.cch = mii.cch + 1;

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}

			//TRACE(_T("menu %s\n"), szItemText);

			//
			NONCLIENTMETRICS ncm;
			memset(&ncm, 0, sizeof(ncm));
	

			//Windows Server 2003 and Windows XP/2000:  
			//If an application that is compiled for Windows Server 2008 or Windows Vista must also run on Windows Server 2003 or Windows XP/2000,
	        //use the GetVersionEx function to check the operating system version at run time and, if the application is running on Windows Server 
	        //2003 or Windows XP/2000, subtract the size of the iPaddedBorderWidth member from the cbSize member of the NONCLIENTMETRICS structure 
		    //before calling the SystemParametersInfo function.
			OSVERSIONINFO osvi;
		    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		    GetVersionEx(&osvi);
			if(osvi.dwMajorVersion <= 5)
			{
				ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth); //why? See http://msdn.microsoft.com/en-us/library/ms724506(VS.85).aspx
			}
			else
			{
				ncm.cbSize = sizeof(NONCLIENTMETRICS);
			}

			
			bRet = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}

			HFONT hFontMenu= CreateFontIndirect(&ncm.lfMenuFont);
			HDC   hDCDesktop = GetDC(GetDesktopWindow());
			HDC   hDCMem = ::CreateCompatibleDC(hDCDesktop);
			HFONT hFontOld = (HFONT)SelectObject(hDCMem,hFontMenu); 

			 

			::GetTextExtentPoint32(hDCMem, szItemText, _tcslen(szItemText),&sizeText);
			
			SelectObject(hDCMem, hFontOld);			
			DeleteObject(hFontMenu);
			DeleteDC(hDCMem);
			ReleaseDC(GetDesktopWindow(), hDCDesktop);
		}while(0);

		return sizeText;
	}


	SIZE GetItemBitmapSize(UINT uID)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;

		SIZE sizeBitmap  = {0,0};
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return sizeBitmap;

		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_DATA;
		

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			
			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			//��ȡ������ͼƬ�ĳߴ�
			if(mii.dwItemData)
			{
				BITMAP bitmap;
				
				bRet = GetObject(HBITMAP(mii.dwItemData), sizeof(BITMAP), &bitmap);

				if(!bRet)
				{
					dwErr = GetLastError();

				}

				sizeBitmap.cx = bitmap.bmWidth;
				sizeBitmap.cy = bitmap.bmHeight;
			}

		}while(0);

		return sizeBitmap;
	}


	HBITMAP GetMenuItemBitmap(UINT uID)
	{

		HBITMAP hBitmap = NULL;
		BOOL bRet = FALSE;
		DWORD dwErr = 0;
		
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
		if(hMenu == NULL) return NULL;

		SIZE sizeBitmap  = {0,0};
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		do
		{
			//fMask, Members to retrieve or set
			mii.fMask  = MIIM_DATA;
		

			bRet = GetMenuItemInfo(hMenu, uID, FALSE, &mii);

			
			if(!bRet)
			{
				dwErr = GetLastError();
				break;
			}


			hBitmap = (HBITMAP)mii.dwItemData;


		}while(0);

		return hBitmap;
	}


	BOOL  HasMenuID(UINT uID)
	{
		
		HMENU hMenu = GetMenuHandle(this->m_hMenu, uID);
	
		return hMenu != NULL;
	}


	//@����:�жϲ˵����Ƿ��ڲ˵������Ӳ˵��д��ڡ�
	//@����: hMenu, ��ʼ�����Ĳ˵����
	//       uID, �˵���ID
	HMENU  GetMenuHandle(HMENU hSearchMenu, UINT uID)
	{

		int nCount = GetMenuItemCount(hSearchMenu);
		BOOL bFound = FALSE;
		for(int nPos = 0; nPos < nCount; nPos ++)
		{
			if(GetMenuItemID(hSearchMenu, nPos) == uID)
			{
				return hSearchMenu;				
			}
			else 
			{
				HMENU hSubmenu = GetSubMenu(hSearchMenu, nPos);

				if(hSubmenu)
				{

					HMENU hMenuFound = GetMenuHandle(hSubmenu, uID);
					if(hMenuFound != NULL)
					{
						return hMenuFound;
					}
					
				}


			}

		}//for_each(menu item)


		return NULL;
	}




protected:
	HMENU m_hMenu;
	HDC   m_hMemDC;


};