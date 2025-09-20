#include "AlgoLib.h"
#include <vector>
//added by toxuke@gmail.com, 2013/01/07
//У��ͼ����
//���β�Ȧ�ṹ
struct TRectLayer
{
    int m_nHorzMarkNumber ;//ˮƽ����У������Ŀ
    int m_nVertMarkNumber ;//��ֱ����У������Ŀ
    std::vector<int> m_vecScreenMarkNo;//Ȧ������ĻУ�������������ɵ����顣
};



//@����:
//
inline int  Pattern2ColNum(E_AutoCalibratePattern ePattern)
{
    int nColNum = 11;
    switch(ePattern)
    {
    case E_CALIBRATE_PATTERN_9_COLS:
        nColNum = 9;
        break;

    case E_CALIBRATE_PATTERN_10_COLS:
        nColNum = 10;
        break;

    case E_CALIBRATE_PATTERN_11_COLS:
        nColNum = 11;
        break;

    case E_CALIBRATE_PATTERN_12_COLS:
        nColNum = 12;
        break;

    case E_CALIBRATE_PATTERN_13_COLS:
        nColNum = 13;
        break;

    case E_CALIBRATE_PATTERN_14_COLS:
        nColNum = 14;
        break;

    case E_CALIBRATE_PATTERN_15_COLS:
        nColNum = 15;
        break;

    case E_CALIBRATE_PATTERN_16_COLS:
        nColNum = 16;
        break;

    case E_CALIBRATE_PATTERN_17_COLS:
        nColNum = 17;
        break;
    }//switch

    return nColNum;
}


//�������ɲ�����У��ͼ��, 
class CCalibratePatternBase
{
public:
    CCalibratePatternBase()
    {

    }

    virtual ~CCalibratePatternBase()
    {

    }

    //@����:��ʼ��У��ͼ��
    //@����:szPattern, У��ͼ���ĳߴ�
    //      eGridNum, У��ͼ���о��ο������
    //virtual void InitPattern(E_AutoCalibratePattern ePattern, SIZE szPattern) = 0;


    //virtual void DrawPattern(HWND hWnd) = 0;

};


//У��ͼ���и��о��ο�ĸ߶ȴ�С����Ȳ����й�ϵ
class CArithSeriesRowPattern: public CCalibratePatternBase
{
public:
    CArithSeriesRowPattern()
        :
    m_nRowCount(0),
        m_nColCount(0),
        m_nCalibrateMarkCount(0),
        m_nUpperPartMarkCount(0),
        m_nUpperGridCount(0),
        m_nUpperGridRow(0),
        m_nLowerPartMarkCount(0),
        m_nLowerGridCount(0),
        m_nLowerGridRow(0)
    {

        memset(&m_UpperArea, 0, sizeof(m_UpperArea));
        memset(&m_LowerArea, 0, sizeof(m_LowerArea));

    }

    virtual ~CArithSeriesRowPattern()
    {

        DeleteAllGrayBrush();
    }

    //@����:��ʼ��У��ͼ��
    //@����:
    //      ePattern, У��ͼ������ö����
    //      rcMonitor, ��ʾ���ĳߴ�
    //      dbRowHeigthRatio, ��һ���иߺ����һ���и�֮��
    //@˵��:Ӧ�ø���У��ͼ���ϰ벿��������°벿�����֮��ȷ��dbRowHeigthRatio����
    //      ��һ�е����һ�еĸ����и�H1, H2,...Hn���һ���Ȳ����С�
    //
    //��������:
    //         dbRowHeightRation = 0.31293
    //         row = 17;
    //         col = 12
    virtual void InitPattern(E_AutoCalibratePattern ePattern, const RECT& rcMonitor, double dbRowHeigthRatio = 1.0)
    {
        m_patternSize.cx = rcMonitor.right  - rcMonitor.left;
        m_patternSize.cy = rcMonitor.bottom - rcMonitor.top;




        //�޶�����ȷ�Χ
        if(dbRowHeigthRatio > 3.0) dbRowHeigthRatio = 3.0;

        if(dbRowHeigthRatio < 1.0/3.0) dbRowHeigthRatio = 1.0/3.0;


        //========����У�����ο������������
        //ָ������
        m_nColCount = Pattern2ColNum(ePattern);

        //�����ɼ���ó�

        //�������������п�
        int nColWidth = m_patternSize.cx / m_nColCount;


        double dbFirstRowH   = 0.0; // ��һ���и�
        double dbLastRowH    = 0.0; // �ڶ����и�
        double dbDiff        = 0.0; //�Ȳ����еĹ���       


        //��һ���и����
        if(dbRowHeigthRatio > 1.0)
        {
            //��Ϊ�����иߵ����п��1.3��
            dbFirstRowH = (double)nColWidth*1.5;

            //��������
            m_nRowCount = int(2.0*(double)m_patternSize.cy/((1.0 + 1.0/dbRowHeigthRatio)*dbFirstRowH) + .5);

            //���һ���и�
            dbLastRowH  = dbFirstRowH/dbRowHeigthRatio;
        }
        else//���һ���и����
        {
            //��Ϊ�����иߵ����п�1.3��
            dbLastRowH = (double)nColWidth*2;

            //��������
            m_nRowCount = int(2.0*(double)m_patternSize.cy/((1.0 + dbRowHeigthRatio)*dbLastRowH) + .5);

            //��һ���и�
            dbFirstRowH = dbLastRowH*dbRowHeigthRatio;

        }

        dbDiff = (dbLastRowH - dbFirstRowH)/double(m_nRowCount -1);



        //У���������
        m_nCalibrateMarkCount = (m_nRowCount - 1) * (m_nColCount - 1);


        //����ÿ��У�����ο��λ��
        CalcGridCoord(
            rcMonitor,
            m_nRowCount, 
            m_nColCount,
            nColWidth,
            nColWidth,
            dbFirstRowH,
            dbDiff);


        //�������²�������
        SplitArea(
            rcMonitor, 
            m_nRowCount, 
            m_nColCount,
            dbFirstRowH, 
            dbDiff);


        //����У��������
        UpdateMarkCoord();



        //�ռ�Ȧ����Ϣ
        CollectLayerMarkInfo();



        //InitGrayBrushes(0xA9, 0xFF);
        //InitGrayBrushes(0x80, 0xFF);
        InitGrayBrushes(0xFF, 0xFF);
    }



