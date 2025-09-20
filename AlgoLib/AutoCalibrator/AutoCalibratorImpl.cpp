#include "stdafx.h"
#include "AutoCalibratorImpl2.h"
#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "GlobalVar.h"
#include "SIMD.h"
#include "FillPolygon.h"
#include "SortInDirection.h"
#include <limits>
//@����:��ʼУ��, ���ⲿģ�����
//@����:hNotifyWnd, ��Ϣ֪ͨ����
//      E_AutoCalibratePattern, У������ö��ֵ
//      bSaveInermediatFile, �Ƿ񱣴�У�����м���ͼƬ
//      bRecordVideo���Ƿ��¼У�������е�ԭʼ��Ƶ
BOOL CAutoCalibratorImpl2::StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern ePattern, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx, BOOL bSaveInermediatFile  ,BOOL bRecordVideo)
{
    m_bSaveInermediatFile  = bSaveInermediatFile;
    m_bRecordVideo         = bRecordVideo;
    m_fpChangeCameraParams = ChangeCameraParamsProc;
    m_lpCtx                = lpCtx;

    if(m_bIsCalibrating)
    {
        return FALSE;
    }


    //����
    if(HWND(m_AutoCalibrateWnd) == NULL)
    {
        if(!m_AutoCalibrateWnd.CreateFullScreen())
        {
            return FALSE;
        }
    }


    m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_UNDEF;
    m_hNotifyWnd = hNotifyWnd;
    m_AutoCalibrateWnd.ShowWindow(SW_SHOW);
    m_AutoCalibrateWnd.SetFocus();


    //<<2013/03/11,by toxuke@gmail.com �����Զ�У�����峬ʱ���״̬
    m_AutoCalibrateWnd.StartTimeoutDetect();
    //>>


    // m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//�ڰ׻��㷨
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    m_nStageWaitCount = 0;
    m_bIsCalibrating = TRUE;
    m_bIsSimulatedCalibrating = FALSE;

    //InitAutoCalibrateSaveDirectory();
    SYSTEMTIME now;
    GetLocalTime(&now);
    m_strCliabrateDebugDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);



    CAtlString strFileName;
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    strFileName.Format(_T("%s\\AutoCalibrate_%04d%02d%02d-%02d%02d%02d%03d.avi"),
        m_strCliabrateDebugDir,
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond,
        localTime.wMilliseconds);

    m_uCx = GetSystemMetrics(SM_CXSCREEN);
    m_uCy = GetSystemMetrics(SM_CYSCREEN);

    m_ePattern = ePattern;

    //���������
    ShowCursor(FALSE);


    m_nTryTimes = 1;
    //����ͼƬ���Ŀ¼
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
    SetImageDebugDir(strImageDubugDir);

    if(m_bRecordVideo)//��Ҫ����У����Ƶ
    {
        m_oAVIRecorder.OpenFile(strFileName, 640, 480);
    }

    return TRUE;
}


//@����:ֹͣУ��
void CAutoCalibratorImpl2::EndCalibrating()
{
    ShowCursor(TRUE);
    if(m_bIsCalibrating)
    {
        if(m_bIsSimulatedCalibrating)
        {
            PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0 );
            //WaitForSingleObject(m_hSimulatedCalibrateThread, INFINITE);
            m_oAVIInput.Close();
        }
        else
        {
            //����У������
            m_AutoCalibrateWnd.ShowWindow(SW_HIDE);

            //�رճ�ʱ��⹦��
            m_AutoCalibrateWnd.CloseTimeoutDetect();

            m_bIsCalibrating = FALSE;


            if(m_bRecordVideo)//��ϸ���ʱ, ������Ƶ
            {
                m_oAVIRecorder.Close();
            }

            if(E_AUTO_CALIBRATOR_OK == this->m_eCalibrateError && !this->m_bSaveInermediatFile)
            {//�ɹ������ò������Զ�У���м��ļ���־

                //ɾ���ɹ�У�����У��ͼƫ��AVI�ļ�
                DeleteDirectory(m_strCliabrateDebugDir);
            }
        }



        PostMessage(m_hNotifyWnd,WM_AUTO_CALIBRATE_DONE, WPARAM(this->m_eCalibrateError), LPARAM(this->m_bIsSimulatedCalibrating));
    }
}

//@����:��ʼ����Ļ�ϲ����²��߽��ɫС�����λ��
//@����:hWnd, У��������
//      nHorzSideSquareNumber, ˮƽ�����ɫС����ĸ���
//      VertSideSquareNumber, ��ֱ�����ɫС����ĸ���
//      nSquareWidth, ��ɫС����߳�
//@˵��:ע��:�����ֱ����İ�ɫС�������ĿΪ�����������������������������غϵġ�

void CAutoCalibratorImpl2::InitBorderWhiteSquarePositions(HWND hWnd,int nHorzSideSquareNumber, int nVertSideSquareNumber, int nSquareWidth)
{
    //�Ӽ������Ļ�����Ͻǿ�ʼ����˳ʱ�뷽�����ζԸ�����ɫС������
    //����:
    //0     1     2     3
    //                   
    //9                 4
    //                   
    //8    7      6     5
    //

    RECT rcClient;
    GetClientRect(hWnd,&rcClient);

    int i= 0;
    int W  = rcClient.right  - rcClient.left;
    int H  = rcClient.bottom - rcClient.top ;


    int  nHorzIntervalNum = nHorzSideSquareNumber - 1;
    int nVertIntervalNum  = nVertSideSquareNumber - 1;
    int _HorzInterval_Integer  = (W - nSquareWidth)/nHorzIntervalNum;//ÿ�������ˮƽ�����ֵ����������
    int _HorzInterval_Fraction = (W - nSquareWidth)%nHorzIntervalNum;//ÿ�������ˮƽ�����ֵ����������

    int _VertInterval_Integer  = (H - nSquareWidth)/nVertIntervalNum;//ÿ�������ˮƽ�����ֵ����������
    int _VertInterval_Fraction = (H - nSquareWidth)%nVertIntervalNum;//ÿ�������ˮƽ�����ֵ����������

    RECT rcSquare;
    rcSquare.left   = rcClient.left;
    rcSquare.top    = rcClient.top ;
    rcSquare.right  = rcSquare.left + nSquareWidth;
    rcSquare.bottom = rcSquare.top  + nSquareWidth;

    int _FractionCount = 0;

    int iIndex = 0;

    int nSqureNumber = 2*(nHorzSideSquareNumber - 1) + 2*(nVertSideSquareNumber - 1);
    m_vecBorderSquarePositions.resize(nSqureNumber);


    //����
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;

        rcSquare.left  += _HorzInterval_Integer;
        _FractionCount += _HorzInterval_Fraction;
        if(_FractionCount > nHorzIntervalNum)
        {
            rcSquare.left ++;
            _FractionCount -= nHorzIntervalNum;
        }

        rcSquare.right = rcSquare.left + nSquareWidth;
    }


    _FractionCount = 0;
    //�ұ�
    for(i=0; i < nVertIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.top   += _VertInterval_Integer;
        _FractionCount += _VertInterval_Fraction;
        if(_FractionCount > nVertIntervalNum)
        {
            rcSquare.top ++;
            _FractionCount -= nVertIntervalNum;
        }
        rcSquare.bottom = rcSquare.top + nSquareWidth;

    }




    _FractionCount = 0;
    //�ױ�
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.left  -= _HorzInterval_Integer;
        _FractionCount -= _HorzInterval_Fraction;
        if(_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.left --;
            _FractionCount += nHorzIntervalNum;
        }
        rcSquare.right = rcSquare.left + nSquareWidth;


    }


    //���
    _FractionCount = 0;
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.top   -= _VertInterval_Integer;
        _FractionCount -= _VertInterval_Fraction;
        if(_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.top --;
            _FractionCount += nHorzIntervalNum;
        }
        rcSquare.bottom = rcSquare.top + nSquareWidth;
    }

    //���һ������
    m_vecBorderSquarePositions[iIndex] = rcSquare;

    //���߽�С�׿��Ϊ����������
    //�ϰ벿��
    //0     1     2     3
    //                   
    //9                 4
    int nUpperHalfSquareNumber = nHorzSideSquareNumber + ((nVertSideSquareNumber + 1)/2 - 1)*2;
    m_vecUpperHalfSquarePositions.resize(nUpperHalfSquareNumber);

    int nHalfVertSideSquareNumber = (nVertSideSquareNumber + 1)/2;
    memcpy(&m_vecUpperHalfSquarePositions[0], &m_vecBorderSquarePositions[0], sizeof(RECT)*(nHorzSideSquareNumber + nHalfVertSideSquareNumber - 1));

    memcpy(
        &m_vecUpperHalfSquarePositions[nHorzSideSquareNumber + nHalfVertSideSquareNumber - 1],
        &m_vecBorderSquarePositions[nSqureNumber - (nHalfVertSideSquareNumber - 1)],
        sizeof(RECT)*nHalfVertSideSquareNumber - 1
        );





    //�°벿��
    //9                 4
    //                   
    //8    7      6     5
    int nLowerHalfSquareNumber =  nHorzSideSquareNumber + ((nVertSideSquareNumber + 1)/2 - 1)*2;
    m_vecLowerHalfSquarePositions.resize(nLowerHalfSquareNumber);

    memcpy(
        &m_vecLowerHalfSquarePositions[0],
        &m_vecBorderSquarePositions[nHorzSideSquareNumber + nVertSideSquareNumber/2 - 1],
        sizeof(RECT)*nLowerHalfSquareNumber
        );
}






//@����:��ʾΧ�Ʊ߽�İ�ɫ����
//@����:hWnd, ���������ϵĴ�����
//      nSquareWidth,��ɫ����ı߳�
//      nNumberEachSide, ÿ���ߵİ�ɫ�������Ŀ
//@����:2013/10/22
void CAutoCalibratorImpl2::DrawSmallSquares(HWND hWnd, const RECT* SquarePositions, int nSquareNumber)
{

    HDC hDC = GetDC(hWnd);

    HBRUSH hBrush = ::CreateSolidBrush(WHITE);

    for(int i=0; i<nSquareNumber; i++)
    {
        FillRect(hDC, &SquarePositions[i], hBrush);
    }


    DeleteObject(hBrush);
    ReleaseDC(hWnd, hDC);

}


