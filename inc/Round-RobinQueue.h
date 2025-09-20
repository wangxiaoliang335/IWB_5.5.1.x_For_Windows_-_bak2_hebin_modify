#pragma once
//ѭ������FIFO
//�Ƚ��ȳ�
template <class T, int size>
class CRoundRobinQueue
{
public:
    CRoundRobinQueue()
    {
        Reset();
    }

    //@����:��FIFO��д������
    //@˵��:д�����ݺ� ���"FIFO��"��־, 
    //      �����һ��дλ�úͶ�λ����ͬ������λ"FIFO��"��־
    void Write(const T& t)
    {
        m_data[m_nWritePos] = t;

        m_nWritePos++;

        if (m_nWritePos == _countof(m_data))
        {
            m_nWritePos = 0;
        }

        m_bIsEmpty = FALSE;

        if (m_nWritePos == m_nReadPos)
        {
            m_bIsFull = TRUE;
        }


    }

    //@����:��FIFO�ж�ȡ����
    //@˵��:��ȡ���ݺ� ���"FIFO��"��־, 
    //      �����һ����λ�ú�дλ����ͬ������λ"FIFO��"��־
    const T* Read()
    {
        const T* pT =  &m_data[m_nReadPos];

        m_nReadPos++;
        if (m_nReadPos == _countof(m_data))
        {
            m_nReadPos = 0;
        }

        m_bIsFull = FALSE;

        if(m_nReadPos == m_nWritePos)
        {
            m_bIsEmpty = TRUE;
        }


        return pT;
    }



    BOOL IsEmpty()
    {
        return m_bIsEmpty;
    }

    BOOL IsFull()
    {
        return m_bIsFull;
    }


    void Reset()
    {
        m_nReadPos = m_nWritePos = 0;
        m_bIsEmpty = TRUE;
        m_bIsFull  = FALSE;

    }
protected:

    int m_nReadPos;
    int m_nWritePos;
    T m_data[size];

    //˵��:���պ�д��ʱ, ������m_nReadPos == m_nWritePos
    //     �����Ҫ�ɶ�д����ʵʱ���ٿ�/��״̬
    BOOL m_bIsEmpty;//���ձ�־
    BOOL m_bIsFull ;//д����־

};