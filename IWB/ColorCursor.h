#pragma once
//����:��ɫ�������
class CColorCursor
{
public:
    CColorCursor()
        :
        m_hCursor(NULL)
        , m_hAndMask(NULL)
        , m_hBmColor(NULL)
    {

    }

    virtual ~CColorCursor()
    {
        Destroy();
    }


    void Destroy()
    {

        if (this->m_hCursor)
        {
            ::DestroyIcon(this->m_hCursor);
            m_hCursor = NULL;
        }

        if (m_hAndMask)
        {
            ::DeleteObject(m_hAndMask);
            m_hAndMask = NULL;
        }

        if (m_hBmColor)
        {
            ::DeleteObject(m_hBmColor);
            m_hBmColor = NULL;
        }
    }



    //@����:����ʵ�ľ��ι��
    //@����:xHotSpot, ����ȵ�ˮƽ����
    //      yHotSpot, ����ȵ㴹ֱ���� 
    //      nCxCurosr,���ߴ���
    //      nCyCursor,���ߴ�߶�
    //     
    //      rcSize, ���ε�λ��
    //      clrFill,���������ɫ
    //      clrBorder, ���αߵ���ɫ
    //      nOverlapCursorResID, ��Ҫ���ӵĹ����Դ��ID.
    BOOL CreateSolidRectangleCursor(
         //int  xHotspot,
         //int  yHotspot,
         int nCxCursor,
         int nCyCursor,
         //RECT& rcSize,
         COLORREF  clrFill,//���������ɫ
         COLORREF  clrBorder,//���αߵ���ɫ
        UINT  nOverlapCursorResID
        )
    {

        BOOL bRet = FALSE;


         int  xHotspot = nCxCursor/2;
         int  yHotspot = nCyCursor/2;


        HDC hDC					= ::GetDC(NULL);
        HDC hMainDC				= ::CreateCompatibleDC(hDC); 

        HBITMAP hSrcBitmap      = ::CreateCompatibleBitmap(hDC, nCxCursor, nCyCursor);
        HBITMAP hBmpOld         = (HBITMAP)SelectObject(hMainDC, hSrcBitmap);

        //COLORREF clrBackground  = RGB(0,128,128);
        COLORREF clrTransparent  = RGB(0, 128, 128);

        HBRUSH   hBrushTransparent = ::CreateSolidBrush(clrTransparent);
    

        //��䱳��
        RECT rcArea;
        rcArea.left   = 0;
        rcArea.top    = 0;
        rcArea.right  = nCxCursor;
        rcArea.bottom = nCyCursor;
        FillRect(hMainDC, &rcArea, hBrushTransparent);


        //���ƾ���
        HPEN     hPen           = ::CreatePen(PS_SOLID, 1, clrBorder);
        HPEN     hPenOld        = (HPEN)SelectObject(hMainDC, hPen);
        HBRUSH   hBrushFill     = ::CreateSolidBrush(clrFill);
        HBRUSH   hBrushOld = (HBRUSH)SelectObject(hMainDC, hBrushFill);

        //Rectangle(hMainDC, rcArea.left,  rcArea.top, rcArea.right, rcArea.bottom);

        //�ߴ����
        long width  = rcArea.right  - rcArea.left;
        long height = rcArea.bottom - rcArea.top;
        long left   = width >> 2;
        long right  = left + (width >> 1);
        long top    = height >> 2;
        long bottom = top + (height >> 1);
        Rectangle(hMainDC, left, top, right, bottom);


        DeleteObject(hBrushTransparent);
        DeleteObject(hBrushFill);

        SelectObject(hMainDC, hBrushOld);
        SelectObject(hMainDC, hPenOld);
        SelectObject(hMainDC, hBmpOld);

        DeleteDC(hMainDC);
        ReleaseDC(NULL,hDC);


        do
        {
            GetMaskBitmaps(hSrcBitmap, clrTransparent);

            //Create the cursor using the masks and the hotspot values provided
            ICONINFO iconinfo = {0};
            iconinfo.fIcon		= FALSE;
            iconinfo.xHotspot	= xHotspot;
            iconinfo.yHotspot	= yHotspot;
            iconinfo.hbmMask	= m_hAndMask;
            iconinfo.hbmColor	= m_hBmColor;

            m_hCursor = ::CreateIconIndirect(&iconinfo);

        }while(0);


        return TRUE;

    }