    //@����:����������У��ͼ��
    virtual void DrawPattern(HWND hWnd)
    {
        //do nothing
        if(hWnd == NULL) return;


        HDC hdc = ::GetDC(hWnd);

        //HBRUSH hRedBrush = ::CreateSolidBrush(RGB(255,0,0));//��ɫ��ˢ
        //modify by zwn �ǰ�255�Ĵ���ɫ�޸�Ϊ��ɫ
        //HBRUSH hWhiteBrush = ::CreateSolidBrush(RGB(255, 255, 255));//��ɫ��ˢ
        HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0, 0 ));//��ɫ��ˢ
        //      HBRUSH hWhite1Brush = ::CreateSolidBrush(RGB(211, 211, 211)); //����
        //      HBRUSH hWhite2Brush = ::CreateSolidBrush(RGB(192, 192, 192)); //ǳ��
        //      HBRUSH hWhite3Brush = ::CreateSolidBrush(RGB(169, 169, 169)); //���


        //HBRUSH brushes[2];
        //brushes[0] = hRedBrush;
        //brushes[0] = hWhiteBrush;
        // brushes[1] = hBlackBrush;


        int nRowStartBrushIndex = 0;
        int nColBrushIndex = 0;
        int iGridIndex = 0;

        for(int row=0; row < this->m_nRowCount; row++)
        {
            int row_center = row - m_nRowCount/2;

            for(int col=0; col < this->m_nColCount; col++)
            {

                int col_center = col - m_nColCount/2;

                if(nColBrushIndex == 0)//�ǰ�ɫ��ˢ,���վ������ĵľ���׼���ҶȻ�ˢ
                {
                    double R2 = sqrt((double)(row_center*row_center  + col_center*col_center));
                    UINT  nGrayBrushIndex = int(R2);
                    if(nGrayBrushIndex >= m_vecGrayBrushes.size())
                    {
                        nGrayBrushIndex = m_vecGrayBrushes.size() - 1;
                    }


                    FillRect(hdc, &this->m_vecGrids[iGridIndex], m_vecGrayBrushes[nGrayBrushIndex]);
                }
                else
                {


                    FillRect(hdc, &this->m_vecGrids[iGridIndex], hBlackBrush);
                }

                nColBrushIndex = (nColBrushIndex + 1) & 0x01;

                iGridIndex ++;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row



        //::DeleteObject(hRedBrush);
        //::DeleteObject(hWhiteBrush);
        ::DeleteObject(hBlackBrush);

        ReleaseDC(hWnd, hdc);

    }



    /*
    //@����:�����ϰ벿У��ͼ��
    void DrawUpperPartPattern(HWnd hWnd)
    {
    //do nothing if no window specified
    if(hWnd == NULL) return;

    HDC hDC = ::GetDC(hWnd);


    HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//��ɫ��ˢ



    //�°벿��ɫ���
    RECT rc = m_LowerArea;
    ScreenToClient(hWnd, &rc.left);
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);

    //���һ�е��б��=this->m_nRowCount - 1, �����кŵ���żѡ��ˢ����, ż���ѡ��ҶȻ�ˢ, ����ѡ���ɫ��ˢ
    int nRowStartBrushIndex = (this->m_nRowCount - 1) % 2;


    int nColBrushIndex = nRowStartBrushIndex;
    int iGridIndex = 0;
    for(int row=0; row < this->m_nUpperGridRow; row++)
    {
    int offset_row = row - m_nRowCount/2;

    for(int col=0; col < this->m_nColCount; col++)
    {
    int offset_col = col - m_nColCount/2;

    if(0 == nColBrushIndex)
    {
    double R2 = sqrt((double)(offset_row*offset_row  + offset_col*offset_col));

    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, m_vecGrayBrushes[int(R2)]);

    }
    else
    {
    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);
    }
    nColBrushIndex = (nColBrushIndex + 1) &0x01 ;//%2

    iGridIndex ++;

    }//for-each col

    nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
    nColBrushIndex = nRowStartBrushIndex;

    }//for-each row

    ::DeleteObject(hBlackBrush);
    ::ReleaseDC(hWnd, hDC);
    }





    //@����:�����°벿ͼ��
    void DrawLowerPartPattern(HWND hWnd)
    {
    //do nothing if no specified 
    if(hWnd == NULL) return;

    HDC hDC = ::GetDC(hWnd);

    HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//��ɫ��ˢ


    //�ϰ벿��ɫ���
    RECT rc = m_UpperArea;
    ScreenToClient(hWnd, &rc.left);
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);

    int nRowStartBrushIndex = 0;
    int nColBrushIndex = 0;
    int iGridIndex = m_nRowCount*m_nColCount - 1;


    for(int row=m_nRowCount - 1; row > (this->m_nRowCount - 1 - m_nLowerGridRow); row --)
    {
    int offset_row = row - m_nRowCount/2;

    for(int col=0; col < this->m_nColCount; col++)
    {
    int offset_col = col - m_nColCount/2;

    if(0 == nColBrushIndex)
    {
    double R2 = sqrt((double)(offset_row*offset_row  + offset_col*offset_col));

    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right); 
    FillRect(hDC, &rc, m_vecGrayBrushes[int(R2)]);

    }
    else
    {
    rc = this->m_vecGrids[iGridIndex];
    ScreenToClient(hWnd, &rc.left );
    ScreenToClient(hWnd, &rc.right);
    FillRect(hDC, &rc, hBlackBrush);
    }

    nColBrushIndex = (nColBrushIndex + 1) & 0x01;//%2

    iGridIndex --;

    }//for-each col

    nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
    nColBrushIndex = nRowStartBrushIndex;

    }//for-each row

    ::DeleteObject(hBlackBrush);
    ::ReleaseDC(hWnd, hDC);
    }*/


    //@����:�����ϰ벿У��ͼ��
    //@����:hWnd, ��ʾͼ���Ĵ���
    //      clrHilight, ��������ɫ
    void DrawUpperPartPattern(HWND hWnd, COLORREF clrHilight)
    {

        if(hWnd == NULL) return;

        HDC hDC = GetDC(hWnd);

        HBRUSH hBlackBrush   = ::CreateSolidBrush(RGB(  0,   0,  0 ));//��ɫ��ˢ

        HBRUSH hHilightBrush = ::CreateSolidBrush(clrHilight);//������ˢ


        //�°벿��ɫ���
        RECT rc = m_LowerArea;

       //ӳ����Ļ���굽У�����������
        MapWindowPoints(
            NULL,                     //handle to the source window, desktop
            hWnd,       //handle to destination window
            (LPPOINT)&rc,             //array of points to map 
            sizeof(RECT)/sizeof(POINT)//number of points in array
            );


        FillRect(hDC, &rc, hBlackBrush);

        //���һ�е��б��=this->m_nRowCount - 1, �����кŵ���żѡ��ˢ����, ż���ѡ��ҶȻ�ˢ, ����ѡ���ɫ��ˢ
        int nRowStartBrushIndex = (this->m_nRowCount - 1) % 2;

        int nColBrushIndex = nRowStartBrushIndex;
        int iGridIndex = 0;
        for(int row=0; row < this->m_nUpperGridRow; row++)
        {


            for(int col=0; col < this->m_nColCount; col++)
            {
                rc = this->m_vecGrids[iGridIndex];
               //ӳ����Ļ���굽У�����������
                MapWindowPoints(
                    NULL,                     //handle to the source window, desktop
                    hWnd,       //handle to destination window
                    (LPPOINT)&rc,             //array of points to map 
                    sizeof(RECT)/sizeof(POINT)//number of points in array
                    );

                if(0 == nColBrushIndex)
                {
                    FillRect(hDC, &rc, hHilightBrush);

                }
                else
                {
                    FillRect(hDC, &rc, hBlackBrush);
                }
                nColBrushIndex = (nColBrushIndex + 1) &0x01 ;//%2

                iGridIndex ++;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row

        ::DeleteObject(hBlackBrush);
        ::DeleteObject(hHilightBrush);

    }





    //@����:�����°벿ͼ��
    //@����:hWnd, ��ʾͼ���Ĵ���
    //      clrHilight, ��������ɫ
    void DrawLowerPartPattern(HWND hWnd, COLORREF clrHilight)
    {
        //do nothing if no window specified
        if(hWnd == NULL) return;

        HDC hDC = ::GetDC(hWnd);

        HBRUSH hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,  0 ));//��ɫ��ˢ
        HBRUSH hHilightBrush = ::CreateSolidBrush(clrHilight);//������ˢ


        //�ϰ벿��ɫ���
        RECT rc = m_UpperArea;
       //ӳ����Ļ���굽У�����������
        MapWindowPoints(
            NULL,                     //handle to the source window, desktop
            hWnd,       //handle to destination window
            (LPPOINT)&rc,             //array of points to map 
            sizeof(RECT)/sizeof(POINT)//number of points in array
            );
        FillRect(hDC, &rc, hBlackBrush);
        FillRect(hDC, &rc, hBlackBrush);

        int nRowStartBrushIndex = 0;
        int nColBrushIndex = 0;
        int iGridIndex = m_nRowCount*m_nColCount - 1;


        for(int row=m_nRowCount - 1; row > (this->m_nRowCount - 1 - m_nLowerGridRow); row --)
        {
            int offset_row = row - m_nRowCount/2;

            for(int col=0; col < this->m_nColCount; col++)
            {
                int offset_col = col - m_nColCount/2;

                rc = this->m_vecGrids[iGridIndex];
               //ӳ����Ļ���굽У�����������
                MapWindowPoints(
                    NULL,                     //handle to the source window, desktop
                    hWnd,       //handle to destination window
                    (LPPOINT)&rc,             //array of points to map 
                    sizeof(RECT)/sizeof(POINT)//number of points in array
                    );

                if(0 == nColBrushIndex)
                {
                    FillRect(hDC, &rc, hHilightBrush);

                }
                else
                {
                    FillRect(hDC, &rc, hBlackBrush);
                }

                nColBrushIndex = (nColBrushIndex + 1) & 0x01;//%2

                iGridIndex --;

            }//for-each col

            nRowStartBrushIndex = (nRowStartBrushIndex + 1) & 0x01;//%2
            nColBrushIndex = nRowStartBrushIndex;

        }//for-each row

        ::DeleteObject(hHilightBrush);
        ::DeleteObject(hBlackBrush);
        //::ReleaseDC(hWnd, hdc);
    }




    //@����:��ȡУ��ͼ���о��ο������
    int GetRowCount()const
    {
        return m_nRowCount;
    }

    //@����:��ȡУ��ͼ���о��ο������
    int GetColCount()const
    {
        return m_nColCount;
    }

    //@����:����У���������
    int GetCalibrateMarkCount()const
    {
        return m_nCalibrateMarkCount;
    }

    //@����:��ȡ�ϰ벿У�������Ŀ
    int GetUpperPartMarkCount()const
    {
        return m_nUpperPartMarkCount;
    }

    //@����:��ȡ�ϰ벿���ο����Ŀ
    int GetUpperGridCount() const
    {
        return m_nUpperPartMarkCount;
    }

    //@����:��ȡ�°벿У�������Ŀ
    int GetLowerPartMarkCount()const
    {
        return m_nLowerPartMarkCount;
    }

    //@����:��ȡ�°벿���ο����Ŀ
    int GetLowerGridCount() const
    {
        return m_nLowerPartMarkCount;
    }

    const RECT& GetUpperArea()const
    {
        return m_UpperArea;
    }


    const RECT& GetLowerArea()const
    {
        return m_LowerArea;
    }

    const POINT& GetMarkCoord(int i)const
    {
        assert( i>= 0 && i< this->m_nCalibrateMarkCount);

        return this->m_vecMarkerCoord[i];
    }



    //@����:�ж��Ƿ������һ�е�У����
    const BOOL IsMarkerOfLastRow(int nIndex)
    {
        if( nIndex>= 0 && nIndex< this->m_nCalibrateMarkCount)
        {

            int nFirstIndex =  (m_nRowCount - 2) * (m_nColCount -1);
            int nLastIndex  =  (m_nRowCount - 1) * (m_nColCount -1) - 1;

            if(nFirstIndex <= nIndex && nIndex <= nLastIndex)
            {
                return TRUE;
            }

        }
        return FALSE;
    }




    //@����:����Ȧ�����Ŀ
    const int GetLayerCount()const
    {
        return m_vecLayers.size();
    }


    //@����:����ָ����Ȧ����Ϣ
    const TRectLayer&  GetLayer(int nLayerIndex)const
    {
        assert(nLayerIndex >=0 && nLayerIndex < (int)m_vecLayers.size());

        return m_vecLayers[nLayerIndex];
    }


    //@����:��ʾ����ȦУ�����������
    //@����:hWnd, ��ʾ�������Ĵ�����
    //      nOutermostMarkerIndex, ����ȦУ����������Ȧ�е����
    void PrintOutermostMask(HWND hWnd, int nOutermostLayerMarkerIndex)
    {
        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return;

        int nMarkGlobalIndex = GetOutmostLayerMarkGlobalIndex(nOutermostLayerMarkerIndex);


        //����У����������ĸ�Grid��������
        int nMarkerEachRow = this->m_nColCount - 1;

        int nLeftTopGridIndex     = nMarkGlobalIndex/nMarkerEachRow * this->m_nColCount + (nMarkGlobalIndex % nMarkerEachRow);
        int nRightBottomGridIndex = nLeftTopGridIndex  + this->m_nColCount + 1;

        HDC hDC = ::GetDC(hWnd);


        //����Ļˢ��ȫ��
        RECT rcScreen;
        rcScreen.left   = 0;
        rcScreen.top    = 0;
        rcScreen.right  = m_patternSize.cx;
        rcScreen.bottom = m_patternSize.cy;
        FillRect(hDC, &rcScreen, m_hBlackBrush);


        //��������ˢ�ɰ�ɫ
        RECT rcGrid;

        rcGrid.left   = m_vecGrids[nLeftTopGridIndex].left;
        rcGrid.top    = m_vecGrids[nLeftTopGridIndex].top;

        rcGrid.right  = m_vecGrids[nRightBottomGridIndex].right;
        rcGrid.bottom = m_vecGrids[nRightBottomGridIndex].bottom;


        FillRect(hDC, &rcGrid, m_hWhiteBrush);

        ReleaseDC(hWnd, hDC);

    }



    //@����:������Ȧ��һ��У��ͼ��
    //@����:hWnd, ��ʾУ��ͼ���Ĵ�����
    //      nMarkerIndex, У����������
    void PintOutermostMarker(HWND hWnd, int nOutermostLayerMarkerIndex)
    {
        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return;

        int nMarkGlobalIndex = GetOutmostLayerMarkGlobalIndex(nOutermostLayerMarkerIndex);


        //����У����������ĸ�Grid��������
        int nMarkerEachRow = this->m_nColCount - 1;

        int nLeftTopGridIndex     = nMarkGlobalIndex/nMarkerEachRow * this->m_nColCount + (nMarkGlobalIndex % nMarkerEachRow);
        int nRightTopGridIndex    = nLeftTopGridIndex  + 1;
        int nRightBottomGridIndex = nLeftTopGridIndex  + this->m_nColCount + 1;
        int nLeftBottomGridIndex  = nLeftTopGridIndex  + this->m_nColCount;






        HDC hDC = ::GetDC(hWnd);

        RECT rcGrid;

        //A B
        //����
        //����
        //D  C
        rcGrid = m_vecGrids[nLeftTopGridIndex];

        //���ư׿�A
        FillRect(hDC, &rcGrid, m_hWhiteBrush);


        //���ƺڿ�B
        rcGrid = m_vecGrids[nRightTopGridIndex];
        FillRect(hDC, &rcGrid, m_hBlackBrush);

        //���ư׿�C
        rcGrid = m_vecGrids[nRightBottomGridIndex];
        FillRect(hDC, &rcGrid, m_hWhiteBrush);

        //���ƺڿ�D
        rcGrid = m_vecGrids[nLeftBottomGridIndex];
        FillRect(hDC, &rcGrid, m_hBlackBrush);


        ReleaseDC(hWnd, hDC);

    }



    //@����:��ȡ����ȦУ�������Ŀ
    int GetOutermostMarkerCount()
    {
        int nOutermostMarkerCount = 0;

        if(this->m_nRowCount == 2)
        {
            nOutermostMarkerCount = this->m_nColCount - 1;

        }
        else
        {
            nOutermostMarkerCount = (this->m_nColCount - 1) * 2 + (this->m_nRowCount - 3) * 2;
        }

        return nOutermostMarkerCount;
    }


    //@����:�������У��У�����ڲ��еı��ת��Ϊȫ�ֱ��
    //     ��[0]                      ��[m_nColCount - 2]
    //     ��  ��  ��  ��  ��  ��     ��
    //     ��                         ��
    //     ��                         ��
    //     .                           .
    //     .                          .
    //     ��  ��  ��  ��  ��  ��     ���[m_nColCount - 21] + [m_nRowCount - 2]
    //     ��
    //     [m_nColCount - 2] + [m_nRowCount - 2] + [m_nColCount - 2] + 
    int GetOutmostLayerMarkGlobalIndex(int nOutermostLayerMarkerIndex)
    {

        if(nOutermostLayerMarkerIndex >= GetOutermostMarkerCount()) return - 1;

        int nMarkGlobalIndex  = nOutermostLayerMarkerIndex;
        int nMarkCountEachRow = m_nColCount - 1;
        int nMarkCountEachCol = m_nRowCount - 1;

        int nOutermostLayerTopRightOutMarkIndex  = nMarkCountEachRow     - 1;
        int nOutermostLayerBottomRightMarkIndex  = nOutermostLayerTopRightOutMarkIndex + nMarkCountEachCol - 1;
        int  nOutermostLayerBottomLeftMarkIndex   = nOutermostLayerBottomRightMarkIndex + nMarkCountEachRow - 1;

        //��"����ȦУ�������"ת��ΪΪȫ��У������
        if(nOutermostLayerMarkerIndex <= nOutermostLayerTopRightOutMarkIndex)
        {
            nMarkGlobalIndex = nOutermostLayerMarkerIndex;
        }
        else if(nOutermostLayerMarkerIndex <=  nOutermostLayerBottomRightMarkIndex)
        {
            nMarkGlobalIndex = nOutermostLayerTopRightOutMarkIndex + (nOutermostLayerMarkerIndex - nOutermostLayerTopRightOutMarkIndex)*nMarkCountEachRow;
        }
        else if(nOutermostLayerMarkerIndex <= nOutermostLayerBottomLeftMarkIndex)
        {
            nMarkGlobalIndex = nMarkCountEachRow*(m_nRowCount - 2);
            nMarkGlobalIndex += nOutermostLayerBottomLeftMarkIndex  -  nOutermostLayerMarkerIndex;
        }
        else
        {
            nMarkGlobalIndex = (nMarkCountEachCol - 1 - (nOutermostLayerMarkerIndex - nOutermostLayerBottomLeftMarkIndex))*nMarkCountEachRow;
        }

        return nMarkGlobalIndex;

    }






protected:
    //member function
    //@����:����У��ͼ����ÿ��С�ľ��ο��λ������
    void CalcGridCoord(
        const RECT& rcPatternArea      ,//У��ͼ��������ߴ�
        int         nRows              ,//У��ͼ���о��ο������
        int         nCols              ,//У��ͼ���о��ο������
        int         nLeftMostGridWidth ,//����ͼ������ߵ�С���ο�Ŀ��
        int         nRightMostGridWidth,//����ͼ�����ұߵ�С���ο�Ŀ��
        double      dbFirstRowHeight   ,//��һ���и�
        double      dbDiff              //�Ȳ����й���
        )
    {

        //
        int nAreaWidth  = rcPatternArea.right - rcPatternArea.left;
        int nAreaHeight = rcPatternArea.bottom - rcPatternArea.top;


        //��ȥ�������߾��ο��, �����м��еľ��ο�Ŀ��
        int nMiddelGridWidth = (nAreaWidth - nLeftMostGridWidth - nRightMostGridWidth)/(nCols - 2);
        int nMiddleGridWidthFractionInc = (nAreaWidth - nLeftMostGridWidth - nRightMostGridWidth )%(nCols - 2);

        m_vecGrids.resize(nRows * nCols);

        RECT    rcGrid;
        double dbRowHeight = 0.0;

        int nGridIndex = 0;

        for(int r = 0; r < nRows; r++)
        {
            //���㴹ֱ����
            //��һ��
            if(r == 0)
            {
                dbRowHeight   = dbFirstRowHeight;
                rcGrid.top    = rcPatternArea.top;
            }
            else
            {
                dbRowHeight += dbDiff; //�иߵȲ�仯

                rcGrid.top    = rcGrid.bottom;
            }
            rcGrid.bottom =  rcGrid.top + (int)(dbRowHeight + .5);

            int nMiddleGridWidthFractionCounter = 0;//���ο��ȵ�С�������ۼ���

            //����ˮƽ����
            for(int c = 0 ; c < nCols; c++)
            {
                //��һ��
                if(0== c)//��һ��
                {
                    rcGrid.left = rcPatternArea.left;
                    rcGrid.right = rcPatternArea.left + nLeftMostGridWidth;
                }
                else if(nCols - 1 == c)//���һ��
                {  
                    rcGrid.left  = rcGrid.right;
                    rcGrid.right = rcGrid.left + nRightMostGridWidth;
                }
                else //�м���
                {
                    rcGrid.left  = rcGrid.right;
                    rcGrid.right = rcGrid.left + nMiddelGridWidth;

                    nMiddleGridWidthFractionCounter += nMiddleGridWidthFractionInc;
                    if(nMiddleGridWidthFractionCounter >= nCols - 2)
                    {
                        nMiddleGridWidthFractionCounter -= (nCols - 2);
                        rcGrid.right ++;
                    }

                }

                m_vecGrids[nGridIndex++] = rcGrid;
            }//for each column
        }//for each row




    }


    //@����:ȷ��������������������
    //@����:
    //@˵��:�����򻮷�Ϊ����������Լ��ȵ����
    void SplitArea(
        const RECT& rcPatternArea      ,//У��ͼ��������ߴ�)
        int         nRows              ,//У��ͼ���о��ο������
        int         nCols              ,//У��ͼ���о��ο������
        double      dbFirstRowHeight   ,//��һ���и�
        double      dbDiff              //�Ȳ����й���
        )

    {
        int nAreaWidth  = rcPatternArea.right  - rcPatternArea.left;
        int nAreaHeight = rcPatternArea.bottom - rcPatternArea.top;


        RECT    rcGrid;
        double dbRowHeight = 0.0;

        for(int r = 0; r < nRows; r++)
        {
            //���㴹ֱ����
            //��һ��
            if(r == 0)
            {
                dbRowHeight   = dbFirstRowHeight;
                rcGrid.top    = rcPatternArea.top;
            }
            else
            {
                dbRowHeight += dbDiff; //�иߵȲ�仯

                rcGrid.top    = rcGrid.bottom;
            }
            rcGrid.bottom =  rcGrid.top + (int)(dbRowHeight + .5);

            //�������£��ױߴ��ڵ���һ��У��ͼ���߶ȵ����̸�Ϊ�ϲ�����������һ�С�
            if(rcGrid.bottom >= nAreaHeight/2)
            {
                m_UpperArea.left   = rcPatternArea.left;
                m_UpperArea.right  = rcPatternArea.right;
                m_UpperArea.top    = rcPatternArea.top;
                m_UpperArea.bottom = rcGrid.bottom;

                //�ϲ���У�������
                m_nUpperPartMarkCount = r * (nCols - 1);

                //�ϲ��־��ο����
                m_nUpperGridCount = (r+ 1) * nCols;

                //�ϲ��־��ο�����
                m_nUpperGridRow = r+1;


                m_LowerArea.left   = rcPatternArea.left;
                m_LowerArea.right  = rcPatternArea.right;
                m_LowerArea.top    = rcGrid.top;
                m_LowerArea.bottom = rcPatternArea.bottom;

                //�²���У�������
                m_nLowerPartMarkCount = ((nRows -1) - r ) * (nCols - 1);


                //�²��־��ο����
                m_nLowerGridCount = (nRows - r ) * nCols;


                //�²��־��ο�����
                m_nLowerGridRow = nRows - r;


                break;
            }

        }//


    }



    //@����:����У��ͼ����ÿ��У���������
    void UpdateMarkCoord()
    {
        m_vecMarkerCoord.resize(m_nCalibrateMarkCount);

        int nMarkIndex = 0;
        for(int r = 0; r < m_nRowCount - 1; r++)
        {
            for(int c = 0; c < m_nColCount - 1; c++)
            {
                const RECT& rcGrid = m_vecGrids[r*m_nColCount + c];

                m_vecMarkerCoord[nMarkIndex].x = rcGrid.right;
                m_vecMarkerCoord[nMarkIndex].y = rcGrid.bottom;
                nMarkIndex ++;
            }
        }
    }




    //@����:�ռ�ÿ��Ȧ���ϵ�У����ı����Ϣ
    void CollectLayerMarkInfo()
    {

        //����Ȧ����Ŀ
        //��ʽLayerNum = (min(row, col) + 1)/2
        int nLayerNum = 0;
        if(m_nRowCount > m_nColCount)
        {
            nLayerNum = m_nColCount/2;
        }
        else 
        {
            nLayerNum = m_nRowCount /2;
        }

        m_vecLayers.resize(nLayerNum);


        //һ����У����ĸ���
        int nMarkNumInRow = m_nColCount - 1;

        //ÿ������һȦУ�����, �޳�һȦУ�����ʣ�µ�У���������������
        int nMarkCols = m_nColCount - 1;
        int nMarkRows = m_nRowCount - 1;

        //
        for(int L=0; L < nLayerNum; L++)
        {

            TRectLayer& rectLayer = m_vecLayers[L];

            ///��L���еĵ�һ��У����ı�Ŷ�Ӧ��L�еĵ�L��Ԫ��
            int nMarkNo = 0 + L*nMarkNumInRow + L; 
            if(1 == nMarkCols)//��ǰ��ֻ��һ��
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkRows);
                rectLayer.m_nHorzMarkNumber = 1;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                for(int i = 0; i < nMarkRows; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

            }//if
            else if(1 == nMarkRows)//��ǰ��ֻ��һ��
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkCols);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = 1;
                for(int i = 0; i< nMarkCols; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo ++;
                }
            }//else if
            else
            {
                int nLayerMarkCount = (nMarkCols - 1) + (nMarkRows - 1) + (nMarkCols - 1) + (nMarkRows - 1);
                rectLayer.m_vecScreenMarkNo.resize(nLayerMarkCount);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                int nLayerMarkIndex = 0;
                for(int i=0; i < nMarkCols -1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo++;
                }

                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

                for(int i=0; i < nMarkCols-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo--;
                }

                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo -= nMarkNumInRow;
                }

                if(nMarkCols >= 2)
                {
                    nMarkCols -= 2;
                }

                if(nMarkRows >= 2)
                {
                    nMarkRows -= 2;
                }

            }//else

        }//for-each layer

    }


    //@����:��ʼ����ˢ����
    //@����:YMax, ����ֵ
    //      YMin, �ֵ
    //@˵��:������ÿ����ˢ�������������Ļ���ĵľ���ɵ�����ϵ��
    void InitGrayBrushes(BYTE YMin,BYTE YMax)
    {

        //ɾ������Ѵ����ĻҶȻ�ˢ
        DeleteAllGrayBrush();

        //ͶӰ��ͶӰʱ���������ͼ������, ���ڹ������ͷ�п��Կ���ͶӰ��������ɵ����ߣ�
        //���У��ͼ�����Էֱ棬�����Զ�У��ʧ�ܡ�
        //���ǽ��׿�����ȴ��м���������ǿ��
        //��׿����������Ļ���ĵĹ�ϵ����
        //Y = A*R^2 + B
        //
        const double dbYmin = (double)YMin;
        const double dbYmax = (double)YMax;

        //R=0, Y = Ymin = B
        double B = YMin;

        //R = Rmaxʱ, Y = Ymax = A*Rmax^(1/2) + B, 
        double Rmax2 = (double)(m_nRowCount*m_nRowCount)/4 + (double)(m_nColCount*m_nColCount)/4;
        double A = (dbYmax - dbYmin)/Rmax2;

        int nBrushCount = int(sqrt(Rmax2) + 0.5);
        m_vecGrayBrushes.resize(nBrushCount);

        for(int R=0; R < nBrushCount; R++)
        {
            double R2 = R*R;
            int Y = int(A*R2 + B);
            m_vecGrayBrushes[R] = ::CreateSolidBrush(RGB(Y , Y , Y ));
        }



        m_hWhiteBrush = ::CreateSolidBrush(RGB(255, 255, 255));
        m_hBlackBrush = ::CreateSolidBrush(RGB(  0,   0,   0));
    }

    void DeleteAllGrayBrush()
    {
        int nSize = m_vecGrayBrushes.size();

        for(int i=0; i < nSize; i ++)
        {

        }

        m_vecGrayBrushes.clear();

        DeleteObject(m_hWhiteBrush);
        DeleteObject(m_hBlackBrush);

    }

    //member variables
    SIZE   m_patternSize        ;//У��ͼ���ĳߴ�
    int    m_nRowCount          ;//У��ͼ���ľ��ο�����
    int    m_nColCount          ;//У��ͼ���ľ��ο�����
    int    m_nCalibrateMarkCount;//

    int    m_nUpperPartMarkCount;//�ϰ벿У�������
    int    m_nUpperGridCount    ;//�ϰ벿���ο�����
    int    m_nUpperGridRow      ;//�ϰ벿���ο�����


    int    m_nLowerPartMarkCount;//�°벿У�������
    int    m_nLowerGridCount    ;//�°벿���ο�����
    int    m_nLowerGridRow      ;//�°벿���ο�����


    RECT   m_UpperArea          ;//�ϰ벿����
    RECT   m_LowerArea          ;//�°벿����

    std::vector<RECT>  m_vecGrids      ;//У��ͼ���е�ÿ��С�ľ��ο��λ�����ꡣ
    std::vector<POINT> m_vecMarkerCoord;//У��ͼ���е�ÿ��У���������
    std::vector<TRectLayer> m_vecLayers;//Ȧ����Ϣ

    std::vector<HBRUSH> m_vecGrayBrushes;//�ҶȻ�ˢ��
    HBRUSH m_hWhiteBrush;//��ɫ��ˢ
    HBRUSH m_hBlackBrush;//��ɫ��ˢ

};//CArithSeriesRowPattern