//@����:����"��Ļ��������"��״̬����״̬
//@����:grayImage, �����У��ͼƬ
void CAutoCalibratorImpl2::ResetSubStateMachine_SearachScreenArea(const CImageFrame& grayImage)
{
    m_tSubStateMachine_SearchScreenArea.nRunTimes   = 0;
    m_tSubStateMachine_SearchScreenArea.nFlashTimes = 0;
    m_tSubStateMachine_SearchScreenArea.bShowSquare = FALSE;
    m_tSubStateMachine_SearchScreenArea.frameAccumulate.SetSize(grayImage.Width(), grayImage.Height(), 2);
    m_tSubStateMachine_SearchScreenArea.frameAccumulate.Clear();
    if(FALSE == this->m_bIsSimulatedCalibrating)
    {
        FillBoard(m_AutoCalibrateWnd, 0, BLACK);
    }

}


//@����:�������а�ɫ��������ĵ㣬����Щ������������ĵķ�λ�ǵĴ�С��������
//@����:srcImage, 16λ�Ҷ�λͼ
//      pPts, �������, �����ҵ������ĵ�����
//      nExpectedNumber, �����ҵ��ĵ�ĸ���
//     pGlobalCentroid ,�������, ȫ������
BOOL CAutoCalibratorImpl2::SearchSquareCentroids(const CWordFrame&  srcImage, POINT* pPts, int nExpectedNumber, POINT* pGlobalCentroid)
{
    //16λ�Ҷ�ͼƬתΪ8λ�Ҷ�ͼƬ
    int nImageWidth  = srcImage.Width();
    int nImageHeight = srcImage.Height();

    int nPixelCount = nImageWidth*nImageHeight;
    int nLoopCount  = nPixelCount;

    WORD max = srcImage.Max();
    WORD min = srcImage.Min();
    WORD range = max -min;
    const WORD* pWordPixel = srcImage.GetData();

    static CImageFrame grayImage;
    grayImage.SetSize(nImageWidth, nImageHeight, 2);

    BYTE* pPixel = grayImage.GetData();


    //16λ�Ҷ�λͼתΪ8λ�Ҷ�λͼ
    while(nLoopCount)
    {
        *pPixel = BYTE( (UINT)255 * (UINT)(*pWordPixel - min)/(UINT)range);
        pWordPixel++;
        pPixel ++;
        nLoopCount --;
    }

    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(grayImage, _T("BorderSquaresSrc.bmp"));
    }


    //���ݰ�ɫ������ռ��Ļ����İٷֱȹ����ɫ������������Ƶͼ������ռ�����ظ���
    int nHightPixelCountEstimated = SQUARE_SIZE * SQUARE_SIZE * nExpectedNumber * nImageWidth * nImageHeight /(m_uCx * m_uCy);

    //ͳ��ֱ��ͼ
    int hist[256];
    memset(hist, 0, sizeof(hist));
    nLoopCount = nPixelCount;
    pPixel = grayImage.GetData();
    while(nLoopCount)
    {
        hist[*pPixel] ++;
        pPixel ++;
        nLoopCount --;
    }


    BYTE threshold = 255;
    int nSum = 0;
    for(threshold=255;threshold > 0; threshold--)
    {
        nSum += hist[threshold];

        if(nSum >= nHightPixelCountEstimated)
        {
            break;
        }

    }


    static CBitFrame bitFrame;
    bitFrame.SetSize(nImageWidth, nImageHeight);

    static CImageFrame debugImage;
    debugImage.SetSize(nImageWidth, nImageHeight, 1);



    //8λ�Ҷ�λͼת��Ϊ��ֵ����1λλͼ
    GrayToBitFrame_SSE2(
        grayImage.GetData(), 
        (BYTE*)bitFrame.GetData(),
        threshold,
        nPixelCount);



    //Ŀ����
    CConnectedComponentScanlineAlogrithm<> blobDetect;

    blobDetect.ProcessImage(
        (const BYTE *)bitFrame.GetData(),
        nImageWidth,
        nImageHeight);


    int nMassThreshold = 2;//�������ޣ�����С�ڸ����޵�Ŀ�걻����

    int nSquaresFound  = blobDetect.GetObjCount(nMassThreshold);

    //BOOL bInc = TRUE;//�������ӱ�־

    //ͨ����⵽��Ŀ�����Ŀ����̬ȷ�����޵ĸߵ�
    if(nSquaresFound != nExpectedNumber)
    {
        //if(nSquaresFound < nExpectedNumber)
        //{    //Ŀ��С������ֵ, �򽵵�����
        //    bInc = FALSE;
        //}
        //else
        //{
        //    bInc = TRUE;
        //}

        do
        {
            //if(bInc)
            //{
            if(threshold == 0xFF)
            {
                break;
            }
            threshold ++;

            //}
            //else
            //{
            //    if(threshold == 0x00)
            //    {
            //        break;
            //    }
            //    threshold --;
            //}

            GrayToBitFrame_SSE2(
                grayImage.GetData(), 
                (BYTE*)bitFrame.GetData(),
                threshold,
                nPixelCount);

            blobDetect.ProcessImage(
                (const BYTE *)bitFrame.GetData(),
                nImageWidth,
                nImageHeight);


            nSquaresFound  = blobDetect.GetObjCount(nMassThreshold);


            if(nSquaresFound == nExpectedNumber)
            {
                break;
            }

            //if(bInc && nSquaresFound <= nExpectedNumber)
            //{
            //    break;
            //}
            //else if(!bInc && nSquaresFound >=  nExpectedNumber)
            //{
            //    break;
            //}


        }while(1);
    }



    if(nSquaresFound != nExpectedNumber)
    {
        return FALSE;
    }
    else//�ɹ�
    {

        //��ֵ����1λλͼתΪ8λ�Ҷ�λͼ
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            debugImage.GetData(),
            bitFrame.GetPixelCount());

        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            Debug_SaveImageFrame(debugImage, _T("BorderSquaresBinarization.bmp"));
        }



        //����С������Ŀ��
        blobDetect.FilterObject(nMassThreshold);


        //��ȡ�ҵ��ı߽��

        const TBlobObject* pBlobObj =  blobDetect.GetObjs();
        for(UINT i=0; i < blobDetect.GetObjCount(0); i++)
        {

            pPts[i].x = pBlobObj[i].GPos.x;
            pPts[i].y = pBlobObj[i].GPos.y;

        }

        //�������б߽�������
        POINT ptCentroid;

        int mx = 0;
        int my = 0;
        for(int i=0; i < nExpectedNumber; i++)
        {
            mx += pPts[i].x;
            my += pPts[i].y;
        }


        ptCentroid.x = mx/nExpectedNumber;
        ptCentroid.y = my/nExpectedNumber;

        //���ҵ��ı߽�㰴�շ�λ�Ƕ�����
        SortInDirection(&pPts[0], nExpectedNumber, ptCentroid);


        if(pGlobalCentroid)
        {
            *pGlobalCentroid = ptCentroid;
        }
        return TRUE;
    }
}


//@����:����߽���η���
BOOL CAutoCalibratorImpl2::ProcessBorderSquareImages(const CWordFrame&  srcImage, CAutoCalibratorImpl2::ESearchScreenPart eScreenPart)
{
    CImageFrame* pMaskFrame = NULL;
    POINT* pPts = NULL;
    POINT* pGlobalCentroid = NULL;
    int nPtNumber = 0;
    switch(eScreenPart)
    {

    case E_SEARCH_SCREEN_UPPER:
        m_vecUpperBorderCalibratePts.resize(m_vecUpperHalfSquarePositions.size());
        pPts            = &m_vecUpperBorderCalibratePts[0];
        nPtNumber       = m_vecUpperBorderCalibratePts.size();
        pMaskFrame      = &m_oUpperHalfMaskFrame;
        pGlobalCentroid = &m_ptUpperCentroid;
        break;

    case E_SEARCH_SCREEN_LOWER:
        m_vecLowerBorderCalibratePts.resize(m_vecLowerHalfSquarePositions.size());
        pPts            = &m_vecLowerBorderCalibratePts[0];
        nPtNumber       = m_vecLowerBorderCalibratePts.size();
        pMaskFrame      = &m_oLowerHalfMaskFrame;
        pGlobalCentroid = &m_ptLowerCentroid;
        break;

    case E_SEARCH_SCREEN_WHOLE:
        m_vecBorderCalibratePts.resize(m_vecBorderSquarePositions.size());
        pPts            = &m_vecBorderCalibratePts[0];
        nPtNumber       = m_vecBorderCalibratePts.size();
        pMaskFrame      = &m_oScreenMaskFrame;
        pGlobalCentroid = &m_ptAllCentroid;
        break;

    }//switch

    BOOL bRet = SearchSquareCentroids(srcImage, pPts, nPtNumber, pGlobalCentroid);
    if(!bRet) return FALSE;

    if(pMaskFrame)
    {
        int video_w = srcImage.Width();
        int video_h = srcImage.Height();
        //����ͼ��λ
        pMaskFrame->SetSize(video_w, video_h, 1);
        pMaskFrame->Clear();

        //������ͼ���������
        FillPolygon(
            pMaskFrame->GetData(),
            video_w,
            video_h,
            &pPts[0],
            nPtNumber,
            255,
            TRUE);



        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            Debug_SaveImageFrame(*pMaskFrame, _T("ScreenMask.bmp"));
        }

        ////��������λͼ4��
        //int nDilateTimes = 4;
        //for(int i=0;i < nDilateTimes; i++)
        //{
        //    Morph_Dilate8(
        //        pMaskFrame->GetData(),
        //        pMaskFrame->GetData(),
        //        pMaskFrame->Width(),
        //        pMaskFrame->Height());
        //}
        //if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        //{
        //    Debug_SaveImageFrame(*pMaskFrame, _T("ScreenMask_Dilated.bmp"));
        //}

    }

    return TRUE;
}



