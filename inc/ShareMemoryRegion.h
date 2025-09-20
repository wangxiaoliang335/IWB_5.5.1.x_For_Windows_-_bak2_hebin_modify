#ifndef _H_SHRARE_MEMROY_REGION
#define _H_SHRARE_MEMROY_REGION

#ifdef SHAREMEMORYREGION_EXPORTS

#define SMR_API _declspec(dllexport)
#else
#define SMR_API _declspec(dllimport)
#endif

extern "C"
{
	//@��  ��:����ʼʱ��ʼ������������
	//@��  ��:lReservedΪ����ֵ��0�ǲ����κδ���
	//
	//
	//@����ֵ: ���ز������ͣ�TURE�ǳ�ʼ���ɹ���FALSEΪ��ʼ��ʧ��

	BOOL SMR_API SMR_Init(LONG lReserved=0);

	//@��  ��: �������ʱ��Ҫ�ͷŸ�������
	//@��  ��: lReservedΪ����ֵ��0�ǲ����κδ���
	//
	//
	//@����ֵ: ���ز������ͣ�TURE�ǳ�ʼ���ɹ���FALSEΪ��ʼ��ʧ��

	BOOL SMR_API SMR_Uninit(LONG lReserved=0);

	//@��  ��: �����������ڴ�д������
	//@��  ��: pData����Ҫд������ݣ�dataLengthд�����ݵĳ��ȣ�OffSet ��д��ƫ������lReservedΪ����ֵ��0�ǲ����κδ���
	//
	//
	//@����ֵ: ������д�����ݵĳ���

	UINT SMR_API SMR_Write(const char * pProName, const char * pVerName,const unsigned char* pData, UINT dataLength,UINT OffSet ,LONG lReserved = 0);


	//@��  ��: �����Ǵӹ����ڴ��ж�������
	//@��  ��: pBuf�������ݵĴ洢�ڴ档BufSize�������ݵĳ��ȡ�OffSet �Ƕ���ƫ����lReservedΪ����ֵ��0�ǲ����κδ���
	//
	//
	//@����ֵ: ������д�����ݵĳ���
	UINT SMR_API SMR_Read(const char * pProName, const char * pVerName, unsigned char* pBuf, UINT BufSize,UINT OffSet ,LONG lReserved = 0);


	//@��  ��: �����ǵȴ�д���¼�
	//@��  ��: dwWaitTimeΪ�ȴ���ʱ�䣬��λ�Ǻ���
	//
	//
	//@����ֵ: �����¼�������
	DWORD SMR_API SMR_WaitForWriteEvent(DWORD dwWaitTime);

	//@��  ��: ������д�����������Ϣ
	//@��  ��: ProgramName��������ơ�VersionNumber ����İ汾�ţ�DataLen��Ҫд����ַ��ĳ���
	//
	//
	//@����ֵ: �����ڴ��ĳ���
	
	UINT SMR_API SMR_AddDataBlock(const char * ProgramName, const char * VersionNumber, LPVOID StartAddress ,UINT nDataLen);


	//@��  ��: �����Ƕ�������������Ϣ
	//@��  ��: ProgramName��������ơ�ProgramNameLen�������Ƶĳ��ȣ�VersionNumber ����İ汾�ţ�VersionNumberLen����汾�ŵĳ���
	//
	//
	//@����ֵ: ���ض������ݵĳ���

	UINT SMR_API SMR_ReadDataBlockInfo(char* ProgramName,UINT ProgramNameLen, char * VersionNumber,UINT VersionNumberLen);
}
#endif