//Բ��У��ͼ��
class CCirclePattern
{
public:

    CCirclePattern()
    {
        m_nRadius   = 40;
        m_nColCount = 9;
        m_nRowCount = 5;
        m_nCalibrateMarkCount = m_nColCount*m_nRowCount;

    }

    int CalculateCalibPatternRadius(E_AutoCalibratePattern ePattern, int nScreenWidth)
    {
        int nCols = 5;
        switch(ePattern)
        {
        case E_CALIBRATE_PATTERN_5_COLS:
            nCols = 5;
            break;

		case E_CALIBRATE_PATTERN_6_COLS:
			nCols = 6;
			break;
		case E_CALIBRATE_PATTERN_7_COLS:
			nCols = 7;
			break;

		case E_CALIBRATE_PATTERN_8_COLS:
			nCols = 8;
			break;

        case E_CALIBRATE_PATTERN_9_COLS:
            nCols = 9;
            break;

        case E_CALIBRATE_PATTERN_10_COLS:
            nCols =10;
            break;

        case E_CALIBRATE_PATTERN_11_COLS:
            nCols = 11;
            break;

        case E_CALIBRATE_PATTERN_12_COLS:
            nCols = 12;
            break;

        case E_CALIBRATE_PATTERN_13_COLS:
            nCols = 13;
            break;

        case E_CALIBRATE_PATTERN_14_COLS:
            nCols = 14;

        case E_CALIBRATE_PATTERN_15_COLS:
            nCols = 15;

        case E_CALIBRATE_PATTERN_16_COLS:
            nCols = 16;
            break;

        case E_CALIBRATE_PATTERN_17_COLS:
            nCols = 17;
            break;

        default:
            nCols = 5;
        }

		int leftMargin  = 10;// (nRadius >> 2) > 10 ? nRadius >> 2): 10;
		int rightMargin = 10;// (nRadius >> 2) > 10 ? nRadius >> 2): 10;

        //W - leftMargin - rightMargin - 2*R = (nCols - 1)*4R

        int R = (nScreenWidth - leftMargin - rightMargin)/(4*(nCols-1) + 2);

        return R;
    }