//@����:grayImage, �Ҷ�ͼƬ
CAutoCalibratorImpl2::ESubStateMachineStatus CAutoCalibratorImpl2::RunSubStateMachine_SearchScreenArea(const CImageFrame& grayImage, CAutoCalibratorImpl2::ESearchScreenPart eScreenPart)
{
    ESubStateMachineStatus status = E_SUB_STATE_MACHINE_CONINTUE;

    //���м�����+1
    m_tSubStateMachine_SearchScreenArea.nRunTimes ++;

    if(m_tSubStateMachine_SearchScreenArea.nRunTimes % MAX_STAGE_WAIT_COUNT == 0)
    {

        if(m_tSubStateMachine_SearchScreenArea.bShowSquare)//��ʾ����
        {

            CImageFrame diffImage = grayImage;
            diffImage -=  m_tSubStateMachine_SearchScreenArea.frameBackground ;

            if(this->m_bIsSimulatedCalibrating)
            {
                //�������
                m_oDebugWindow.PrintFrame(diffImage);
            }

            int video_w = diffImage.Width();
            int video_h = diffImage.Height();

            int nPixelCount = video_w * video_h;

            BYTE* pDiffPixel = diffImage.GetData();
            WORD* pWordPixel = m_tSubStateMachine_SearchScreenArea.frameAccumulate.GetData();

            int nLoopCount = nPixelCount;
            while(nLoopCount)
            {

                if(*pDiffPixel>= 8)//���ֵ>8,����Ϊ����Ч����
                {
                    *pWordPixel += *pDiffPixel;
                }

                pWordPixel++;
                pDiffPixel++;
                nLoopCount --;
            }

            m_tSubStateMachine_SearchScreenArea.nFlashTimes ++;



            if( m_tSubStateMachine_SearchScreenArea.nFlashTimes >= MAX_FLASH_TIMES)
            {
                //��˸���̽���, �����ռ�����ͼ������

                if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
                {
                    Debug_SaveImageFrame(m_tSubStateMachine_SearchScreenArea.frameAccumulate, _T("AcccumulateDiff.bmp"));
                }

               //<<debug
                m_oDebugBorderSquareFrame.Merge(grayImage);
                //debug>>





                BOOL bRet = ProcessBorderSquareImages(
                    m_tSubStateMachine_SearchScreenArea.frameAccumulate,
                    eScreenPart);

                if(!bRet)
                {
                    return E_SUB_STATE_MACHINE_FAILED;
                }

                return E_SUB_STATE_MACHINE_FINISHED;
            }


            //ȫ������ʾ
            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                FillBoard(m_AutoCalibrateWnd, BLACK);
            }
            m_tSubStateMachine_SearchScreenArea.bShowSquare = FALSE;
        }
        else//δ��ʾ����
        {

            m_tSubStateMachine_SearchScreenArea.frameBackground = grayImage;



            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                switch(eScreenPart)
                {
                case E_SEARCH_SCREEN_UPPER://��ʾ�ϲ��߽緽��
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecUpperHalfSquarePositions[0], m_vecUpperHalfSquarePositions.size());
                    break;

                case E_SEARCH_SCREEN_LOWER://��ʾ�²��߽緽��
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecLowerHalfSquarePositions[0], m_vecLowerHalfSquarePositions.size());
                    break;

                case E_SEARCH_SCREEN_WHOLE://��ʾȫ���߽緽��
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecBorderSquarePositions[0], m_vecBorderSquarePositions.size());
                    break;

                }
            }

            m_tSubStateMachine_SearchScreenArea.bShowSquare = TRUE;
        }

    }

    return status;
}

//@����:��Ļ��������������ĺ���������Ҫ�м�����Ļ����������ͼ��������ת����
//@����:nImageWidth, ��Ƶͼ����
//      nImageHeight,��Ƶͼ��߶�
BOOL CAutoCalibratorImpl2::OnPostSearchScreenArea(int nImageWidth, int nImageHeight)
{
    //������ת����
    //�����нǵ�����ת����, ʹ֮�� ��Y ��X����ϵ
    //������ת����
    m_ptRoationCenter.x = (m_ptUpperCentroid.x + m_ptLowerCentroid.x)/2;
    m_ptRoationCenter.y = (m_ptUpperCentroid.y + m_ptLowerCentroid.y)/2;


    //
    //��תʸ��
    POINT Vx,Vy;

    double VLength;

    //Vy:����ת���ĵ��²����ĵ�ʸ��
    Vy.x = m_ptLowerCentroid.x - m_ptRoationCenter.x;
    Vy.y = m_ptLowerCentroid.y - m_ptRoationCenter.y;

    VLength = sqrt(double(Vy.x*Vy.x + Vy.y * Vy.y));

    //Vx��Vy
    Vx.x = Vy.y;
    Vx.y = -Vy.x;

    //������ת����
    //
    //|x|   |c11  c12| |u|
    //| | = |        |*| |
    //|y|   |c21  c22| |v|
    //��֤:
    //|u|  |Vx.x|    |x|  |Vx.x/VLength Vx.y/VLength| |Vx.x/VLength| |1|
    //| | =|    |    | | =|                         |*|            |=| |
    //|v|  |Vx.y| ʱ,|y|  |Vy.x/VLength Vy.y/VLength| |Vx.y/Vlenth | |0|
    //��
    //|u|  |Vy.x|    |x|  |Vx.x/VLength Vx.y/VLength| |Vy.x/VLength| |0|
    //| | =|    |    | | =|                         |*|            |=| |
    //|v|  |Vy.y| ʱ,|y|  |Vy.x/VLength Vy.y/VLength| |Vy.y/Vlenth | |1|

    //
    m_dbRotationMatrix[0][0] = (double)Vx.x/VLength;
    m_dbRotationMatrix[0][1] = (double)Vx.y/VLength;

    m_dbRotationMatrix[1][0] = (double)Vy.x/VLength;
    m_dbRotationMatrix[1][1] = (double)Vy.y/VLength;



    //�ϲ��ϲ��ı߽����²��ı߽��
    if(VERT_SIDE_SQUARE_NUMBER % 2 == 1)//��ֱ�����������߽��
    {

        //�������������ֹ���������, ��Ҫ�����������ҳ�

        int relatedIndex[2][2];//[0]:������С�����������������������
        ;//[1]:�������С�����������������������
        memset(relatedIndex, 0 ,sizeof(relatedIndex));
        int nMinimumDist[2]    ;//
        nMinimumDist[0] = (std::numeric_limits<int>::max)();
        nMinimumDist[1] = (std::numeric_limits<int>::max)();

        for(size_t i=0; i < m_vecUpperBorderCalibratePts.size(); i++)
        {
            const POINT& pt1 = m_vecUpperBorderCalibratePts[i];
            for(size_t j=0; j < m_vecLowerBorderCalibratePts.size(); j++)
            {
                const POINT& pt2 = m_vecLowerBorderCalibratePts[j];

                int dist2 = (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
                if(dist2 < nMinimumDist[0])
                {
                    relatedIndex[1][0] = relatedIndex[0][0];
                    relatedIndex[1][1] = relatedIndex[0][1];
                    nMinimumDist[1] = nMinimumDist[0];

                    nMinimumDist[0] = dist2;
                    relatedIndex[0][0] = i;
                    relatedIndex[0][1] = j;
                    continue;
                }

                if(dist2 < nMinimumDist[1])
                {
                    relatedIndex[1][0] = i;
                    relatedIndex[1][1] = j;
                    nMinimumDist[1] = dist2;
                }


            }
        }


        //m_vecBorderCalibratePts.resize(m_vecUpperBorderCalibratePts.size() + m_vecLowerBorderCalibratePts.size() - 2);
        m_vecBorderCalibratePts.resize(BORDER_SQUARE_NUMBER);

        for(size_t i=0; i<m_vecUpperBorderCalibratePts.size(); i++)
        {

            BOOL bMerged = FALSE;
            for(size_t j=0; j < _countof(relatedIndex); j++)
            {
                if(i == relatedIndex[j][0])
                {
                    POINT ptUpper = m_vecUpperBorderCalibratePts[i] ;
                    POINT ptLower = m_vecLowerBorderCalibratePts[relatedIndex[j][1]] ;

                    POINT ptAvg;
                    ptAvg.x     = (ptUpper.x + ptLower.x)>>1;
                    ptAvg.y     = (ptUpper.y + ptLower.y)>>1;

                    m_vecBorderCalibratePts[i] = ptAvg;
                    bMerged = TRUE;
                    break;
                }

            }

            if(!bMerged)
            {
                m_vecBorderCalibratePts[i] = m_vecUpperBorderCalibratePts[i];
            }
        }


        int k = m_vecUpperBorderCalibratePts.size();
        for(size_t i=0; i<m_vecLowerBorderCalibratePts.size(); i++)
        {

            BOOL bSkipped = FALSE;
            for(size_t j=0; j < _countof(relatedIndex); j++)
            {
                if(i == relatedIndex[j][1])
                {
                    bSkipped = TRUE;
                    break;
                }

            }

            if(!bSkipped)
            {
                m_vecBorderCalibratePts[k++] = m_vecLowerBorderCalibratePts[i];
            }
        }


    }
    else//��ֱ����ż�����߽��
    {

        m_vecBorderCalibratePts.resize(BORDER_SQUARE_NUMBER);

        memcpy(
            &m_vecBorderCalibratePts[0],
            &m_vecUpperBorderCalibratePts[0],
            sizeof(POINT)*m_vecUpperBorderCalibratePts.size());

        memcpy(
            &m_vecBorderCalibratePts[m_vecUpperBorderCalibratePts.size()],
            &m_vecLowerBorderCalibratePts[0],
            sizeof(POINT)*m_vecLowerBorderCalibratePts.size());
    }

    //��������������λͼ
    //�������б߽�������
    POINT ptCentroid;

    int mx = 0;
    int my = 0;
    for(size_t i=0; i < BORDER_SQUARE_NUMBER; i++)
    {
        mx += m_vecBorderCalibratePts[i].x;
        my += m_vecBorderCalibratePts[i].y;
    }


    ptCentroid.x = mx/m_vecBorderCalibratePts.size();
    ptCentroid.y = my/m_vecBorderCalibratePts.size();

    //���ҵ��ı߽�㰴�շ�λ�Ƕ�����
    SortInDirection(&m_vecBorderCalibratePts[0], m_vecBorderCalibratePts.size(), ptCentroid);


    m_ptAllCentroid = ptCentroid;


    //����ͼ��λ
    m_oScreenMaskFrame.SetSize(nImageWidth, nImageHeight, 1);
    m_oScreenMaskFrame.Clear();

    //������ͼ���������
    FillPolygon(
        m_oScreenMaskFrame.GetData(),
        nImageWidth,
        nImageHeight,
        &m_vecBorderCalibratePts[0],
        m_vecBorderCalibratePts.size(),
        255,
        TRUE);

    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(m_oScreenMaskFrame, _T("FullScrenMask.bmp"));
    }


    //�����߽���һȦ����,"���Ͻ�"�ĵ��������������, Ŀ���ǽ���"ͼ���"��"��Ļ��"��ӳ���ϵ��
    //
    //
    TurnPoint turnPts[4];
    int nFound = SearchTurnPoints(&m_vecBorderCalibratePts[0], m_vecBorderCalibratePts.size(), turnPts, _countof(turnPts));
    if(nFound != _countof(turnPts))
    {
        return FALSE;
    }

    //��ת�����ĸ��ǵ�
    for(int i=0; i < _countof(turnPts); i++)
    {
        long x = turnPts[i].ptCoord.x - m_ptRoationCenter.x;
        long y = turnPts[i].ptCoord.y - m_ptRoationCenter.y;

        turnPts[i].ptCoord.x = m_ptRoationCenter.x + long(m_dbRotationMatrix[0][0]*x + m_dbRotationMatrix[0][1]*y);
        turnPts[i].ptCoord.y = m_ptRoationCenter.y + long(m_dbRotationMatrix[1][0]*x + m_dbRotationMatrix[1][1]*y);
    }


    int nLeftTopIndex = MatchLeftTopCorner(turnPts, m_ptRoationCenter, _countof(turnPts));
    if(nLeftTopIndex == -1)
    {
        return FALSE;
    }


    //
    m_vecBorderCalibMap.resize(BORDER_SQUARE_NUMBER);

    int nImagePtIndex = turnPts[nLeftTopIndex].nLayerPtIndex;


    //
    for(size_t nScreenPtIndex=0; nScreenPtIndex < BORDER_SQUARE_NUMBER; nScreenPtIndex++)
    {
     
        m_vecBorderCalibMap[nScreenPtIndex].ptImageCoord = m_vecBorderCalibratePts[nImagePtIndex];
        
        const RECT rcSquare = m_vecBorderSquarePositions[nScreenPtIndex];
        
        

        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.x   = (rcSquare.left + rcSquare.right )/2;
        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.y   = (rcSquare.top  + rcSquare.bottom)/2;
    }



    //
    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(m_oScreenMaskFrame, _T("FullScrenMask.bmp"));
    }


   //��ʼ��У��ͼ��
    SIZE patternSize;
    patternSize.cx = this->m_uCx;
    patternSize.cy = this->m_uCy;


    double dbRatio = 0.5;
    //�������²������֮�ȵĵ���
    //
    dbRatio = (double)m_oUpperHalfMaskFrame.Statistic(0xFF)/(double)m_oLowerHalfMaskFrame.Statistic(0xFF);
    //

    //��ʼ��У��ͼ��
    m_oCalibratePattern.InitPattern(this->m_ePattern, patternSize, dbRatio);


    //����ͼ���е�У��������ߴ�
    m_vecInteriorCalibPoint.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());

    //����У����ͼ�����굽��Ļ�����ӳ������ĳߴ�
    m_vecInteriorCalibMap.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());


    return TRUE;

}

