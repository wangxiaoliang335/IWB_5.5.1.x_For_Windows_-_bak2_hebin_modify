#pragma once
//@Author:toxuke@gmail.com
//@Date  :2013/03/27

#ifdef MJPG_DECODER_EXPORTS
	#define MJPG_DECODER_API _declspec(dllexport)
#else
	#define  MJPG_DECODER_API _declspec(dllimport)
#endif


//������������Ļص�����
typedef void  (_stdcall *DECODE_DONE_CALLBACK)(void* lpCtx, int nErroCode, const BYTE* lpYUY2, int nLength);


struct jpeg_decompress_struct;
struct my_error_mgr;
class MJPG_DECODER_API CMJPG_Decoder
{
public:
	CMJPG_Decoder()
		:
	m_pfCallback(NULL),
    m_YBuf(NULL),
    m_UBuf(NULL),
    m_VBuf(NULL),
    m_GrayBuf(NULL),
	m_nImageWidth(640),
     m_nImageHeight(480)
	{
		Init();
	}

	~CMJPG_Decoder()
	{
		Uninit();
	}

	void Init();

	void Uninit();
	
	//@����:����MJPG����,�����н��봦��
	//���� :lpMJPGData,ָ��MJPG���ݵ�ָ��
	//      nLength, MJPG���ݳ���
    //      pGrayData, �������, ����Ҷ����ݵĻ�����
    //      pEffectiveDataLength, �������, ����ʵ����Ч���ݳ��ȵı�����ַ
	int ProcessData(const BYTE* lpMJPGData,  int nLength,BYTE* pGrayData, int* pEffectiveDataLength = NULL);

	//@����:���ý��������Ļص�����
	void SetCallBackFunc(DECODE_DONE_CALLBACK callback);

	//const BYTE* GetYUVData() const;
    const BYTE* GetGrayData() const;

	int GetImageWidth()  const;
	int GetImageHeight() const;

private:
    DECODE_DONE_CALLBACK m_pfCallback;

    struct jpeg_decompress_struct* m_cinfo;
    struct my_error_mgr*           m_jerr;

    static  const int RAW_LINES = 8;
    unsigned char**  m_raw_data[3];/* Output row buffer */
    unsigned char*   m_YRowPtr[RAW_LINES];
    unsigned char*   m_URowPtr[RAW_LINES];
    unsigned char*   m_VRowPtr[RAW_LINES];

    int  m_nImageWidth;
    int  m_nImageHeight;

    unsigned char* m_YBuf;
    unsigned char* m_UBuf;
    unsigned char* m_VBuf;

    //unsigned char*  m_YUY2Buf;
    unsigned char*  m_GrayBuf;//����Ҷ�ͼƬ�Ļ�������

	int m_nRowSize;


};