    //����:��ʼ��ͼ��
    virtual void InitPattern(int nRadius, const RECT& rcMonitor)
    {

        if(nRadius <= 0) nRadius = 40;
        m_nRadius = nRadius;

        int nScreenWidth  = rcMonitor.right - rcMonitor.left;
        int nScreenHeight = rcMonitor.bottom - rcMonitor.top;

        int leftMargin   = 10;//nRadius/4;
        int rightMargin  = 10;//nRadius/4;
        int topMargin    = 10;//nRadius/4;
        int bottomMargin = 10;//nRadius/4;

        int fourRadius = 4*nRadius;


         m_nColCount  = (nScreenWidth  - leftMargin - rightMargin - nRadius - nRadius) / fourRadius + 1;
         m_nRowCount  = (nScreenHeight - leftMargin - rightMargin - nRadius - nRadius) / fourRadius + 1;
        
         //�����*�г˻�����MAX_OBJ_NUMBER,��С�����������еĴ��ߡ�
         if (m_nColCount * m_nRowCount > MAX_OBJ_NUMBER)
         {
             if (m_nColCount > m_nRowCount)
             {
                 m_nColCount = MAX_OBJ_NUMBER / m_nRowCount;
             }

             if (m_nRowCount > m_nColCount)
             {
                 m_nRowCount = MAX_OBJ_NUMBER / m_nColCount;
             }
         }

         if (m_nColCount < 2)
         {
             m_nColCount = 2;
         }

         if (m_nRowCount < 2)
         {
             m_nRowCount = 2;
         }
        
        int nHorzInterval_integer     = (nScreenWidth - leftMargin - rightMargin - nRadius - nRadius) / (m_nColCount - 1);
        int nHorzInterval_remainder   = (nScreenWidth - leftMargin - rightMargin - nRadius - nRadius) % (m_nColCount - 1);

        int nVertInterval_integer     = (nScreenHeight - topMargin - bottomMargin - nRadius - nRadius) / (m_nRowCount - 1);
        int nVertInterval_remainder   = (nScreenHeight - topMargin - bottomMargin - nRadius - nRadius) % (m_nRowCount - 1);


        //����У�������
        m_nCalibrateMarkCount= m_nColCount * m_nRowCount;


        m_vecMarkerCoord.resize(m_nCalibrateMarkCount);

        int nCircleIndex = 0;

		int x = rcMonitor.left;
		int y = rcMonitor.top ;
        int x_fraction_count = 0;
        int y_fraction_count = 0;

        y = rcMonitor.top + nRadius;
        for(int r = 0; r < m_nRowCount; r++)
        {
            x = rcMonitor.left + leftMargin + nRadius;
            for(int c = 0; c < m_nColCount; c++)
            {
                
                m_vecMarkerCoord[nCircleIndex].x = x;
                m_vecMarkerCoord[nCircleIndex].y = y;

                nCircleIndex ++;

                x += nHorzInterval_integer;

                x_fraction_count += nHorzInterval_remainder;
                if(x_fraction_count > (m_nColCount - 1))
                {
                    x_fraction_count -= (m_nColCount - 1);
                    x ++;
                }

            }//for(c)

            y += nVertInterval_integer;
            y_fraction_count += nVertInterval_remainder;
            if(y_fraction_count > (m_nRowCount - 1))
            {
                y_fraction_count -= (m_nRowCount - 1);
                y ++;
            }

        }//for(r)
        
        
        //�ռ�Ȧ����Ϣ
        CollectLayerMarkInfo();
    }