//@�� ��:�ڽǵ�������, �������Ͻǵ�ƥ���ѡ�㡣
//@�� ��:turnPoints, �ǵ�����
// ptG, ��������
// nTurnPoints, �ǵ����
//@����ֵ:�������ϽǵĽǵ������������
// -1, ��δ�ҵ����ϽǵĽǵ㡣
//@˵ ��: �ú���������DoSubAreaCornerMatch���á�
//
//����Ȧ������GΪԭ��, ��ֱ����ΪY������ϵ��,
// ��y
// |
//�D+�D��x
// |0
//����λ�����ϽǵĽǵ�, �ýǵ�������Ǵ���2,3����, ����X������н�
//��С
int CAutoCalibratorImpl2::MatchLeftTopCorner(const TurnPoint* turnPoints, POINT& ptG, int nTurnPointNumber)
{
    int iMatchIndex = -1;

    //ע�⵽Tangent������[1/2��, 3/2��]֮�䵥������,������Ҫѡȡ��[[1/2��, 3/2��]֮��
    //Tangentֵ��С�Ľǵ�
    double dbMinTan = (std::numeric_limits<double>::max)();

    for(int i=0; i<nTurnPointNumber; i++)
    {
        int nX, nY;

        nX = turnPoints[i].ptCoord.x - ptG.x;
        nY = -(turnPoints[i].ptCoord.y - ptG.y);//ȡ��,�ô�ֱ����Ϊ������

        if(nX>=0) continue;


        if(iMatchIndex == -1)
        {
            if(nX == 0)
            {
                if(nY> 0)
                {
                    dbMinTan = (std::numeric_limits<double>::min)();
                }
                else
                {
                    dbMinTan = (std::numeric_limits<double>::max)();
                }
            }
            else
            {
                dbMinTan = (double)nY/(double)nX;
            }

            iMatchIndex = i;
        }
        else
        {
            if(nX == 0)
            {
                if(nY>0)
                {
                    dbMinTan = (std::numeric_limits<double>::min)();
                    iMatchIndex = i;
                }
            }
            else
            {
                double dbTan = (double)nY/(double)nX;
                if(dbMinTan > dbTan)
                {
                    dbMinTan = dbTan;
                    iMatchIndex = i;
                }
            }
        }
    }

    return iMatchIndex;
}

//@����:��һȦ���в��ҹյ�
//@����:pPts, Ȧ�ϵ������,�Ѱ��շ�����������
//      nPtNumber, Ȧ�ϵ����Ŀ
//      pTurnpoint,����յ������
//      nExpectedNum, �յ������Ԫ�ظ���
//@����:�յ���Ŀ
int CAutoCalibratorImpl2::SearchTurnPoints(const POINT* pPts, int nPtNumber, TurnPoint* pTurnpoint, int nExpectedNum)
{

    int nTurnPointCount = 0;
    //������:"�Ե�ǰ��Ϊ�յ�"��������"�Ե�ǰ��Ϊʼ��"������֮��ļн����ж��Ƿ�Ϊ�յ�
    //����ǰ��Ϊ�յ㣬��ʸ���нǽϴ�
    for(int i=0; i < nPtNumber; i++)
    {
        int iPrevIndex, iNextIndex;

        if(i == 0)
        {
            iPrevIndex = nPtNumber - 1;
        }
        else
        {
            iPrevIndex = i-1;
        }

        if(i == nPtNumber - 1)
        {
            iNextIndex = 0 ;
        }
        else
        {
            iNextIndex = i + 1;
        }

        POINT ptCurrent = pPts[i];
        POINT ptPrev    = pPts[iPrevIndex];
        POINT ptNext    = pPts[iNextIndex];


        long v1_dx, v1_dy, v2_dx, v2_dy;
        v1_dx = ptCurrent.x - ptPrev.x ;
        v1_dy = ptCurrent.y - ptPrev.y;


        v2_dx = ptNext.x - ptCurrent.x;
        v2_dy = ptNext.y - ptCurrent.y;


        //����ʸ���Ĳ��
        long cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

        //����ʸ���ĵ��
        long dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

        //V1��V2 = |V1||V2|sin��
        //=>sin��^2 = (V1��V2)^2 /[(|V1||V2|)^2]
        //ע��˴���ֹ���ͼ����������,2013/04/11
        long v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
        long v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;


        double sin_v1_v2_square = (double(cross_product)/double(v1_length_sqare))*(double(cross_product)/double(v2_length_sqare));
        const double sin_30_square = 0.5*0.5;

        //��ʸ���н���[330, 360]��[0,30]֮��ʱ, ����Ϊ���ڱ��ϵ�У����,���ǽ��ϵĵ㡣
        if( dot_product > 0 //��֤�ڵ�һ��������
            &&
            sin_v1_v2_square < sin_30_square)
        {
            //Ϊ�ڱ��ϵ�У����
        }
        else
        {
            if(nTurnPointCount < nExpectedNum)
            {
                //��ֹ���쳣������ڴ����Խ��
                pTurnpoint[nTurnPointCount].ptCoord = pPts[i];
                pTurnpoint[nTurnPointCount].nLayerPtIndex = i;
            }
    
            nTurnPointCount++;
        }

    }//for(i)

    return nTurnPointCount;
}

