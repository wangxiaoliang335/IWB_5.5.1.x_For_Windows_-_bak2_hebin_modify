#pragma once
//ʸ����
template <class T, int Dimension>
class Vec
{
public:
	Vec& operator=(const Vec& r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] = r.m_data[i];
		}
		return *this;
	}
	T& operator[](int index)
	{
		return m_data[index];
	}

	const T& operator[](int index) const
	{
		return m_data[index];
	}

	Vec& operator- (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] -= r.m_data[i];
		}
		return *this;
	}

	Vec& operator-= (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] -= r.m_data[i];
		}
		return *this;
	}
	

	Vec& operator+ (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] += r.m_data[i];
		}
		return *this;
	}

	Vec& operator+= (const Vec&r)
	{
		for(int i=0; i<Dimension; i++)
		{
			m_data[i] += r.m_data[i];
		}
		return *this;
	}


	//@����:��ʸ�����ݱ�����ASCII�ļ���
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

		for(int c=0; c < Dimension; c++)
		{
			fprintf(file, "  %.7e", m_data[c]);

		}//for-each(col)

		fprintf(file, "\r\n");		

		fclose(file);
		return true;
	}


protected:
	T m_data[Dimension];
};