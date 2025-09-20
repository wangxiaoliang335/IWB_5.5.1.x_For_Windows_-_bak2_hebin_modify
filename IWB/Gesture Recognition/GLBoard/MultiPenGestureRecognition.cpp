#include "StdAfx.h"
#include <map>
#include "..\..\headers.h"

//////////////////////////////////////////////////////////////////////////
//��Ϣ�Ŷ��忪ʼ

/*ģ������Ϣ�ſ�ʼ*/
const UINT MyWM_PEN_BASE    = 1027                               ;
////ÿֻ�ʵ�״̬��Ϣ�Ŷ������£�����i = 0, ..., MAXPENNUM
//const UINT MyWM_PEN_I_Down   = MyWM_PEN_BASE + 2 * i           ;
//const UINT MyWM_PEN_I_UP     = MyWM_PEN_BASE + 2 * i + 1       ;
/*ģ������Ϣ�Ž���*/

/*ģ������Ϣ�ſ�ʼ*/
const UINT MyWM_ERASER_BASE    = 1127                                 ;
////ÿֻ��Ƥ����״̬��Ϣ�Ŷ������£�����i = 0, ..., MAXERASERNUM
//const UINT MyWM_ERASERA_Down      = MyWM_ERASER_BASE + 2 * i        ;
//const UINT MyWM_ERASERA_UP        = MyWM_PEN_BASE + 2 * i + 1       ;
/*ģ������Ϣ�Ž���*/

//��Ϣ�Ŷ������
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CSmartMultiPen

//////////////////////////////////////////////////////////////////////////
bool g_bHistoryEraserInfo[GLBOARDMAXERASERNUM]; //��ʷ�����Ϣ���±��Ƕ�Ӧ��id
//////////////////////////////////////////////////////////////////////////

CSmartMultiPen::CSmartMultiPen()
: m_bIsTouchPadMode(false)
 ,m_bIsIdle(true)
 ,m_hWndGLBoad(NULL)
 ,m_isSmartMatchReset(true)
{
    m_oSmartPenMatch.Reset();
}

CSmartMultiPen::~CSmartMultiPen()
{
}

void CSmartMultiPen::ResetSmartMatch()
{
    if (!m_isSmartMatchReset)
    {
        //m_oSmartPenMatch.Reset();
        DoRecognition(NULL, 0);
        m_isSmartMatchReset = true;
    }
}

void CSmartMultiPen::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    m_oStrokFilter.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);
}

bool CSmartMultiPen::DoRecognition(POINT *points, int nPointCount)
{
    if (m_bIsIdle && nPointCount == 0) return true;    

    m_oSmartPenMatch.DoMatch(points, nPointCount);
    m_isSmartMatchReset = false;

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartPenMatch.GetAllMatchInfo(&nElementCount);

    TContactInfo penInfo[GLBOARDMAXPENNUM];

    //if (m_bIsTouchPadMode)
    //{
    //    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);    

    //    for(int i = 0; i < nElementCount; i++)
    //    {
    //        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
    //        penInfo[i].uId       = pMatchInfo[i].uId;
    //        penInfo[i].pt        = pMatchInfo[i].ptPos;
    //    }  


    //    if (m_bIsTouchPadMode) CSpotListProcessor::GetVirtualHID().InputPoints(penInfo, nElementCount);   
    //}
    //else
    //{            
    //    for (int i = 0; i < nElementCount; i++)
    //    {  
    //        if (pMatchInfo[i].uId == 0)
    //        {//��uId = 0�ıʴ��ڣ��������ã��������÷�����Ϣģ��  
    //            penInfo[0].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
    //            penInfo[0].uId       = pMatchInfo[i].uId;
    //            penInfo[0].pt        = pMatchInfo[i].ptPos;

    //            CSpotListProcessor::GetVirtualHID().InputPoints(penInfo, 1);
    //        }
    //        else
    //        {
    //            UINT msg = MyWM_PEN_BASE + ((pMatchInfo[i].uId - 1) << 1); //����uId=0��Ϊ������ã�����ΪuId�ıʰ󶨵���Ϣ��ΪMyWM_PEN_BASE + 2*(uId - 1)
    //            msg += ((pMatchInfo[i].eMatchState == E_MISMATCHED)?1:0); //
    //            PostMessage(m_hWndGLBoad, msg, pMatchInfo[i].ptPos.x,  pMatchInfo[i].ptPos.y);
    //        }
    //    }
    //}

    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);  

    for (int i = 0; i < nElementCount; i++)
    {
        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
        penInfo[i].uId       = pMatchInfo[i].uId;
        penInfo[i].pt        = pMatchInfo[i].ptPos;
    }

    m_oStrokFilter.DoFilter(penInfo, nElementCount);

    for (int i = 0; i < nElementCount; i++)
    {  
        if (penInfo[i].uId == 0)
        {//��uId = 0�ıʴ��ڣ��������ã��������÷�����Ϣģ��  
            /*penInfo[0].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
            penInfo[0].uId       = pMatchInfo[i].uId;
            penInfo[0].pt        = pMatchInfo[i].ptPos;*/

            CSpotListProcessor::GetVirtualHID().InputPoints(&(penInfo[i]), 1, NULL, 0);
        }
        else
        {
            UINT msg = MyWM_PEN_BASE + ((penInfo[i].uId - 1) << 1); //����uId=0��Ϊ������ã�����ΪuId�ıʰ󶨵���Ϣ��ΪMyWM_PEN_BASE + 2*(uId - 1)
            msg += ((penInfo[i].ePenState == E_PEN_STATE_UP)?1:0); //
            PostMessage(m_hWndGLBoad, msg, penInfo[i].pt.x,  penInfo[i].pt.y);
        }
    }
  
    if (nPointCount == 0)
    {
         m_bIsIdle = true;
    }
    else
    {
        m_bIsIdle = false;
    }

    return false;
}