//@����:���Զ�У����ͶιͼƬ��������֮
//@����ֵ:�쳣����FALSE
BOOL CAutoCalibratorImpl2::FeedImage(const CYUY2Frame* pYUVFrame)
{

    if(pYUVFrame == NULL) return FALSE;


    if(FALSE == this->m_bIsSimulatedCalibrating)
    {
        if(m_bRecordVideo)////////�������ñ�����Ƶ
        {
            m_oAVIRecorder.Write((const BYTE*)pYUVFrame->GetData(), pYUVFrame->Size());
        }
    }


    CImageFrame monoFrame;

    //Covert2MonochromeFrame(*pYUVFrame, this->m_eMonochromizeAlog, monoFrame);
    Covert2MonochromeFrame_Y(*pYUVFrame, monoFrame);

    if(this->m_bIsSimulatedCalibrating)
    {
        m_oDebugWindow.PrintFrame(monoFrame);
    }


    BOOL bFailed = FALSE;


    ////static short s_BoardBrightness = 0;//��Ļ�װ�����
    ////static short s_BInc            = 0;//��Ļ�������� inc=int[a/b] + float[a%b/b]
    ////static short s_BIncFraction    = 0;//��Ļ��������С�����ֵ�������ʾ����������С�����ֳ��Գ������������������
    ////static short s_BFractionCount  = 0;//��Ļ��������С�����ֵ�������ʾ��ÿ���ۼӺ͡�

    //static int  s_UpperPartPixelCount = 0;//�ϰ벿�����ظ���
    //static int  s_LowerPartPixelCount = 0;//�°벿�����ظ���
    //static int  s_OuterMostMarkerIndex = 0;//


    switch(m_eCalibrateStage)
    {
    case E_AUTO_CALIBRATE_START://��ʼУ��
        if(FALSE == this->m_bIsSimulatedCalibrating)
        {
            if(m_fpChangeCameraParams)
            {
                m_fpChangeCameraParams(E_CAMERA_PARAMS_CHANGE_SEARCH_SCREEN_AREA, m_lpCtx);
            }
        }

        InitBorderWhiteSquarePositions(m_AutoCalibrateWnd, HORZ_SIDE_SQUARE_NUMBER, VERT_SIDE_SQUARE_NUMBER, SQUARE_SIZE);//��ʼ��
        
        
        //������Ļ������״̬��
        ResetSubStateMachine_SearachScreenArea(monoFrame);

        
       //<<debug
        m_oDebugBorderSquareFrame.SetSize(monoFrame.Width(), monoFrame.Height(), 1);
        m_oDebugBorderSquareFrame.Clear();
      //debug>>

        m_nStageWaitCount = 0;
        //m_nStageTryTimes = 0;

        m_eCalibrateStage = E_SEARCH_SCREEN_UPPER_MASK_AREA;
        m_bCalibrateSequenceComplete = FALSE;
        break;


    case E_SEARCH_SCREEN_UPPER_MASK_AREA://�����ϲ���Ļ����
        {
            ESubStateMachineStatus result = RunSubStateMachine_SearchScreenArea(monoFrame, E_SEARCH_SCREEN_UPPER);
            if(result == E_SUB_STATE_MACHINE_FINISHED)
            { 

                //������Ļ������״̬��
                ResetSubStateMachine_SearachScreenArea(monoFrame);



                //ת�������°벿��Ļ����׶�
                m_eCalibrateStage =E_SEARCH_SCREEN_LOWER_MASK_AREA;


            }
            else if(result == E_SUB_STATE_MACHINE_FAILED)//ʧ��ת�˳�
            {
                //�ɹ���У�����
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;

            }
        }
        break;



    case E_SEARCH_SCREEN_LOWER_MASK_AREA://�����²���Ļ����
        {
            ESubStateMachineStatus result = RunSubStateMachine_SearchScreenArea(monoFrame, E_SEARCH_SCREEN_LOWER);
            if(result == E_SUB_STATE_MACHINE_FINISHED)
            { 

                BOOL bRet = OnPostSearchScreenArea(monoFrame.Width(), monoFrame.Height());
                if(!bRet)//ʧ����
                {
                     //�ɹ���У�����
                      m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                     //ת��У�������׶�
                      m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                      break;
                }


                if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
                {

                    //��ӡ���

                    Debug_SaveImageFrame(m_oDebugBorderSquareFrame, _T("BorderSqaureMatchResult.bmp"));
                }


                if(FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, 0, BLACK);
                }


                m_nStageWaitCount = 0;
                //ת��У�����̸��ϲ��׶�
                m_eCalibrateStage = E_SEARCH_BACKGROUND;

            }
            else if(result == E_SUB_STATE_MACHINE_FAILED)//ʧ��ת�˳�
            {
                //�ɹ���У�����
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;

            }
        }
        break;

    case E_SEARCH_BACKGROUND://��������
        m_nStageWaitCount++;
         if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
         {

             m_oBackgroundFrame = monoFrame;
             if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
             {
                 Debug_SaveImageFrame(m_oBackgroundFrame, _T("Background.bmp"));
             }

             if(FALSE == this->m_bIsSimulatedCalibrating)
                {
                    if(m_fpChangeCameraParams)
                    {
                        m_fpChangeCameraParams(E_CAMERA_PARAMS_CHANGET_PRINT_PATTERN, m_lpCtx);
                    }

                    //�����ϰ벿��У��ͼ��
                    m_oCalibratePattern.DrawUpperPartPattern(m_AutoCalibrateWnd);
                }


            m_nStageWaitCount = 0;

            m_eCalibrateStage = E_CALIBRATE_UPPER_CHESSBOARD;//ת�����̸�У��

         }
         break;

    case E_CALIBRATE_UPPER_CHESSBOARD://У�����̸��ϲ�
        m_nStageWaitCount++;

        if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        {

            //m_oUpperSrcFrame = monoFrame;
            PreProcessPatternImage(monoFrame, m_oScreenMaskFrame, m_oBackgroundFrame, m_oUpperSrcFrame);

            
            int nAvgGridArea  = m_oUpperHalfMaskFrame.Statistic(0xFF)/this->m_oCalibratePattern.GetUpperGridCount();
            int nDetectRadius = int(sqrt(double(nAvgGridArea))/2.0);

            //У���ϰ벿
            BOOL bRet = SubAreaCornerDetect(
                this->m_oCalibratePattern.GetUpperPartMarkCount(),
                this->m_oScreenMaskFrame,
                this->m_oUpperSrcFrame,
                m_dbRotationMatrix,
                m_ptRoationCenter,
                nDetectRadius,
                0);


            if(!bRet)//������У��ʧ��
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                break;
            }

            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                //�����ϰ벿��У��ͼ��
                m_oCalibratePattern.DrawLowerPartPattern(m_AutoCalibrateWnd);
            }


            m_nStageWaitCount = 0;
            m_eCalibrateStage = E_CALIBRATE_LOWER_CHESSBOARD;//ת������У��


        }//if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)

        break;



    case E_CALIBRATE_LOWER_CHESSBOARD://У�����̸��²�
        m_nStageWaitCount++;

        if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        {

            m_oLowerSrcFrame = monoFrame;
           // PreProcessPatternImage(monoFrame, this->m_oScreenMaskFrame, m_oBackgroundFrame, m_oLowerPatternFrame);

            //�������
            //if(this->m_bIsSimulatedCalibrating)
            //{
            //    m_oDebugWindow.PrintFrame(m_oLowerPatternFrame);
            //}

            int nAvgGridArea  = m_oLowerHalfMaskFrame.Statistic(0xFF)/this->m_oCalibratePattern.GetUpperGridCount();
            int nDetectRadius = int(sqrt(double(nAvgGridArea))/3.0);

            //У���°벿
            BOOL bRet = SubAreaCornerDetect(
                this->m_oCalibratePattern.GetLowerPartMarkCount(),
                this->m_oScreenMaskFrame,
                this->m_oLowerSrcFrame,
                m_dbRotationMatrix,
                m_ptRoationCenter,
                nDetectRadius,
                0);


            if(!bRet)//������У��ʧ��
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                break;
            }



            //������תʸ��
            //<<Debug
            CImageFrame outputFrame;
            outputFrame = m_oUpperSrcFrame;
            outputFrame.Merge(m_oLowerSrcFrame);
            outputFrame.Line(m_ptUpperCentroid, m_ptLowerCentroid, 255);
            outputFrame.DrawArrow(m_ptUpperCentroid, m_ptLowerCentroid, 30.0, 10, 255);

            CDib dibResult;
            LoadToDib(outputFrame, dibResult);

            for(std::vector<POINT>::size_type i=0; i<m_vecInteriorCalibPoint.size(); i++)
            {
                POINT pt = m_vecInteriorCalibPoint[i].ptImageCoord;
                dibResult.SetPixel(pt.x, pt.y, RGB(255,0,0));
            }

            CAtlString strFileName;
            strFileName.Format(_T("corner_match_result.bmp"));
            Debug_SaveDib(dibResult, strFileName);
            //

            std::vector<int> vecImage2ScreenMap;//"��⵽��У����"��"��ʾ��У������"��ӳ���ϵ��

            //������У�������ƥ��
            BOOL bMatchSuccess = DoCornerMatch(m_vecInteriorCalibPoint, vecImage2ScreenMap);

            if(!bMatchSuccess)
            {
                //�ɹ���У�����
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
            }
            else
            {
                //���У�����
                for(std::vector<int>::size_type i = 0; i < vecImage2ScreenMap.size(); i++)
                {
                    if(vecImage2ScreenMap[i] != -1)
                    {
                        int nCornerIndex = vecImage2ScreenMap[i];

                        m_vecInteriorCalibMap[nCornerIndex].ptScreenCoord = this->m_oCalibratePattern.GetMarkCoord(nCornerIndex);

                        m_vecInteriorCalibMap[nCornerIndex].bIsMatched = TRUE;
                        m_vecInteriorCalibMap[nCornerIndex].ptImageCoord = this->m_vecInteriorCalibPoint[i].ptImageCoord;
                    }
                }


                if(this->m_bIsSimulatedCalibrating)
                {
                    std::vector<POINT> vecDbgData;
                    vecDbgData.resize(this->m_vecInteriorCalibPoint.size());
                    for(std::vector<POINT>::size_type i = 0;
                        i < vecDbgData.size();
                        i++)
                    {
                        vecDbgData[i] = m_vecInteriorCalibPoint[i].ptImageCoord;
                    }
                    this->m_oDebugWindow.PrintPtNo(vecDbgData, vecImage2ScreenMap);
                }

                //�ɹ���У�����
                m_eCalibrateError = E_AUTO_CALIBRATOR_OK;

                //ת��У�������׶�
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
            }//else


        }//if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)

        break;


    case E_AUTO_CALIBRATE_END:

        if(m_eCalibrateError == E_AUTO_CALIBRATOR_OK)
        {
            //���У��,�ɹ�
            EndCalibrating();
            m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;

        }
        else
        {
            if(m_nTryTimes == MAX_TRY_TIMES)
            {
                EndCalibrating();
                m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;
            }
            else
            {
                //������ʾУ������
                m_nTryTimes ++;//У������

                //����ͼƬ���Ŀ¼
                CString strImageDubugDir;
                strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
                SetImageDebugDir(strImageDubugDir);

                m_eCalibrateStage = E_AUTO_CALIBRATE_START;
            }

        }
        break;

    case E_AUTO_CALIBRATE_STOPPED:

        return FALSE;

    }//switch

    return TRUE;

}


//@����:������ȷ��У������Ŀ
int CAutoCalibratorImpl2::GetValidCalibratePtNumber()const
{

    int nValidCorner = 0;

    for(int i=0; i < this->m_oCalibratePattern.GetCalibrateMarkCount(); i++)
    {
        if(m_aryAllCalibMap[i].bIsMatched)
        {
            nValidCorner++;
        }

    }//foreach(i)

    return nValidCorner;
}