	//@����:����У��Բ�İ뾶��
	int GetCircleRaidus()const
	{
		return m_nRadius;
	}

    //@����:����Ȧ�����Ŀ
    int GetLayerCount()const
    {
        return m_vecLayers.size();
    }

    //@����:����У���������
    int GetCalibrateMarkCount()const
    {
        return m_nCalibrateMarkCount;
    }


    const POINT& GetMarkCoord(int i)const
    {
        assert( i>= 0 && i< this->m_nCalibrateMarkCount);

        return this->m_vecMarkerCoord[i];
    }



    //@����:����ָ����Ȧ����Ϣ
    const TRectLayer&  GetLayer(int nLayerIndex)const
    {
        assert(nLayerIndex >=0 && nLayerIndex < (int)m_vecLayers.size());

        return m_vecLayers[nLayerIndex];
    }


    //@����:����������У��ͼ��
    virtual void DrawPattern(HWND hWnd, COLORREF clrForeground = RGB(255,255,255), COLORREF clrBackground = RGB(0,0,0))
    {
        //do nothing
        if(hWnd == NULL) return;

        HDC hdc = ::GetDC(hWnd);
        HBRUSH hBlackBrush = ::CreateSolidBrush(clrBackground);//��ɫ��ˢ
        HBRUSH hWhiteBrush = ::CreateSolidBrush(clrForeground);//��ɫ��ˢ

		/*
		COLORREF clrGray = RGB(\
			max(0, GetRValue(clrForeground) - 50),\
			max(0, GetGValue(clrForeground) - 50),\
			max(0, GetBValue(clrForeground) - 50));
		
		HBRUSH hGrayBrush = ::CreateSolidBrush(clrGray);
		*/


        HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc, hWhiteBrush);

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        FillRect(hdc, &rcClient, hBlackBrush);