//CSmartMultiPen
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CSmartMultiEraser
CSmartMultiEraser::CSmartMultiEraser()
: m_bIsIdle(true)
,m_hWndGLBoad(NULL)
,m_isSmartMatchReset(true)
{
    m_oSmartEraserMatch.Reset();
}

CSmartMultiEraser::~CSmartMultiEraser()
{
}

void CSmartMultiEraser::ResetSmartMatch()
{
    if (!m_isSmartMatchReset)
    {
        DoRecognition(NULL, 0);
        m_isSmartMatchReset = true;
    }
}

bool CSmartMultiEraser::DoRecognition(POINT *points, int nPointCount)
{
    if (m_bIsIdle && nPointCount == 0) return true;    

    m_oSmartEraserMatch.DoMatch(points, nPointCount);

    m_isSmartMatchReset = false;

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartEraserMatch.GetAllMatchInfo(&nElementCount);

    TContactInfo penInfo[GLBOARDMAXPENNUM];

    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);  

    for (int i = 0; i < nElementCount; i++)
    {
        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
        penInfo[i].uId       = pMatchInfo[i].uId;
        penInfo[i].pt        = pMatchInfo[i].ptPos;
    }

    m_oStrokFilter.DoFilter(penInfo, nElementCount);

    for (int i = 0; i < nElementCount; i++)
    {  
        UINT msg = MyWM_ERASER_BASE + (penInfo[i].uId  << 1); //���ΪuId����Ƥ���󶨵���Ϣ��ΪMyWM_ERASER_BASE + 2*uId
        msg += ((penInfo[i].ePenState == E_PEN_STATE_UP)?1:0); //
        PostMessage(m_hWndGLBoad, msg, penInfo[i].pt.x,  penInfo[i].pt.y);
    }

    if (nPointCount == 0)
    {
        m_bIsIdle = true;
    }
    else
    {
        m_bIsIdle = false;
    }

    return false;
}
//CSmartMultiEraser
//////////////////////////////////////////////////////////////////////////


CMultiPenGestureRecognition::CMultiPenGestureRecognition(void)
: m_nActualSplitPenCount(0)
 ,m_nActualSplitEraserCount(0)
 ,m_isEnableGR(true)
 ,m_nHistoryErasers(0)
{
}

CMultiPenGestureRecognition::~CMultiPenGestureRecognition(void)
{
}

void CMultiPenGestureRecognition::ResetSmartMatch()
{
    m_multiPen.ResetSmartMatch();
    m_multiEraser.ResetSmartMatch();

    //m_lastEraserMap.clear();
}

void CMultiPenGestureRecognition::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    m_multiPen.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);
}