//@����:���У����ǵ�����
//@����:nIndex, У����ǵ��������
const TCalibCoordMap* CAutoCalibratorImpl2::GetCalibrateMarkerCoord(int nIndex) const
{
    const TCalibCoordMap* pCellCoord = NULL;

    int nSearchCount = 0;

    for(int i=0; i < this->m_oCalibratePattern.GetCalibrateMarkCount(); i++)
    {
        if(m_aryAllCalibMap[i].bIsMatched)
        {
            nSearchCount++;

            if(nSearchCount == nIndex+1)
            {
                pCellCoord = &m_aryAllCalibMap[i];
                break;
            }
        }

    }//foreach(i)

    return pCellCoord;
}




//@����:����У��������
HWND CAutoCalibratorImpl2::GetCalibrateHWnd()const
{
    return (HWND)m_AutoCalibrateWnd;
}


CAtlString  CAutoCalibratorImpl2::GetCalibrateIntermediataDirName()
{
    return g_strImageDebugRoot;
}



//@����:������У��
//@����:
//     nExpectedMarkerNumber,�����ҵ���У�������Ŀ
//     maskFrame, ����ͼƬ
//     srcFrame, У��ͼƬ
//     roateMatrix, ��У��ͼ����ת��ˮƽ��ֱʱʹ�õ���ת����ϵ��
//     ptCenter, ��ת���ĵ�ͼ������
//     nDetectRadius, У����������İ뾶
//    nAreaNo,������
//@˵��:�ҵ��Ľǵ��ŵ�����m_vecInteriorCalibPoint��
BOOL CAutoCalibratorImpl2::SubAreaCornerDetect(
    int nExpectedMarkerNumber,
    const CImageFrame& maskFrame,
    const CImageFrame& srcFrame,
    const double m[2][2],
    const POINT& ptCenter,
    int nDetectRadius,
    int nAreaNo)
{
    BOOL bRet = TRUE;

    //ǿ³���Խǵ���
    //CRobustCornerDetector cornerDetector;

    //bRet = cornerDetector(srcFrame, maskFrame, nExpectedMarkerNumber, nDetectRadius, this->m_eDebugLevel, m_nTryTimes);

    CImageFrame rawImage = srcFrame;
    rawImage &= maskFrame;


    CFastCornerDetector fastCornerDetector;
     bRet = fastCornerDetector(
         rawImage.GetData(),
         rawImage.Width(),
         rawImage.Height(),
         nExpectedMarkerNumber, /*nDetectRadius,*/
         this->m_eDebugLevel, 
          m_nTryTimes);

    if(bRet)
    {
        //ע��:�������⵽��У���������Ѿ����Ŵ�2^INT_SCALE_SHIFT_SIZE����
        //
        //const std::vector<POINT>& vecGs = cornerDetector.GetCentroids();
        const std::vector<POINT>& vecGs = fastCornerDetector.GetResults();

        std::vector<TImageCalibPoint> cornerData;
        cornerData.resize(vecGs.size());

        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < vecGs.size(); i++)
        {
            cornerData[i].ptImageCoord = vecGs[i];

        }

        //
        // x |c11 c12 |
        // = | |
        // y |c21 c22 |

        //��ת����ϵ��
        double c11, c12, c21, c22;

        c11 = m[0][0];
        c12 = m[0][1];

        c21 = m[1][0];
        c22 = m[1][1];




        //������ת����
        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < cornerData.size();
            i++)
        {
            const POINT& ptImageCoord = cornerData[i].ptImageCoord;
            POINT& ptRotatedCoord = cornerData[i].ptRotatedCoord;
            double dx = ptImageCoord.x - ptCenter.x;
            double dy = ptImageCoord.y - ptCenter.y;
            ptRotatedCoord.x = LONG(c11 * dx + c12 * dy);
            ptRotatedCoord.y = LONG(c21 * dx + c22 * dy);

            ptRotatedCoord.x += ptCenter.x;
            ptRotatedCoord.y += ptCenter.y;
        }


        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < cornerData.size();
            i++)
        {
            m_vecInteriorCalibPoint[m_vecInteriorCalibPtNumber++] = cornerData[i];
        }

        //<<debug
        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {

            CImageFrame rotateCornersFrame(srcFrame.Width(), srcFrame.Height());

            for(std::vector<TImageCalibPoint>::size_type i = 0;
                i < cornerData.size();
                i++)
            {
                //<<//<<modified by toxuke@gmail.com, 2013/04/11��
                POINT ptOutput;
                ptOutput.x = cornerData[i].ptRotatedCoord.x >> INT_SCALE_SHIFT_SIZE;
                ptOutput.y = cornerData[i].ptRotatedCoord.y >> INT_SCALE_SHIFT_SIZE;
                //>>

                rotateCornersFrame.SetPixel(ptOutput, 255);
            }

            CDib dib;
            LoadToDib(rotateCornersFrame, dib);
            dib.SetPixel(ptCenter.x>>INT_SCALE_SHIFT_SIZE, ptCenter.y>>INT_SCALE_SHIFT_SIZE, RGB(255, 0, 0));
            CAtlString strFileName;
            strFileName.Format(_T("corner_point_rotate_adjust_%d.bmp"),nAreaNo);
            Debug_SaveDib(dib, strFileName);
        }
        //debug>>

    }

    return bRet;
}


