#pragma once
#ifdef MJPG_ENCODER_EXPORTS
    #define MJPG_ENCODER_API _declspec(dllexport)
#else
    #define MJPG_ENCODER_API _declspec(dllimport)
#endif
//@��  ��:��DIB����Ϊ,JPEG�ļ�
//@��  ��:pBIH, ָ��DIBͷ������,��ӵ�ɫ�����������
//@����ֵ:�ɹ�, TRUE
//        ʧ��, FALSE
MJPG_ENCODER_API BOOL Dib2JPEG(const BITMAPINFOHEADER* pBIH, LPCTSTR szFileName);