bool CMultiPenGestureRecognition::SplitLightSpotByMass(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser)
{
    //Ϊ���ԣ��Ȱ����еĹ�߿��ɱ�
    /*int actualCount = nCount > GLBOARDMAXPENNUM ? GLBOARDMAXPENNUM : nCount;
    for (int i = 0; i < nCount; i++)
    {
        m_ptSplitPens[i] = aryLightSpots[i].ptPosInScreen;
    }

    m_nActualSplitPenCount = actualCount;*/
    const int MAXALTERASERSIZE = 128;
    const int MAXLIGHTSPOTSPERMITTED = 128;
    
    int thresh_120 = g_oWGRConfig.MapStandardX2ActualValue(120);

    int MERGE_THRESHOLD = thresh_120 * thresh_120;//(m_nMergeAreaWidth*m_nMergeAreaWidth)/8;

    int posAlterEraser[MAXALTERASERSIZE] = {-1};                //�洢ɨ��õ��ĺ�ѡ��Ƥ����ߵ��±�
    //POINT ptAlterEraser[MAXALTERASERSIZE];
    //bool flagAlterEraser[MAXLIGHTSPOTSPERMITTED] = {false};     //��־aryLightSpots��Ӧλ���Ƿ񱻺�ѡΪ��Ƥ��
    bool flagMerged[MAXLIGHTSPOTSPERMITTED] = {false};          //��־aryLightSpots��Ӧλ���Ƿ��Ѿ����ϲ���
    int nActualAlterErasers = 0;

    int nSpotCount = nCount > MAXLIGHTSPOTSPERMITTED ? MAXLIGHTSPOTSPERMITTED : nCount;    

    if(m_isEnableGR)
    {        
        AtlTrace(_T("Before Merge: nSpotCount = %d \r\n"), nSpotCount);

        //ɨ��aryLightSpots����ȡ��ѡ��Ƥ��    
        for (int i = 0; i < nSpotCount; i++)
        {
            if ((aryLightSpots[i].lStdSpotAreaInVideo) * nMultiEraser < aryLightSpots[i].mass)
            {//�����������������Ϊ��ѡ��Ƥ��
                posAlterEraser[nActualAlterErasers] = i;
                //flagAlterEraser[nActualAlterErasers] = true;
                nActualAlterErasers++;
            }
            //else
            //{
            //    //���ڹ���ǰ�mass�Ӵ�С���еģ������ǰmass������Ҫ��
            //    break;
            //}
        }
        //
        AtlTrace(_T("Before Merge: AlterEraserCount = %d \r\n"), nActualAlterErasers);

        //�ϲ���ѡ��Ƥ��MERGE_THRESHOLD��Χ�Ĺ��
        int j = 0;
        m_nActualSplitEraserCount = 0;
        while(j < nActualAlterErasers)
        {
            if (flagMerged[posAlterEraser[j]] == true)
            {//�Ѿ����ϲ�
                j++;
                continue;
            }

            int nToBeMerged = posAlterEraser[j];
            int nMass = aryLightSpots[nToBeMerged].mass;
            POINT pt = aryLightSpots[nToBeMerged].spotParam.ptPosInScreen;

            flagMerged[nToBeMerged] = true;   //��־nToBeMergedλ�ù���Ѿ����ϲ�

            for (int k = 0; k < nSpotCount; k++)
            {
                if (flagMerged[k] == true)
                {//�Ѿ����ϲ�
                    continue;
                }

                int dx = pt.x - aryLightSpots[k].spotParam.ptPosInScreen.x;
                int dy = pt.y - aryLightSpots[k].spotParam.ptPosInScreen.y;

                int R2 = dx*dx + dy*dy;//�������֮��ľ���

                if (R2 < MERGE_THRESHOLD)
                {
                    //��߹�������Ҫ�ϲ�
                    pt.x = (pt.x * nMass + aryLightSpots[k].spotParam.ptPosInScreen.x * aryLightSpots[k].mass) / (nMass + aryLightSpots[k].mass);
                    pt.y = (pt.y * nMass + aryLightSpots[k].spotParam.ptPosInScreen.y * aryLightSpots[k].mass) / (nMass + aryLightSpots[k].mass);

                    nMass += aryLightSpots[k].mass;

                    flagMerged[k] = true;  //��־kλ�ù���Ѿ����ϲ�
                }
            }

            if (m_nActualSplitEraserCount < GLBOARDMAXERASERNUM)
            {
                m_ptSplitErasers[m_nActualSplitEraserCount] = pt;
                m_nActualSplitEraserCount++;
            }

            j++;
        }
    }  
    else
    {
        //����������ʶ���������ʷ�����ڵİ����Ϣ
        m_nHistoryErasers = 0;
        m_nActualSplitEraserCount = 0;
        m_multiEraser.ResetSmartMatch();
    }

    //
    if (m_nActualSplitEraserCount >= GLBOARDMAXERASERNUM)
    {//�µİ������������������������������Ѹ�����ʷ�����ϢΪ���İ���������´�ʹ��
        m_nHistoryErasers = GLBOARDMAXERASERNUM;        
    }
    else if(nSpotCount > 0)
    {
        //�������°��ƥ����ʷ�����Ȼ����δƥ�����ʷ����ϲ�ʣ��ı���Ϣ       
        bool flagHisMatched[GLBOARDMAXERASERNUM] = {false};

        int i = 0;
        while(i < m_nActualSplitEraserCount)
        {
            POINT &refCurEraser = m_ptSplitErasers[i];
            for(int j = 0; j < m_nHistoryErasers; j++)
            {
                if (flagHisMatched[j] == false)
                {
                    POINT &refHisEraser = m_ptHistoryEraser[j];

                    int dx = refCurEraser.x - refHisEraser.x;
                    int dy = refCurEraser.y - refHisEraser.y;

                    int R2 = dx*dx + dy*dy;//�������֮��ľ���

                    if (R2 < MERGE_THRESHOLD)
                    {
                        flagHisMatched[j] = true;
                    }
                }
            }

            i++;
        }

        //��δ�ϲ���ǰnCanAddedErasers����ʷ����ϲ��µıʵ�
        i = 0;        
        while(i < m_nHistoryErasers)
        {
            if (flagHisMatched[i]) {i++; continue;} //����ʷ����Ѿ���ƥ�䵽�°����            

            POINT &refHisEraser = m_ptHistoryEraser[i];
            
            bool toBeMerged = false;
            for (int j = 0; j < nSpotCount; j++)
            {
                if (flagMerged[j] == false)
                {
                    //�ù��δ���ϲ����µİ���У������루δ���°��ƥ��ģ���ʷ�����
                    const POINT &refSpot = aryLightSpots[j].spotParam.ptPosInScreen;

                    int dx = refSpot.x - refHisEraser.x;
                    int dy = refSpot.y - refHisEraser.y;

                    int R2 = dx*dx + dy*dy;//�������֮��ľ���

                    if (R2 < MERGE_THRESHOLD)
                    {
                        flagMerged[j] = true;                        
                        refHisEraser = refSpot;
                        toBeMerged = true;
                    }
                }
            }            

            if(toBeMerged)
            {                
                m_ptSplitErasers[m_nActualSplitEraserCount] = refHisEraser;

                m_nActualSplitEraserCount++;
            }            

            i++;

            if (m_nActualSplitEraserCount >= GLBOARDMAXERASERNUM) break;            
        }
    }

    if (m_nActualSplitEraserCount > 0) memcpy(m_ptHistoryEraser, m_ptSplitErasers, sizeof(POINT) * m_nActualSplitEraserCount); //������ʷ�����Ϣ
    m_nHistoryErasers = m_nActualSplitEraserCount;

    //��δ���ϲ�����Ƥ���Ĺ�ߴ����ʵ�����
    int k = 0;
    m_nActualSplitPenCount = 0;
    while(k < nSpotCount)
    {
        if (flagMerged[k] == false)
        {
            if (m_nActualSplitPenCount < GLBOARDMAXPENNUM)
            {
                m_ptSplitPens[m_nActualSplitPenCount] = aryLightSpots[k].spotParam.ptPosInScreen;
                m_nActualSplitPenCount++;
            }
        }

        k++;
    }

    AtlTrace(_T("After Merge: PensCount = %d \r\n"), m_nActualSplitPenCount);
    AtlTrace(_T("After Merge: EraserCount = %d \r\n"), m_nActualSplitEraserCount);

    return true;
}

bool CMultiPenGestureRecognition::DoGestureRecognition(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser)
{
    if (m_multiEraser.IsIdle() && m_multiPen.IsIdle() && nCount == 0) return true;

    bool isEraserResponse = true;
    bool isPenResponse = true;
    
    SplitLightSpotByMass(aryLightSpots, nCount, nMultiEraser);
    
    isPenResponse = m_multiPen.DoRecognition(m_ptSplitPens, m_nActualSplitPenCount);    
    isEraserResponse = m_multiEraser.DoRecognition(m_ptSplitErasers, m_nActualSplitEraserCount);

    return false;
}
