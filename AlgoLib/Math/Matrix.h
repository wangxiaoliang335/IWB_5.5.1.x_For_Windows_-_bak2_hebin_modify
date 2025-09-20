#pragma once

#include "Vec.h"
#include <stdio.h>



template<class T,int nRows,int nCols>
class Matrix
{

public:
	typedef Vec<T, nCols> RowType;
	typedef Vec<T, nRows> ColType;
	typedef  Matrix<T, nRows, nCols> MatrixType;
	Matrix()
		:
	m_nRows(nRows),
	m_nCols(nCols)
	{
		
	}

	MatrixType& operator = (const MatrixType& right)
	{

		for(int nRow = 0; nRow < m_nRows; nRow++)
		{
			m_ary[nRow] = right[nRow];
		}

		return *this;
	}
	
	
	//@function :get a row of the matrix
	//@parameter:nRowNo, 0 based
	RowType& GetRow(int nRowNo)
	{

		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";

		return m_ary[m_nRowNo];


	}

	//@function :get a row of the matrix
	//@parameter:nRowNo, 0 based
	const RowType& GetRow(int nRowNo)const
	{

		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";

		return m_ary[m_nRowNo];


	}

	//@function :set a row of the matrix
	//@parameter:nRowNo, 0 based
	void SetRow(int nRowNo,RowType& row)
	{
		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";
		//if(vecRow.size() <=0 || vecRow.size() >= m_nCols) throw "Column size mismatched.";

		m_ary[m_nRowNo] = row;
	}



	//@function :get a col of the matrix
	//@parameter:nColNo, 0 based.
	ColType GetCol(int nColNo)  
	{
		 ColType col;
		if(nColNo < 0 || nColNo >= m_nCols) throw "Column no error"; 

		for(int i=0; i< m_nRows;i++)
		{
			col[i] = m_ary[m_nRows][nColNo];
		}
		return col;
	}

	//@function :the matrix multiplied by a col vector
	//@parameter:vecCol, 
	ColType operator*(const RowType& col)
	{
		ColType result;
		for(int iRow=0; iRow< m_nRows; iRow++)
		{
			result[iRow]=0;
			for(int iCol=0;iCol<m_nCols;iCol++)
			{
				result[iRow] += m_ary[iRow][iCol] * col[iCol];
			}
		}

		return result;
	}

	void  SetVal(int nRow, int nCol,const T& val)
	{


		if(nCol < 0 || nCol >= m_nCols) throw "Column no error."; 
		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";		

		m_ary[nRow][nCol]  = val;	
	}


	T GetVal(int nRow, int nCol) const
	{
		if(nCol < 0 || nCol >= m_nCols) throw "Column no error."; 
		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";		

		return m_ary[nRow][nCol];	
	}


	Matrix<T,nRows,nCols> operator*(const Matrix<T,nRows,nCols> & right)
	{

		Matrix<T,nRows,nCols> oResult;

		//if(m_nRows != right.GetCols())
		//	throw "Left matrix row number does not match the colum number of the right matrix.";
		//oResult.SetCols(right.GetCols());
		//oResult.SetRows(m_nRows);



		for(int iRow=0; iRow < m_nRows; iRow++)
		{

			for(int iCol=0; iCol< m_nCols; iCol++)
			{
				T val = (T)0;

				for(int k=0; k<m_nCols;k++)
				{
					val += m_ary[iRow][k] * right.GetVal(k,iCol);

				}

				oResult.SetVal(iRow,iCol,val);
			}//for iCol

		}//for iRow


		return oResult;

	}

	RowType& operator[](int nRow)
	{

		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";
		return m_ary[nRow];

	}


	const RowType& operator[](int nRow) const
	{

		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";
		return m_ary[nRow];

	}

	inline int GetRows() const 
	{
		return m_nRows;
	}

	//inline void SetRows(int nRows)
	//{
	//	m_nRows = nRows; 

	//	m_ary.resize(m_nRows);
	//	for(int i=0;i< m_nRows; i++)
	//	{
	//		m_ary[i].resize(m_nCols);
	//		memset(&m_ary[i][0],0,m_ary[i].size());
	//	}
	//}

	inline int GetCols()const 
	{
		return m_nCols;
	}

	//inline void SetCols(int nCols)
	//{
	//	m_nCols = nCols; 

	//	m_ary.resize(m_nRows);
	//	for(int i=0;i< m_nRows; i++)
	//	{
	//		m_ary[i].resize(m_nCols);
	//		memset(&m_ary[i][0],0,m_ary[i].size());
	//	}
	//}

	void Zero()
	{
		for(int iRow = 0; iRow < m_nRows; iRow++)
		{
			for(int iCol = 0; iCol < m_nCols; iCol++)
			{

				m_ary[iRow][iCol ] = 0;
			}
		}
	}


	void SwapRow(int row1, int row2)
	{
		for(int iCol = 0; iCol < m_nCols; iCol++)
		{
			T temp = m_ary[row1][iCol];
			m_ary[row1][iCol]  = m_ary[row2][iCol];
			m_ary[row2][iCol]  = temp;

		}	

	}


	//ת�ò���
	Matrix<T,nCols, nRows> Transpose() const
	{
		Matrix<T,nCols, nRows> m;

		for(int i = 0; i< nRows; i++)
		{
			for(int j=0; j < nCols; j++)
			{
				m[j][i] = m_ary[i][j];

			}//for-each(j)
			
		}//for-each(i)


		return m;


	}

