#pragma once
#include "Matrix.h"
#include "SVD.h"
//
// b1 = | c11 c12 c13 | x1
// b2 = | c21 c22 c23 | x2
// b3 = | c31 c32 c33 | x3
// 
// .
// .
//

//��Ԫһ�η�����
template <class T, int nNumberOfVariables>
class CLinearEquation
{
public:


	typedef CSquareMatrix<T,nNumberOfVariables> CoefMatrix;
	typedef Vec<T, nNumberOfVariables> VecB;
	typedef Vec<T, nNumberOfVariables> VecResult;

	//@����:���÷���ϵ������
	void InitCoef(const CoefMatrix& m, const VecB& vecB)
	{
		m_CoefMatrix = m;
		m_vecB = vecB;
		
	}

	//@����:
	//		���ÿ���ķ����ⷽ��
	//      Cramer's rule
	//@˵��: ���㷨��ȱ����, ����ʽ��ֵ�ڸ߽׵�������п��������
	BOOL CalcResult(VecB& result)
	{
		//T divisor  = m_CoefMatrix.Det();

		T divisor = m_CoefMatrix.Det2();

		if(divisor == 0) return FALSE;

		for(int i = 0; i < nNumberOfVariables; i++)
		{
			CoefMatrix m = m_CoefMatrix;

			//�滻m�ĵ�i��Ϊm_vecB
			for(int j=0; j<nNumberOfVariables; j++)
			{
				m[j][i] = m_vecB[j];
			}

			result[i] = m.Det2()/divisor;

		}//for
		
		return TRUE;

	}

	//@����:�������б仯�ķ�����ⷽ�̣� ���������ʽ����ֵ��
	BOOL CalcResult2(VecB& result)
	{
		//ϵ������ͷ����ұߵĳ�ϵ����������ͬ���б任��

		//�Ƚ�ϵ��������ϵ���ͨ���б任Ϊ��������ʽ�������½�Ԫ��ȫΪ0���ٴ�������,��ϵ�������б仯Ϊ
		//�ԽǾ��󣬷����ұߵĳ�ϵ����������ͬ���б任���ֵ����Ϊ����δ֪������ֵ��

        //ϵ������
		CoefMatrix m = m_CoefMatrix;
        
		//��ϵ������
        VecB       b = m_vecB;

		int row,col;
		int l=0,k=0;

		int rowCount = m.GetRows();
		int colCount = m.GetCols();

		//��һ��ͨ���б任��ϵ�������Ϊ��������ʽ��
		for(row=0; row<rowCount; row++)
		{

			T divisor = m[row][row];
			if(m[row][row] == 0)
			{
				//��row+1...m_nRow-1֮����ҵ�row�в�Ϊ0���У� �����row�н�����
	
				for(l=row+1; l<rowCount; l++)
				{
					//�ҵ���, ���row�н���
					if(m[l][row] != 0)
					{
						m.SwapRow(l, row);

						//��ϵ������
						T temp;
						temp = b[row];
						b[row] = b[l];
						b[l]   = temp;

						break;
					}
				}//for(l)

				if(l == rowCount)
				{
					return FALSE;
				}
			}



			//�仯row+1... rowCount-1��
			for(l=row+1; l<rowCount; l++)
			{
				
				//T  multiplicand =  m[l][row]/m[row][row];
				T f1 = m[l][row];
				T f2 = m[row][row];



				for(int col=row; col<colCount; col++)
				{

					//m[l][col] = m[l][col] - m[row][col] * multiplicand;
					m[l][col] = m[l][col] - m[row][col] *f1/f2;
				}//for(col)

				b[l] = b[l] - b[row]*f1/f2;

			}//for(l)
       

		}//for


		//�ڶ���:�ӵ����ڶ��п�ʼ����ϵ�������Ϊ�ԽǾ���
		col = rowCount - 1;//��Ϊ0����
		for(row = rowCount-2;  row>=0; row--, col--)
		{

			for(l=row; l>=0; l--)
			{
				//T multiplicand  = m[l][col]/m[row+1][col];			

				T f1 =  m[l][col];
				T f2 =  m[row+1][col];

				//ǰ�м�����

				//��ÿ�д���
				for(k = col; k >= row; k--)
				{
					//m[l][k] = m[l][k] - m[row+1][k]*multiplicand;
					m[l][k] = m[l][k] - m[row+1][k]* f1 /f2;
				}//for(k)


				//b[l] = b[l] - b[row+1]*multiplicand;
				b[l] = b[l] - b[row+1] * f1 / f2;

			}//for(l)

		}//for(row)



		//������
		for(row=0; row < rowCount; row++)
		{
			result[row] = b[row]/m[row][row];
		}


		return TRUE;
	}