     HCURSOR CreateCursorFromBitmap(HBITMAP hSrcBitmap,COLORREF clrTransparent,
                                          DWORD   xHotspot,DWORD   yHotspot)
     {
            GetMaskBitmaps(hSrcBitmap, clrTransparent);
             if (m_hAndMask == NULL || m_hBmColor == NULL)
                    return NULL;


            //Create the cursor using the masks and the hotspot values provided
            ICONINFO iconinfo = {0};
            iconinfo.fIcon		= FALSE;
            iconinfo.xHotspot	= xHotspot;
            iconinfo.yHotspot	= yHotspot;
            iconinfo.hbmMask	= m_hAndMask;
            iconinfo.hbmColor	= m_hBmColor;

            m_hCursor = ::CreateIconIndirect(&iconinfo);

            return m_hCursor;

     }


    operator HCURSOR ()
    {	
        return m_hCursor;
    }



protected:
    
    void GetMaskBitmaps(HBITMAP hSourceBitmap,COLORREF clrTransparent)
    {
        HDC hDC					= ::GetDC(NULL);
        HDC hMainDC				= ::CreateCompatibleDC(hDC); 
        HDC hAndMaskDC			= ::CreateCompatibleDC(hDC); 
        HDC hBmColorDC			= ::CreateCompatibleDC(hDC); 

        //Get the dimensions of the source bitmap
        BITMAP bm;
        ::GetObject(hSourceBitmap,sizeof(BITMAP),&bm);

        
        m_hAndMask = ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);
        m_hBmColor = ::CreateCompatibleBitmap(hDC,bm.bmWidth,bm.bmHeight);

        if (m_hAndMask == NULL || m_hBmColor == NULL) return;

        //Select the bitmaps to DC
        HBITMAP hOldMainBitmap    = (HBITMAP)::SelectObject(hMainDC,hSourceBitmap);
        HBITMAP hOldAndMaskBitmap = (HBITMAP)::SelectObject(hAndMaskDC, m_hAndMask);
        HBITMAP hOldBmColorBitmap = (HBITMAP)::SelectObject(hBmColorDC, m_hBmColor);

        //Scan each pixel of the souce bitmap and create the masks
        COLORREF dwSrcPixel;
        int nTransparentCount = 0;
        for(int x=0;x<bm.bmWidth;++x)
        {
            for(int y=0;y<bm.bmHeight;++y)
            {
                dwSrcPixel = ::GetPixel(hMainDC,x,y);
                if(dwSrcPixel == clrTransparent)
                {
                    ::SetPixel(hAndMaskDC,x,y,RGB(255,255,255));
                    ::SetPixel(hBmColorDC,x,y,RGB(0,0,0));
                    nTransparentCount++;
                 }
                else
                {
                    ::SetPixel(hAndMaskDC,x,y,RGB(0,0,0));
                    ::SetPixel(hBmColorDC,x,y, dwSrcPixel);
                }
            }
        }


        if (nTransparentCount == 0)
        {//��ʹ�ö���Ļ�ں����ʱ����
         //���icon�ĳߴ�32*32����icon��AndMask��������һ������Ϊ0xFFFFFFFF�������ܹ�������ʾ��
         //icon�ĳߴ�Ϊ16*16����24*24ʱ���������ܹ�������ʾ����AndMask�е�ֵȫΪ0x0000000, 
         //����Ҳ���ܹ�������ʾ��
            ::SetPixel(hAndMaskDC, bm.bmWidth >> 1, bm.bmHeight >> 1, RGB(0xFF, 0xFF, 0xFF));
            ::SetPixel(hBmColorDC, bm.bmWidth >> 1, bm.bmHeight >> 1, RGB(0, 0, 0));
        }

        ::SelectObject(hMainDC,hOldMainBitmap);
        ::SelectObject(hAndMaskDC,hOldAndMaskBitmap);
        ::SelectObject(hBmColorDC, m_hBmColor);

        
        ::DeleteDC(hAndMaskDC);
        ::DeleteDC(hBmColorDC);
        ::DeleteDC(hMainDC);

        ::ReleaseDC(NULL,hDC);
    }


protected: 
    HCURSOR m_hCursor;
    HBITMAP  m_hAndMask;
    HBITMAP  m_hBmColor;

};