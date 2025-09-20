#include "stdafx.h"
#include "CornerDetect.h"
#include "FastCornerDetector.h"
#include "ImageFrame.h"
#include "ImageDib.h"
#include "BlobDetectorImpl.h"
#include "SIMD.h"
#include "ImageDebug.h"
//@����:���ټ��ǵ��㷨
//@����:pSrcImage, ָ��Ҷ�ͼƬ���ݵ�ָ��
//      nWidth, ͼƬ���
//      nHeight, ͼƬ�߶�
bool CFastCornerDetector::operator()(
                                     const unsigned char* pSrcImage, 
                                     int nWidth, 
                                     int nHeight, 
                                     UINT uExpectedCornerNumber, 
                                     /*int nDetectRadius, */
                                     ECalibrationDebugLevel eDebugLevel, 
                                     int nDebugTimes )
{
    static CImageFrame corner;
    corner.SetSize(nWidth, nHeight, 1);
    memcpy(corner.GetData(), pSrcImage, corner.Size());
    int nThreshold =  corner.Avg();
    //nThreshold >>=1 ;//���޼���
    nThreshold =5 ;//���޹̶�

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(corner, _T("FastDetect_PatternRaw.bmp"));
    }



    corner_detect5_sse2(nWidth, nHeight, pSrcImage, corner.GetData());

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        //Debug_SaveImageFrame(corner, _T("FastDetectCorner01.bmp"), nDebugTimes);
    }


    BYTE* pCorner  = corner.GetData();

    //��ֵ��
    for(int row=0; row < nHeight; row++)
    {
        for(int col=0; col < nWidth; col++)
        {
            //ע��Ⱥ�,����Ϊ0ʱ, ���޵Ⱥ����ֵ��Ϊȫ��
            if(*pCorner <= nThreshold)
            {
                *pCorner = 0x00;
            }
            else
            {
                *pCorner = 0xFF;
            }
            pCorner++;
        }
    }

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(corner, _T("FastDetect_CornerCandidate.bmp"));
    }

    CConnectedComponentScanlineAlogrithm<> blobDetect;


    CBitFrame bitFrame;
    bitFrame.SetSize(nWidth, nHeight);


    GrayToBitFrame_SSE2((const BYTE*)corner.GetData(), (BYTE*)bitFrame.GetData(), 0xFF, nWidth*nHeight);

    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    //{
    //    SaveBitFrame(bitFrame,  _T("FastDetect_BitFrame.bmp"));
    //}

    blobDetect.ProcessImage((const BYTE*)bitFrame.GetData(), nWidth, nHeight);

    std::vector<TBlobObject> blobObjs;

    const TBlobObject* pObj = blobDetect.GetObjs();

    int nObjCount = blobDetect.GetObjCount();
    int nCandidateNum = nObjCount;

    //�޳���״���׵�Ŀ��
    for(int i= 0; i < nObjCount; i++)
    {
        //�޳�����֮�� > 2��Ŀ��
        int nWidth  = pObj[i].rcArea.right  - pObj[i].rcArea.left;
        int nHeight = pObj[i].rcArea.bottom - pObj[i].rcArea.top;


        //if(nCandidateNum > uExpectedCornerNumber)
        {
            if(nWidth == 0 || nHeight == 0)
            {
                nCandidateNum -- ;
                continue;
            }
            else if(nWidth > nHeight && nWidth*2 > nHeight*5)//W/L > 5/2
            {
                nCandidateNum -- ;
                continue;
            }
            else if(nWidth < nHeight && nHeight*2 > nWidth*5)//L/W > 5/2
            {
                nCandidateNum -- ;
                continue;
            }
        }

        blobObjs.push_back(pObj[i]);
        nCandidateNum -- ;
    }

    if(blobObjs.size() == 0)
    {
        return FALSE;
    }


    
	CHeapSortAlgorithm<TBlobObject> heapSort;


    //����Ŀ��������������и���Ŀ��, �Ա㰴��
    //������С, ����ѡȡĿ��
    heapSort(&blobObjs[0], blobObjs.size(), false);


    //<<����
    CImageFrame debugFrame;
    debugFrame = corner;
    //����>>


    if(blobObjs.size() < uExpectedCornerNumber)
    {

        return FALSE;

    }
    else
    {

        m_vecCorners.resize(uExpectedCornerNumber);

        for(UINT i=0; i<uExpectedCornerNumber; i++)
        {

            m_vecCorners[i].x =  (blobObjs[i].my + blobObjs[i].mass/2)/blobObjs[i].mass;
            m_vecCorners[i].y  = (blobObjs[i].mx + blobObjs[i].mass/2)/blobObjs[i].mass;

            //<<����
            debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
            debugFrame.SetPixel(blobObjs[i].GPos, 125);
            //����>>
        }

    }

    if(eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(debugFrame, _T("FastDetect_CornerCandidateResult.bmp"));
    }
    return TRUE;

}
