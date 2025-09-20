#include "stdafx.h"
#include "TouchTester.h"

CTouchTester::CTouchTester()
{
	m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_nOffsetX = 50 ;   //X���ƫ����
	m_nOffsetY = 200 ;   //Y���ƫ������
	m_nIntervalX = (m_nScreenWidth - m_nOffsetX * 2) / PENCOUNT;
	m_nIncreaseY = 0;
	m_nLeftNum = 0;
}

CTouchTester::~CTouchTester()
{
	Reset(); 
}

void CTouchTester::Reset()
{
	m_nOffsetX = 50 ; 
	m_nOffsetY = 200;   
	m_nIntervalX = (m_nScreenWidth - m_nOffsetX * 2) / PENCOUNT;
	m_nIncreaseY = 0;
	m_nLeftNum = 0;
}

BOOL CTouchTester::Process(const TContactInfo* pPenInfos)
{

//	m_nIncreaseY++;
//	for(int i = 0 ; i < PENCOUNT;i++)
//	{
//		m_ContactInfo[i].uId = i ;
//		////ģ��ĵ��״̬�Ͳ��յ��״̬����һ�£������Ϳ��Կ������е�İ��º�̧���ˡ�
//		m_ContactInfo[i].ePenState = pPenInfos[0].ePenState;
//		m_ContactInfo[i].pt.x = m_nOffsetX + i*m_nIntervalX;
//		m_ContactInfo[i].pt.y = m_nOffsetY + m_nIncreaseY;
//    }
	//�ȼ���ο�ֵ��߻����ұߵ�ֵ
	if (m_nIncreaseY == 0 )
	{
		m_nLeftNum = pPenInfos[0].pt.x / m_nIntervalX;
	    m_nIncreaseY++;
	}

	////�ο�����ߵ�����
	for (int i = 0 ; i < m_nLeftNum; i++)
	{
		m_ContactInfo[i].uId = i;
		m_ContactInfo[i].ePenState = pPenInfos[0].ePenState;
		m_ContactInfo[i].pt.x = (pPenInfos[0].pt.x - (m_nLeftNum -1-i)*m_nIntervalX);
	    m_ContactInfo[i].pt.y = pPenInfos[0].pt.y;
	}

	////�ο����ұߵ�����
	for(int j = m_nLeftNum; j < PENCOUNT; j++)
	{
		m_ContactInfo[j].uId = j;
		m_ContactInfo[j].ePenState = pPenInfos[0].ePenState;
		m_ContactInfo[j].pt.x = pPenInfos[0].pt.x + (j- m_nLeftNum +1)*m_nIntervalX;
		m_ContactInfo[j].pt.y = pPenInfos[0].pt.y;
	}
	return TRUE;
}

const TContactInfo* CTouchTester::GetContactInfo()const
{
	return &m_ContactInfo[0];
}

int CTouchTester::GetContactCount()
{
	return _countof(m_ContactInfo);
}