//@����:ͼ���нǵ�����Ļ��У�����ƥ��
//@����:
// corners, ��֪��Ƶͼ���еĵĽǵ���������
// vecImage2ScreenMap, ÿ����Ƶͼ���еĽǵ㵽��ĻУ�����ŵ�ӳ�����顣
BOOL CAutoCalibratorImpl2::DoCornerMatch(const std::vector<TImageCalibPoint>& corners, std::vector<int>& vecImage2ScreenMap)
{
    BOOL bMatchSuccess = TRUE;

    std::vector<TVertex> vecVertex;

    //ʣ���У����ĸ���
    unsigned int nCornerRemained = corners.size();

    vecVertex.resize(nCornerRemained);
    vecImage2ScreenMap.resize(nCornerRemained);

    for(std::vector<TVertex>::size_type i = 0;
        i < nCornerRemained;
        i++)
    {
        vecVertex[i].ptCoord = corners[i].ptRotatedCoord;
        vecVertex[i].nAryIndex = i;
        vecImage2ScreenMap[i] = - 1;
    }


    int nLayer = 0;//��һȦ,������Ȧ��ʼ����



    int nLayerCount = m_oCalibratePattern.GetLayerCount();
    //��Ȧ���㴦��
    for(nLayer = 0; nLayer < nLayerCount; nLayer++)
    {

        const TRectLayer& rectLayer = m_oCalibratePattern.GetLayer(nLayer);

        //����ˮƽ����, У����ǵĸ���
        int LayerMarkerHorzNumber = rectLayer.m_nHorzMarkNumber;

        //���ڴ�ֱ����, У����ǵĸ���
        int LayerMarkerVertNumber = rectLayer.m_nVertMarkNumber;

        //����������ȦУ�������
        int nExpectdPtNumber = rectLayer.m_vecScreenMarkNo.size() ;


        //�����һ��Ȧ��
        if(LayerMarkerHorzNumber >1 && LayerMarkerVertNumber > 1)
        {
            //Step 1. ��ÿ��У����, �ҳ����������������16���ڽӵ㡣
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                POINT P0 = curVertex.ptCoord;

                //�ͳ��������������У������бȽ�
                for(std::vector<TVertex>::size_type j = 0;
                    j < nCornerRemained;
                    j++)
                {

                    if(i == j) continue;//�����Լ��Ƚ�

                    const TVertex& cmpVertex = vecVertex[j];
                    //if(!cmpVertex.bValid) continue;

                    POINT P1 = cmpVertex.ptCoord;
                    double distance = (P1.x - P0.x)*(P1.x - P0.x) + (P1.y - P0.y)*(P1.y - P0.y);


                    //�뵱ǰ�ҵ�������ڽӵ�Ƚ�
                    for(int k = 0; k < _countof(curVertex.aryNeighbours); k ++)
                    {

                        //��һ���ҵ���k������ڽӵ�
                        if(!curVertex.aryNeighbours[k].bValid)
                        {
                            //������С�ڽӱ�
                            curVertex.aryNeighbours[k].bValid = TRUE;
                            curVertex.aryNeighbours[k].distance = distance;
                            curVertex.aryNeighbours[k].nIndex = j;
                            curVertex.nNeighbourNumber++;
                            break;//break k loop
                        }
                        else
                        {
                            double distCmp = curVertex.aryNeighbours[k].distance;
                            if(distance < distCmp)//�����ҵ�������ڽӵ�ľ��뻹ҪС�������
                            {

                                //ǰ��ĵ�Ԫ������һ��,��֤����С���ڽӵ�
                                for(int l = _countof(curVertex.aryNeighbours) - 1; l > k; l--)
                                {
                                    curVertex.aryNeighbours[l] = curVertex.aryNeighbours[l-1];
                                }

                                curVertex.aryNeighbours[k].distance = distance;
                                curVertex.aryNeighbours[k].nIndex = j;

                                if(curVertex.nNeighbourNumber < _countof(curVertex.aryNeighbours))
                                {
                                    curVertex.nNeighbourNumber++;
                                }

                                break;//break k loop
                            }//if
                            else
                            {
                                continue;//�����Ƚ�
                            }

                        }//else

                    }//for(k)

                }//for(j)

            }//for(i)



            //Step 2. ��ÿ��У����ļ���"����ڽ�ʸ��"���鰴ʸ���ķ�λ������
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                curVertex.bIsOuterLayer = FALSE;//�������Ȧ�ǵ��־

                int candidateNumber = curVertex.nNeighbourNumber ;
                POINT p0 = curVertex.ptCoord;
                UINT uQuadrant = 0;

                //��i��У�����MAX_NEIBHBOUR��"����ڽ�ʸ��"
                TNeighbourVector nvs[MAX_NEIBHBOUR];
                memset(nvs, 0 ,sizeof(nvs));


                for(int j = 0; j < candidateNumber; j++)
                {
                    if( !curVertex.aryNeighbours[j].bValid) continue;

                    int index = curVertex.aryNeighbours[j].nIndex;//�ڽӵ�ı��
                    POINT p1 = vecVertex[index].ptCoord;

                    //��i��У����ĵ�j���ڽ�ʸ��
                    TNeighbourVector nv;
                    nv.lDx = p1.x - p0.x;
                    nv.lDy = p1.y - p0.y;
                    nv.nVertexIndex = index;


                    if(nv.lDx > 0 && nv.lDy > 0 )
                    {
                        nv.nQaudrant = 0;
                    }
                    else if(nv.lDx<0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if(nv.lDx == 0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if(nv.lDx <0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 2;
                    }
                    else if(nv.lDx == 0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 3;
                    }
                    else if(nv.lDy == 0 && nv.lDx > 0 )
                    {
                        nv.nQaudrant = 0;
                    }
                    else if(nv.lDy == 0 && nv.lDx < 0)
                    {
                        nv.nQaudrant = 2;
                    }
                    else
                    {
                        nv.nQaudrant = 3;
                    }



                    BOOL bInsertPosFound = FALSE;//����λ���ҵ���־

                    int k = 0;

                    //���ò��������㷨
                    //���Ҳ���λ��
                    for(k = 0; k < j; k++)
                    {

                        //�Ƚ����е�ʸ���ĽǶ�,ʹ������ʸ������С����ķ�λ������
                        TNeighbourVector nvtemp = nvs[k];

                        if(nv.nQaudrant < nvtemp.nQaudrant)
                        {
                            bInsertPosFound = TRUE;
                            break;
                        }
                        else if(nv.nQaudrant == nvtemp.nQaudrant)
                        {
                            LONG crossProduct = nvtemp.lDx * nv.lDy - nvtemp.lDy*nv.lDx ;
                            if(crossProduct < 0 )//��ԭʸ���ļн�>0, ��ʸ����>0
                            {
                                bInsertPosFound = TRUE;
                                break;
                            }

                        }//else

                    }//for(k)


                    //����ʸ������Ų��һ����Ԫ
                    if(bInsertPosFound)
                    {
                        for(int l=j;l >k; l--)
                        {
                            nvs[l] = nvs[l-1];
                        }
                        nvs[k] = nv;
                    }
                    else
                    {
                        nvs[j] = nv;//���뵽��ĩ
                    }
                }//for(j)

                //Step 3.���Ѱ��շ�λ�����е�"����ڽ�ʸ��",�ж���������ʸ��֮��ļн��Ƿ����90��,
                // �������90��, ����Ϊ����������Ȧ��У����
                for(int j = 0;j < candidateNumber;j ++)
                {
                    TNeighbourVector v1;
                    TNeighbourVector v2;

                    if(j == candidateNumber - 1)
                    {
                        v1 = nvs[j];
                        v2 = nvs[0];
                    }
                    else
                    {
                        v1 = nvs[j];
                        v2 = nvs[j+1];

                    }

                    //ʸ���ġ���
                    //v1��v2=|v1||v2|sin(v1^v2);
                    long crossProduct = v1.lDx * v2.lDy - v1.lDy * v2.lDx;

                    //ʸ����.��
                    //V1.V2 = |v1||v2|cos(v1^v2);
                    long dotProduct = v1.lDx * v2.lDx + v1.lDy * v2.lDy;

                    long v1_length_sqare = v1.lDx * v1.lDx + v1.lDy * v1.lDy;
                    long v2_length_sqare = v2.lDx * v2.lDx + v2.lDy * v2.lDy;


                    //double cosV1_V2_square = double(dotProduct*dotProduct)/(double)(v1_length_sqare*v2_length_sqare);
                    double cosV1_V2_square = (double(dotProduct)/double(v1_length_sqare)) * (double(dotProduct)/double(v2_length_sqare));
                    double cos145_squre = 0.67101007166283436652204980734113;


                    if(crossProduct<0)//sin(v1^v2)<0, ˵���нǷ�Χ��180~360��֮��,������ȦУ����
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }
                    else if(dotProduct < 0 && cosV1_V2_square > cos145_squre)//cos(v1^v2)<0, ˵����Χ��145~180��֮��, ������ȦУ����
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }

                }//for(j)

            }//for(i)


            //ͳ���ҵ�������Ȧ�ǵ�ĸ���
            int nOuterLayerPtNumber = 0;

            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if(curVertex.bIsOuterLayer)
                {
                    nOuterLayerPtNumber ++;
                }

            }//for_each(i)





            //��������ҵ���һȦУ����
            //<<debug
            if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
            {
                CAtlString strFileName;
                strFileName.Format(_T("Layer%02d_CornerPoints.bmp"), nLayer );


                //CImageFrame frameOutLayer(640, 480);
                CImageFrame frameOutLayer(m_oScreenMaskFrame.Width(), m_oScreenMaskFrame.Height());
                for(std::vector<TVertex>::size_type i = 0;
                    i < nCornerRemained;
                    i++)
                {
                    TVertex& curVertex = vecVertex[i];

                    if(!curVertex.bIsOuterLayer) continue;

                    POINT ptStart = curVertex.ptCoord;

                    ptStart.x >>= INT_SCALE_SHIFT_SIZE;
                    ptStart.y >>= INT_SCALE_SHIFT_SIZE;

                    frameOutLayer.SetPixel(ptStart, 255);
                }

                Debug_SaveImageFrame(frameOutLayer, strFileName);
            }
            //debug>>


            //Ȧ���ʧ��
            if(nOuterLayerPtNumber != nExpectdPtNumber)
            {

                bMatchSuccess = FALSE;
                break;
            }


            //��ȡ����ȦУ���㵽������һ��������,���ٺ��������ѭ������
            std::vector<TVertex> outlayer;
            outlayer.resize(nOuterLayerPtNumber);


            //У������"����У����ļ���"�е������ŵ�"����ȦУ���㼯��"������ӳ��
            std::vector<int> mapForLayerNeighbourIndex;
            mapForLayerNeighbourIndex.resize(nCornerRemained);

            for(unsigned int i=0; i < nCornerRemained; i++)
            {
                mapForLayerNeighbourIndex[i] = -1;//-1��ʾ������ӳ���Ӧ��ϵ
            }

            int nOutLayerPtIndex = 0;
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if(curVertex.bIsOuterLayer)
                {
                    outlayer[nOutLayerPtIndex] = curVertex;

                    mapForLayerNeighbourIndex[i] = nOutLayerPtIndex;
                    nOutLayerPtIndex++;
                }

            }//for_each(i)


            //��������ȦУ����"����ڽӵ�"��������Ϊ����ȦУ���������е������ţ� ��ֻ����������Ȧ��"����ڽӵ�", �Ա��ܹ������˳ʱ�������ȦУ����
            for(unsigned int i=0; i< outlayer.size(); i++)
            {
                unsigned int nNeigbours = outlayer[i].nNeighbourNumber;
                TVertex& curVertex = outlayer[i];
                unsigned int nLayerNeigbourCount = 0;
                for(unsigned int j=0; j<nNeigbours; j++)
                {
                    int nOldNeighbourIndex = curVertex.aryNeighbours[j].nIndex;

                    if(mapForLayerNeighbourIndex[nOldNeighbourIndex] == -1)
                    {
                        curVertex.nNeighbourNumber -- ;
                        curVertex.aryNeighbours[j].bValid = FALSE;
                    }
                    else
                    {

                        //ע�⵽ʼ���� j>= nLayerNeigbourCount
                        //��ԭ����j��Ԫ����ǰŲ����nLayerNeigbourCount��Ԫ�ش�,
                        //����ȷ������Ȧ�ϵ�У����������������У�������Ϣʼ�մ�����aryNeighbours�Ŀ�ͷ��š�
                        if(j != nLayerNeigbourCount)
                        {
                            curVertex.aryNeighbours[nLayerNeigbourCount] = curVertex.aryNeighbours[j];
                            //����Ԫ��j
                            curVertex.aryNeighbours[j].bValid = FALSE;
                        }

                        //�µ��ڽӵ��indexָ��������Ȧ�����е�������
                        curVertex.aryNeighbours[nLayerNeigbourCount].nIndex = mapForLayerNeighbourIndex[nOldNeighbourIndex];
                        nLayerNeigbourCount++;
                    }

                }//fo(j)

                //assert(curVertex.nNeighbourNumber == 2);
            }//for(i)



            BOOL bLayerMatchSuccess = FALSE;

            //��������Ȧ�ҵ���У����, ����Ȧ������
            POINT ptG;

            long lMx = 0;//
            long lMy = 0;//
            long lM = 0;//
            for(std::vector<TVertex>::size_type i = 0; i< outlayer.size(); i++)
            {
                lMx += outlayer[i].ptCoord.x;
                lMy += outlayer[i].ptCoord.y;
                lM ++;
            }//for(i)

            ptG.x = lMx/lM;//
            ptG.y = lMy/lM;//


            //Ȧ������GΪԭ��,
            //���������G��Ȧ��У�����ʸ���ķ�λ��,
            //��ʸ�����շ�λ�ǵĴ�С����
            //���ò��뷨����
            std::vector<TNeighbourVector> layerVector;
            layerVector.resize(outlayer.size());

            for(std::vector<TVertex>::size_type i = 0; i< outlayer.size(); i++)
            {
                const POINT& ptVertex = outlayer[i].ptCoord;
                TNeighbourVector nv;
                nv.lDx = ptVertex.x - ptG.x;
                nv.lDy = ptVertex.y - ptG.y;
                nv.nVertexIndex = i;

                //���㵱ǰʸ����������
                if(nv.lDx > 0 && nv.lDy > 0 )
                {
                    nv.nQaudrant = 0;
                }
                else if(nv.lDx<0 && nv.lDy > 0)
                {
                    nv.nQaudrant = 1;
                }
                else if(nv.lDx == 0 && nv.lDy > 0)
                {
                    nv.nQaudrant = 1;
                }
                else if(nv.lDx <0 && nv.lDy < 0)
                {
                    nv.nQaudrant = 2;
                }
                else if(nv.lDx == 0 && nv.lDy < 0)
                {
                    nv.nQaudrant = 3;
                }
                else if(nv.lDy == 0 && nv.lDx > 0 )
                {
                    nv.nQaudrant = 0;
                }
                else if(nv.lDy == 0 && nv.lDx < 0)
                {
                    nv.nQaudrant = 2;
                }
                else
                {
                    nv.nQaudrant = 3;
                }

                BOOL bInsertPosFound = FALSE;//����λ���ҵ���־

                unsigned int k = 0;

                //���ò��������㷨
                //���Ҳ���λ��
                for(k = 0; k < i; k++)
                {

                    //�Ƚ����е�ʸ���ĽǶ�,ʹ������ʸ������С����ķ�λ������
                    TNeighbourVector nvCmp = layerVector[k];

                    if(nv.nQaudrant < nvCmp.nQaudrant)
                    {
                        bInsertPosFound = TRUE;
                        break;
                    }
                    else if(nv.nQaudrant == nvCmp.nQaudrant)//ʸ����λ��ͬһ������
                    {
                        LONG crossProduct = nvCmp.lDx * nv.lDy - nvCmp.lDy*nv.lDx ;
                        if(crossProduct < 0 )//��ԭʸ���ļн�>0, ��ʸ����>0
                        {
                            bInsertPosFound = TRUE;
                            break;
                        }

                    }//else

                }//for(k)


                //��Ҫ����,����ʸ������Ų��һ����Ԫ
                if(bInsertPosFound)
                {
                    for(unsigned int l=i;l >k; l--)
                    {
                        layerVector[l] = layerVector[l-1];
                    }

                    layerVector[k] = nv;
                }
                else
                {
                    layerVector[i] = nv;//���뵽��ĩ
                }


            }//for(i)


            //��ȦУ�����Ѿ�����λ����������,������Ҫ��һȦУ�������ҳ��ĸ����ϵĵ㡣
            //�ڽ��ϵ�У������ص���:ǰ������ʸ���ļн�>30��, ���ϵ�У���������ʸ���ļн�С��30�ȡ�
            TurnPoint turns[4];
            int nTurnPointCount = 0;
            for(unsigned int i=0; i < layerVector.size(); i++)
            {
                int iPrevIndex, iNextIndex, iCurrentIndex;

                if(i == 0)
                {
                    iPrevIndex = layerVector[layerVector.size() - 1].nVertexIndex;
                }
                else
                {
                    iPrevIndex = layerVector[i - 1].nVertexIndex;
                }

                if(i == layerVector.size() - 1)
                {
                    iNextIndex = layerVector[0].nVertexIndex;
                }
                else
                {
                    iNextIndex = layerVector[i + 1].nVertexIndex;
                }

                iCurrentIndex = layerVector[i].nVertexIndex;


                POINT ptCurrent = outlayer[iCurrentIndex].ptCoord;
                POINT ptPrev = outlayer[iPrevIndex].ptCoord;
                POINT ptNext = outlayer[iNextIndex].ptCoord;


                long v1_dx, v1_dy, v2_dx, v2_dy;
                v1_dx = ptCurrent.x - ptPrev.x ;
                v1_dy = ptCurrent.y - ptPrev.y;

                ;

                v2_dx = ptNext.x - ptCurrent.x;
                v2_dy = ptNext.y - ptCurrent.y;

                //
                long cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

                long dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

                //V1��V2 = |V1||V2|sin��
                //=>sin��^2 = (V1��V2)^2 /[(|V1||V2|)^2]
                //ע��˴���ֹ���ͼ����������,2013/04/11
                long v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
                long v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;

                //double sin_v1_v2_square = double(cross_product * cross_product)/double(v1_dx * v1_dx + v1_dy * v1_dy)/double(v2_dx * v2_dx + v2_dy * v2_dy);
                double sin_v1_v2_square = (double(cross_product)/double(v1_length_sqare))*(double(cross_product)/double(v2_length_sqare));
                const double sin_30_square = 0.5*0.5;

                //��ʸ���н���[330, 360]��[0,30]֮��ʱ, ����Ϊ���ڱ��ϵ�У����,���ǽ��ϵĵ㡣
                if( dot_product > 0 //��֤�ڵ�һ��������
                    &&
                    sin_v1_v2_square < sin_30_square)
                {
                    //Ϊ�ڱ��ϵ�У����
                }
                else
                {
                    if(nTurnPointCount < _countof(turns))
                    {
                        //��ֹ���쳣������ڴ����Խ��
                        turns[nTurnPointCount].ptCoord = outlayer[iCurrentIndex].ptCoord;
                        turns[nTurnPointCount].nLayerPtIndex = iCurrentIndex;
                    }

                    nTurnPointCount++;
                }

            }//for(i)


            if(nTurnPointCount < 4 || nTurnPointCount > 4)
            {
                bMatchSuccess = FALSE;
                break;
            }


            int iTopLeftCornerPointIndex = MatchLeftTopCorner(turns, ptG, _countof(turns));


            if(iTopLeftCornerPointIndex == -1)
            {
                bMatchSuccess = FALSE;
                break;
            }

            int iStartMatchIndex = 0;


            for(unsigned int i = 0; i< layerVector.size(); i++)
            {

                if(turns[iTopLeftCornerPointIndex].nLayerPtIndex == layerVector[i].nVertexIndex)
                {
                    iStartMatchIndex = i;
                    bLayerMatchSuccess = TRUE;
                    break;
                }
            }//for

            if(!bLayerMatchSuccess)
            {
                bMatchSuccess = FALSE;
                break;
            }



            unsigned int nMatchCount = 0;
            int iMatchIndex = iStartMatchIndex;

            //У������Ļ���
            //int nScreenCheckerIndex = nLayserFirstCheckerScreenIndex;
            //nLayserFirstCheckerScreenIndex += (MarkerNumberEachRow + 1);


            //��Ļ��ŵ�����
            //int nScreenCheckerIndexInc = 1;

            while(nMatchCount < layerVector.size())
            {
                const TVertex& v = outlayer[layerVector[iMatchIndex].nVertexIndex];


                vecImage2ScreenMap[v.nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount];//subarea.m_aryMarkScreenGlobalNo[nScreenCheckerIndex];


                iMatchIndex ++;

                if(iMatchIndex == layerVector.size())
                {
                    iMatchIndex = 0;
                }

                nMatchCount ++;
            }


            //�޳�����Ȧ��Ϊ��һȦ������׼��
            std::vector<TVertex>::iterator it = vecVertex.begin();

            while(it != vecVertex.end())
            {
                if(it->bIsOuterLayer)
                {
                    it = vecVertex.erase(it);
                    //it->bValid = FALSE;
                    nCornerRemained --;
                }
                else
                {
                    it->nNeighbourNumber = 0;
                    memset(it->aryNeighbours, 0, sizeof(it->aryNeighbours));
                    it->bIsOuterLayer = FALSE;
                    it->bIsTurnPoint = FALSE;
                    it++;
                }
            }


            //<<�������ȥ������ȦУ������У���㼯��
            if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
            {
                //CImageFrame nextLayerFrame(640, 480);
                CImageFrame nextLayerFrame(m_oScreenMaskFrame.Width(), m_oScreenMaskFrame.Height());
                for(std::vector<TVertex>::size_type i = 0;
                    i < vecVertex.size();
                    i++)
                {
                    TVertex& curVertex = vecVertex[i];

                    //if(!curVertex.bIsOuterLayer) continue;

                    POINT ptImage = curVertex.ptCoord;
                    ptImage.x >>= INT_SCALE_SHIFT_SIZE;
                    ptImage.y >>= INT_SCALE_SHIFT_SIZE;

                    nextLayerFrame.SetPixel(ptImage, 255);


                    //for(unsigned int k=0; k < curVertex.nNeighbourNumber; k++)
                    //{
                    //    if(!curVertex.aryNeighbours[k].bValid) continue;
                    //    int nIndex = curVertex.aryNeighbours[k].nIndex;
                    //    const POINT& ptEnd = vecVertex[nIndex].ptCoord;
                    //    //frame.Line(ptStart, ptEnd, 255);
                    //    //m_oDebugWindow.PrintFrame(nextLayerFrame);
                    //}

                    //frame.Clear();


                }

                CAtlString strFileName;
                strFileName.Format(_T("Grid_Layer%02d_Excluded.bmp"), nLayer+1);
                Debug_SaveImageFrame(nextLayerFrame, strFileName);
            }
        }//if is circle layer
        else  if(LayerMarkerHorzNumber == 1)//������Ϊһ��
        {
            if(vecVertex.size() != nExpectdPtNumber)
            {

                bMatchSuccess = FALSE;
                break;
            }


            //���մ�Y����С�����˳��ƥ��
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it,itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for(it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if(itMatch->ptCoord.y > it->ptCoord.y)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount];

                vecVertex.erase(itMatch);

            }while(vecVertex.size());

            bMatchSuccess = TRUE;

        }
        else if(LayerMarkerVertNumber == 1)//������Ϊһ��
        {
            if(vecVertex.size() != nExpectdPtNumber)
            {
                bMatchSuccess = FALSE;
                break;
            }


            ////���մ�X����С�����˳��ƥ��
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it,itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for(it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if(itMatch->ptCoord.x > it->ptCoord.x)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount++];

                vecVertex.erase(itMatch);

            }while(vecVertex.size());

            bMatchSuccess = TRUE;
        }
    }//for_each(rectLayer)

    return bMatchSuccess;
}



//@����:ģ��У��
BOOL CAutoCalibratorImpl2::DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, E_AutoCalibratePattern eGridsNumber)
{
    if(this->m_bIsCalibrating)
    {
        this->EndCalibrating();
    }

    if(!m_oAVIInput.OpenFile(lpszAVIFilePath))
    {
        return FALSE;
    }


    this->m_hNotifyWnd = hNotifyWnd;


    m_uCx = uCx;
    m_uCy = uCy;

    SYSTEMTIME now;
    GetLocalTime(&now);
    m_strCliabrateDebugDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);


    m_nTryTimes = 0;

    //����ͼƬ���Ŀ¼
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
    SetImageDebugDir(strImageDubugDir);


    //m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//�ڰ׻��㷨
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    m_nStageWaitCount = 0;
    m_bIsSimulatedCalibrating = TRUE;
    m_bIsCalibrating = TRUE;

    this->m_ePattern = eGridsNumber;

    DWORD dwThreadId = 0;
    m_hSimulatedCalibrateThread = ::CreateThread(
        NULL,
        0,
        SimulatedCalibrateProc,
        (LPVOID)this,
        0,
        &dwThreadId);


    return TRUE;

}