        int nMarkCount = m_vecMarkerCoord.size();

        for(int i = 0; i < nMarkCount; i++)
        {
            
            RECT rcCircle;
            rcCircle.left   = m_vecMarkerCoord[i].x - m_nRadius;
            rcCircle.top    = m_vecMarkerCoord[i].y - m_nRadius;
            rcCircle.right  = m_vecMarkerCoord[i].x + m_nRadius;
            rcCircle.bottom = m_vecMarkerCoord[i].y + m_nRadius;
            
			//ӳ����Ļ���굽У�����������
			MapWindowPoints(
				NULL,       //handle to the source window, desktop
				hWnd,       //handle to destination window
				(LPPOINT)&rcCircle,             //array of points to map 
				sizeof(RECT) / sizeof(POINT)//number of points in array
			);

            Ellipse(hdc, 
                rcCircle.left,
                rcCircle.top,
                rcCircle.right,
                rcCircle.bottom);

			/*
			SelectObject(hdc, hGrayBrush);

			int nXRadial1 = rcCircle.right;
			int nYRaidal1 = (rcCircle.top + rcCircle.bottom) >> 1;
			int nXRadial2 = (rcCircle.left + rcCircle.right) >> 1;
			int nYRaidal2 = rcCircle.top;

			Pie(hdc,
				rcCircle.left,
				rcCircle.top,
				rcCircle.right,
				rcCircle.bottom,
				nXRadial1,
				nYRaidal1,
				nXRadial2,
				nYRaidal2);


			 nXRadial1 = rcCircle.left;
			 nYRaidal1 = (rcCircle.top + rcCircle.bottom) >> 1;
			 nXRadial2 = (rcCircle.left + rcCircle.right) >> 1;
			 nYRaidal2 = rcCircle.bottom;

			Pie(hdc,
				rcCircle.left,
				rcCircle.top,
				rcCircle.right,
				rcCircle.bottom,
				nXRadial1,
				nYRaidal1,
				nXRadial2,
				nYRaidal2);
			
			SelectObject(hdc, hWhiteBrush);
			*/
			
       
        }//for
        