	//
	//@����:����������ϵ�����������ķ�����ⷽ��
	//    
	//@����:���ط��̼�����������
	//@˵ ��:�跽��ΪCX=B
	//       ����CΪm��m����
	//       �������C������D = Inv(C)
	//       ��X=D��B
	//
	BOOL CalcResult3(VecB& result)
	{
		CoefMatrix invMatrix;//�����
		
		//����ϵ���������
		pinv(m_CoefMatrix, invMatrix, 0.0001);

		//
		result = invMatrix * m_vecB;


		//<<debug

		Matrix<T,nNumberOfVariables, nNumberOfVariables> m;
		m = invMatrix * m_CoefMatrix;

		//debug>>


		return TRUE;

	}


	//@����:���ø�˹��ȥ��������Է�����
	//       Gauss emiliation with Paritial pivoting
	BOOL Calc_Gauss(VecB& result)
	{
		 //ϵ������
		CoefMatrix m = m_CoefMatrix;
        
		//��ϵ������
        VecB       B = m_vecB;


		int row;

		int rowCount = m.GetRows();
		int colCount = m.GetCols();

		
		int N = (rowCount > colCount)?colCount:rowCount;

		
		//Step1:ת��Ϊ�����Ǿ���
		for(row=0; row < N-1; row++)
		{
			//���ҵ�row����[row...N]Ԫ���е����ֵ��
			T maxVal =  m_CoefMatrix[row][row];
			
			
			int nPivotRow = row;

			for(int k=row+1; k < N ; k++)
			{
				if(maxVal < m[k][row])
				{
					maxVal = m[k][row];
					nPivotRow = k;
				}
			}

			//
			if(nPivotRow != row)
			{
				T temp;
				m.SwapRow(nPivotRow, row);

				temp = B[row];
				B[row] = B[nPivotRow];
				B[nPivotRow] = temp;
				
			}



			if(maxVal == T(0))
			{
				continue;//
			}

		  T f1 = m[row][row];

		  for(int r=row+1; r < N; r++)
		  {

			  T f2 = m[r][row];

			  for(int c=row; c < N; c++)
			  {
				m[r][c] -= m[row][c] * f2/f1;

			  }

			  B[r] -= B[row]* f2/f1;

		  }
		}

		//Step2: �ӵ�N-1�������ѽ��
		//       ��N-2...0�����̵ݹ���

		if(m[N-1][N-1])
		{
			result[N-1] = B[N-1]/m[N-1][N-1];
		}
		else
		{
			result[N-1] = T(0);
		}

		for(int r = N-2;  r>=0; r--)
		{

			result[r] = B[r];

			for(int c=r+1; c<N; c++)
			{
				result[r] -= result[c]*m[r][c];
			}

			if(m[r][r])
			{
				result[r] = result[r]/m[r][r];
			}
			else
			{
				result[r] = T(0);
			}

		}//for(row)

		return TRUE;
	}

protected:
	//ϵ������
	CoefMatrix m_CoefMatrix;
	
	VecB m_vecB;
};

//Test Example
#include<iostream>
inline void CLinearEquation_Test()
{
	//������Է�����
	/*
	2*X1 +   X2 - 5*X3 +  X4  = 8,
	  X1 - 3*X2 -        6*X4 = 9,
           2*X2 -   X3 + 2*X4 = -5,
	  X1 + 4*X2 - 7*X3 + 6*X4 = 0,

    //���:<3,-4, -1, 1>
	//
    //
	//

	*/

 	CLinearEquation<double, 4> equation;
	CLinearEquation<double, 4>::CoefMatrix coef;
	coef[0][0] =  2;
	coef[0][1] =  1;
	coef[0][2] = -5;
	coef[0][3] =  1;

	coef[1][0] =  1;
	coef[1][1] = -3;
	coef[1][2] =  0;
	coef[1][3] = -6;

	coef[2][0] =  0;
	coef[2][1] =  2;
	coef[2][2] = -1;
	coef[2][3] =  2;

	coef[3][0] =  1;
	coef[3][1] =  4;
	coef[3][2] = -7;
	coef[3][3] =  6;


	CLinearEquation<double, 4>::VecB    B;

	B[0] =  8;
	B[1] =  9;
	B[2] = -5;
	B[3] =  0;

	equation.InitCoef(coef, B);

	CLinearEquation<double, 4>::VecResult result;
	//equation.CalcResult2(result);
	equation.Calc_Gauss(result);

	std::cout<<"x1="
			 <<result[0]
			 <<",x2="
			 <<result[1]
			 <<",x3="
			 <<result[2]
			 <<",x4="
			 <<result[3]
			 <<std::endl;

}