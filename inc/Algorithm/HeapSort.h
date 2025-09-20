#pragma once
template<class T>
class CHeapSortAlgorithm
{
public:
	void operator()(T* pAryObj, UINT nSize, bool bAscend)
	{
		HeapSort(pAryObj, nSize, bAscend);

	}


protected:

	//@������:HeapSort
	//@��  ��:����������m_aryMotionObjects�е�����
	//@��  ��:
	//        bAscend,�Ƿ�����������
	//@����ֵ:m_aryMotionObjects�е����ݷ����仯����TRUE,���򷵻�FALSE;
	//@˵  ��:�ö���������ʱ��ʱ�临�Ӷ���nlog(2)N,��ð�ݷ����򷽷������ܶ�
	bool HeapSort(T* pAryObj, UINT nSize, bool bAscend)
	{
		int k = 0;
		int n = nSize;//[0]~[n-1],�±�ΪK�Ľڵ�����Һ��ӽ����±�ֱ���2*K+1��2*K+2,��Ҷ
		//�����±����ֵΪ(n-1)/2 -1,ע��,ʵ������һ����ȫ������
		bool bRes = false;
		//��ʼ����
		for(k=(n-1)/2;k>=0;k--)
		{
			bRes |= HeapAdjust(pAryObj, k, n-1, bAscend);
		}

		for(k=n-1;k>0;k--)//k=0,����Ҫ��������,��Ϊ[0]~[0]��Ȼ����
		{
			//�Ѷ���Ԫ����[0]~[k]�еļ���ֵ(���߼�Сֵ)
			T obj = pAryObj[k];
			pAryObj [k] = pAryObj[0];//[k],..,[n-1]�Ѿ�����
			pAryObj[0] = obj;
			bRes |= HeapAdjust(pAryObj, 0, k-1, bAscend);//����[0]~[k-1]��ɵĶ�,��[0]��,[1]~[k-1]��Ϊ�󶥶�(С����)

		}//for

		return bRes;
	}

	//@������:HeapAdjust
	//@��  ��:���������з����ĵ�������
	//@��  ��:r,��ʼ�±�;
	//        s,�����±�;
	//        bAscend,�Ƿ�����������;
	//@����ֵ:����Ԫ��λ�÷����仯,����TRUE;
	//@˵  ��:����[r]Ԫ����,[r+1],...,[s]�Ѿ��Ǵ󶥶�(bAscend=TRUE)��С����(bAscend=FALSE)

	bool HeapAdjust(T* pAryObj,int r,int s,bool bAscend)
	{
		int rOld = r;
		T obj = pAryObj[r];
		int k=0;
		for(k=2*r+1;k<=s;k=2*k+1)
		{
			//�������������,��Ҫ����Ϊ�󶥶�,
			if(bAscend)
			{
				if(k<s &&  pAryObj[k+1] > pAryObj[k] ) ++k;//kΪֵ�ϴ��Ԫ�ص��±�

				if(obj >= pAryObj[k]) break;
				pAryObj[r] = pAryObj[k];
				r = k;

			}
			else//����ǽ�������,��Ҫ����ΪС����,
			{
				if(k<s &&  pAryObj[k+1] < pAryObj[k]) ++k;//kΪֵ��С��Ԫ�ص��±�
				if(obj <= pAryObj[k]) break;
				pAryObj[r] = pAryObj[k];
				r = k;
			}

		}
		pAryObj[r]= obj;
		return (rOld==r);
	}

public:

};