	//@����:���������ݱ�����ASCII�ļ���
	//�� ��:szFileName, �ļ���
	//@˵��:ÿ������֮���������ո�ָ���,��Matlab�б���ĸ�ʽһ��
	bool Save(const char* szFileName)const
	{

		FILE* file = NULL;
			
		errno_t err =  fopen_s(&file, szFileName, "wt");

		if(err == EINVAL)
		{
			return false;
		}


		//char buf[256];
		
		for(int r=0; r < m_nRows; r++)
		{
			const RowType& row = m_ary[r];
			
			for(int c=0; c < m_nCols; c++)
			{
				
				fprintf(file, "  %.7e", row[c]);

			}//for-each(col)

			fprintf(file, "\r\n");
		}//for-each(row)

		fclose(file);
		return true;
	}
protected:
	Vec<RowType, nRows> m_ary;
	int m_nRows;//
	int m_nCols;//
};


//����
template <class T, int Dimension>
class CSquareMatrix:public Matrix<T, Dimension, Dimension>
{
public:
	typedef CSquareMatrix<T, Dimension> SquareMatrixType;

	
	//@����:�������캯��
	const CSquareMatrix& operator=(const Matrix<T, Dimension, Dimension>& right)
	{
		this->m_ary;// = right.m_ary;
		return *this;
	}

	//@��  ��:���ô�������ʽ���㷽�������ʽֵ
	//@����ֵ:T
	//
	T Det()
	{
		T value = T(0);


		for(int col=0; col< this->m_nCols; col++)
		{

			T sign  =  (col % 2) == 0? 1:-1; 

			CSquareMatrix<T, Dimension -1> cofactorMatrix;

			cofactorMatrix = CofactorMatrix(0, col);

			value += sign * m_ary[0][col] * cofactorMatrix.Det();
		}

		return value;
	}


	//@�� ��:��������ʽ�任��������ʽ��ֵ��
	//      ������ʽ���½�ȫ����Ϊ0
	//@˵ ��:���ô�������ʽ��������ʽ��ֵʱ, �ڼ���߽�����ʱ, Ч�ʺܲ�
	//      ����10�׵�����ʽ, ��Ҫ������10!��˫���ȸ�������3,628,800��
	//@ע�� :ʹ��Det2���뱣֤T���Ͳ���Ϊ���ͣ�����Ϊ��������     
	T Det2()
	{
		SquareMatrixType m = *this;
		T value = T(0);


		//
		for(int row=0; row<m.m_nRows; row++)
		{
			
			if(m.m_ary[row][row] == 0 )
			{
				
				//��row+1...m_nRow-1֮����ҵ�row�в�Ϊ0����, �����row�н�����
				int l = 0;
				for(l=row+1; l<m.m_nRows; l++)
				{
					if(m.m_ary[l][row] != 0)
					{
						//����l�к�row��
						m.SwapRow(l, row);
						break;
					}

				}

				//�����еĵ�row�о�Ϊ0
				if(l == m.m_nRows)
				{
					return T(0);
				}

			}
			

			//�任row+1��...m_nRow-1��
			for(int l=row+1; l<m_nRows; l++)
			{
				
				T coef =   m.m_ary[l][row] / m.m_ary[row][row];

				for(int col=row; col<m.m_nCols; col++)
				{
					m.m_ary[l][col]  = m.m_ary[l][col]  - m.m_ary[row][col]*coef;

				}
			}	


		}



		//�öԽ���Ԫ�صĳ˻���Ϊ����ʽ��ֵ��

		value = T(1);
		for(int i=0; i<m.m_nRows; i++)
		{
			value *= m.m_ary[i][i];
		}

		return value;
	}

	//@��  ��:���㷽���row��col�еĴ�������ʽ����
	//@����ֵ:T
	CSquareMatrix<T, Dimension - 1 >  CofactorMatrix(int nRowPos, int nColPos)
	{

		CSquareMatrix<T, Dimension -1 > m;
		int i=0, j=0;
		for(int row=0; row <m_nRows; row ++)
		{
			if(row == nRowPos)
			{
				continue;
			}
			else
			{
				for(int col=0; col < m_nCols;  col++)
				{
					if(col == nColPos)
					{
						continue;
					}

					m[i][j] = m_ary[row][col];

					j++;
				}//for col
			}

			i++;
			j = 0;
		}//for row

		return m;
	}

/*
	virtual CSquareMatrix<T, Dimension> operator*(const CSquareMatrix<T, Dimension> & right)
	{

		CSquareMatrix<T, Dimension> oResult;

		//if(m_nRows != right.GetCols())
		//	throw "Left matrix row number does not match the colum number of the right matrix.";
		//oResult.SetCols(right.GetCols());
		//oResult.SetRows(m_nRows);



		for(int iRow=0; iRow < m_nRows; iRow++)
		{

			for(int iCol=0; iCol< m_nCols; iCol++)
			{
				T val = (T)0;

				for(int k=0; k<m_nCols;k++)
				{
					val += m_ary[iRow][k] * right.GetVal(k,iCol);

				}

				oResult.SetVal(iRow,iCol,val);
			}//for iCol

		}//for iRow


		return oResult;

	}
*/
private:

};

//1ά�����ģ��ʵ��, ����ֹ�������ĵݹ�չ��
template<class T>
class CSquareMatrix<T, 1>:public Matrix<T, 1,1>
{
public:
	//@
	//
	//
	T Det()
	{
		T value = m_ary[0][0];

		return value;
	}

private:

};