          SelectObject(hdc, hOldBrush);
        ::DeleteObject(hWhiteBrush);
        ::DeleteObject(hBlackBrush);
		//::DeleteObject(hGrayBrush);

        ReleaseDC(hWnd, hdc);

    }


protected:
      //@����:�ռ�ÿ��Ȧ���ϵ�У����ı����Ϣ
    void CollectLayerMarkInfo()
    {

        //����Ȧ����Ŀ
        //��ʽLayerNum = (min(row, col) + 1)/2
        int nLayerNum = 0;
        if(m_nRowCount > m_nColCount)
        {
            nLayerNum = (m_nColCount + 1)/2;
        }
        else 
        {
            nLayerNum = (m_nRowCount  + 1)/2;
        }

        m_vecLayers.resize(nLayerNum);


        //һ����У����ĸ���
        int nMarkNumInRow = m_nColCount;

        //ÿ������һȦУ�����, �޳�һȦУ�����ʣ�µ�У���������������
        int nMarkCols = m_nColCount;
        int nMarkRows = m_nRowCount;

        //
        for(int L=0; L < nLayerNum; L++)
        {

            TRectLayer& rectLayer = m_vecLayers[L];

            ///��L���еĵ�һ��У����ı�Ŷ�Ӧ��L�еĵ�L��Ԫ��
            int nMarkNo = 0 + L*nMarkNumInRow + L; 
            if(1 == nMarkCols)//��ǰ��ֻ��һ��
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkRows);
                rectLayer.m_nHorzMarkNumber = 1;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                for(int i = 0; i < nMarkRows; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }

            }//if
            else if(1 == nMarkRows)//��ǰ��ֻ��һ��
            {
                rectLayer.m_vecScreenMarkNo.resize(nMarkCols);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = 1;
                for(int i = 0; i< nMarkCols; i++)
                {
                    rectLayer.m_vecScreenMarkNo[i] = nMarkNo;
                    nMarkNo ++;
                }
            }//else if
            else
            {
                int nLayerMarkCount = (nMarkCols - 1) + (nMarkRows - 1) + (nMarkCols - 1) + (nMarkRows - 1);
                rectLayer.m_vecScreenMarkNo.resize(nLayerMarkCount);
                rectLayer.m_nHorzMarkNumber = nMarkCols;
                rectLayer.m_nVertMarkNumber = nMarkRows;

                int nLayerMarkIndex = 0;

                //Ȧ�㶥��
                for(int i = 0; i < nMarkCols -1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo++;
                }

                //Ȧ���ұ�
                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo += nMarkNumInRow;
                }
                
                //Ȧ��ױ�
                for(int i=0; i < nMarkCols-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo--;
                }
                
                //Ȧ�����
                for(int i=0; i < nMarkRows-1; i++)
                {
                    rectLayer.m_vecScreenMarkNo[nLayerMarkIndex ++ ] = nMarkNo;
                    nMarkNo -= nMarkNumInRow;
                }

                if(nMarkCols >= 2)//��ȥ��Ȧ����һ��У����ǵĸ�����
                {
                    nMarkCols -= 2;
                }

                if(nMarkRows >= 2)//��ȥ��Ȧ����һ��У����ǵĸ�����
                {
                    nMarkRows -= 2;
                }

            }//else

        }//for-each layer

    }



protected:
    int                     m_nRadius       ;//ԲȦͼ���İ뾶
    int                     m_nCalibrateMarkCount;//ԲȦͼ���ĸ���
    int                     m_nColCount     ;//У��ԲȦ������
    int                     m_nRowCount     ;//У��ԲȦ������
    std::vector<POINT>      m_vecMarkerCoord;//У��ͼ���е�ÿ��У���������
    std::vector<TRectLayer> m_vecLayers     ;//Ȧ����Ϣ
    std::vector<HBRUSH>     m_vecGrayBrushes;//�ҶȻ�ˢ��
    //HBRUSH                  m_hWhiteBrush   ;//��ɫ��ˢ
    //HBRUSH                  m_hBlackBrush   ;//��ɫ��ˢ
    

};
