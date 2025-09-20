#pragma once

//template <int PENCOUNT = 30>

class CTouchTester
{
public:
	CTouchTester();
	~CTouchTester();

	void Reset();

	BOOL Process(const TContactInfo* pPenInfos);

	const TContactInfo* GetContactInfo() const;
	int GetContactCount();

protected:

	int   m_nScreenWidth ;   //��Ļ�ĳ��ȡ�
	int   m_nScreenHeight;   //��Ļ�ĸ߶ȡ�

	int   m_nOffsetX   ;     //X���ƫ������ֵ��
	int   m_nOffsetY   ;     //Y���ƫ������ֵ��
	int   m_nIntervalX ;     //ÿ���ߵļ����
	int   m_nIncreaseY ;     //ÿ���ߵ�Y�������ֵ
	int   m_nLeftNum   ;     //�ο�ֵ��ߵ�����

	static const int PENCOUNT = 30;
	TContactInfo m_ContactInfo[PENCOUNT];

};