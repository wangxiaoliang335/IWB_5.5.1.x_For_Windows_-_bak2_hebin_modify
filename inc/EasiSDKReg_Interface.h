#pragma once
#ifdef EASISDKREG_INTERFACE_EXPORTS
    #define EASISDKREG_INTERFACE_API _declspec(dllexport)
#else
    #define EASISDKREG_INTERFACE_API _declspec(dllimport)

#endif

#include <string>
extern "C"
{

//@����:SDK��ʼ��
EASISDKREG_INTERFACE_API HRESULT  SKDREG_Init();


//@����:SDK����ʼ��
EASISDKREG_INTERFACE_API void  SKDREG_Uninit();

//@��  ��:ͨ����ȡUSB�����,�������������ȷ�������, �жϻ��������Ƿ����
//@��  ��:nFunctionNo,1~15,�������ܱ��
//@����ֵ:S_OK,��������ʹ��
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��
EASISDKREG_INTERFACE_API HRESULT SDKREG_IsBasicFunctionEnabled(int nFunctionNo);


//@��  ��:ͨ����ȡUSB�����,�ж�PPT����SDK�Ƿ����
//@�� ��:nUSBKeyIndex, ��0��ʼ��USB Key������
//@����ֵ:S_OK,PPT����SDK����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��
EASISDKREG_INTERFACE_API HRESULT SDKREG_PPTIndexSDKEnabled(UINT uUSBKeyIndex = 0);



//@��  ��:ͨ����ȡUSB�����,�ж��������¼�����SDK�Ƿ����
//@�� ��:nUSBKeyIndex, ��0��ʼ��USB Key������
//@����ֵ:S_OK,�������¼�����SDK����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��
EASISDKREG_INTERFACE_API HRESULT SDKREG_KMEventHookSDKEnabled(UINT uUSBKeyIndex = 0);


//@��  ��:ͨ����ȡUSB�����,�ж���������̨SDK�Ƿ����
//@�� ��:nUSBKeyIndex, ��0��ʼ��USB Key������
//@����ֵ:S_OK,��������̨SDK����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��
EASISDKREG_INTERFACE_API HRESULT SDKREG_MousePTZControlSDKEnabled(UINT uUSBKeyIndex = 0);



//@��  ��:���USB�������״̬, �������������ȷ���������
//@����ֵ:S_OK,USB�����״̬����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����,USB������������м�¼��HID����ʵ��HID��һ��
//        -5,δʹ��
//        -6,ϵͳʱ�����
//        -7,д�������ʧ��
//@˵��:ÿ�μ����Ϻ�,�Ὣϵͳʱ��д��USB������
EASISDKREG_INTERFACE_API HRESULT SDKREG_CheckUSBSoftwareDog();


//@��  ��:���ش�����Ϣ
//@��  ��:lErrorCode,�������
//@����ֵ:��������Ӧ��������Ϣ
EASISDKREG_INTERFACE_API const WCHAR* SDKREG_GetErrorMsg(HRESULT lErrorCode);

//@��  ��:��ȡ�������汾��
//@��  ��:pVersion, ��ż��ܹ��汾�����ݻ�������
//        nUSBKeyIndex, ��0��ʼ��USB Key������
//@����ֵ:S_OK,��������̨SDK����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��,
//        -6,ϵͳʱ�����
//        -7,д�����
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetVersion(float* pVersion, UINT uUSBKeyIndex = 0);


//@��  ��:��ȡ��������Ӧ��Ӧ�ó�������
//@��  ��:pAppType, ���Ӧ�ó������͵����ݻ�������
//        nUSBKeyIndex, ��0��ʼ��USB Key������
//@����ֵ:S_OK,��������̨SDK����
//        -1,�����������
//        -2,��������ݼ���ʧ��
//        -3,����������
//        -4,�����Ӳ��ID����
//        -5,δʹ��
//        -6,ϵͳʱ�����
//        -7,д�����
//@˵��:�װ嶨λ�������,
//      ��һ���ֽڶ���
//      bit0: ��λ�������
//            0:�ʴ����Ӱװ�
//            1:�ִ����Ӱװ�
//            2:��ָ����
//            3:���ƻ���
//
//      �ڶ��ֽڶ���
//      bit0:˫��ƴ�ӹ���ʹ��λ
//            0���޶�˫��ƴ�ӹ���
//            1���ж���ƴ�ӹ���
//      bit1:0:windows
//           1:android
//       ����λ����δ��
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetAppType(int* pAppType, UINT uUSBKeyIndex = 0);


//@�� ��:����USB Key����Ŀ
EASISDKREG_INTERFACE_API UINT SDKREG_GetUSBKeyCount();


//@�� ��:����USB Key��·��
//@����:uUSBKeyIndex, USBKey��������
//      devPath, �����豸·���Ļ�����
//      bufSize, �������ߴ�
EASISDKREG_INTERFACE_API HRESULT SDKREG_GetUSBKeyDevPath(UINT uUSBKeyIndex, char* devPathBuf, int bufSize);


//@��  ��:��ȡ����ƴ������,��SDKREG_GetAppType���صĶ���ƴ��ʹ�����ʹ��
//@����ֵ:B0~31�Ķ�������
//       B0:˫��ƴ��
//       B1:3��ƴ��
//       B2:4��ƴ��
//       B3:5��ƴ��
//       B31~4:Ԥ��
EASISDKREG_INTERFACE_API UINT SDKREG_GetMultiScreenMergeType(UINT uUSBKeyIndex = 0);


//@��  ��:����Ӧ�ò�������
//@����ֵ:
//       ����ָ����:0:F0; 1:F1; 2:F2; 3:F3; 4:F4;  5:F5
//       �����ƴ���:0:P0; 1:P1; 2:P2; 3:P3; 4:P4;  5:P5;
//                 6:T0; 7:T1; 8:T2; 9:T3;10:T4; 11:T5
//                 12:TX1;13:TX2
//
//
//
EASISDKREG_INTERFACE_API UINT SDKREG_GetParamType(UINT uUSBKeyIndex = 0);



#define COMPENSATE_PARAM_COUNT   6
#define DATA_PREFIX_BYTE_COUNT 3
#define DATA_SUFFIX_BYTE_COUNT 3


//@��  ��:����������У����������ֵ
//@�� ��:pParamsBuf, ���油���������׵�ַ
//       nParamsBufCount, ��������
//       uUSBKeyIndex, USB�豸����.
//       devPathBuf, �����豸·���Ļ�����, ������ʶ��ͬ���豸��
//@����ֵ:��ȡ��double��������
EASISDKREG_INTERFACE_API  UINT SDKREG_ReadE2PROMCompensateParams(double* pParamsBuf, int nParamsBufCount, UINT uUSBKeyIndex, char* devPathBuf = NULL, int bufSize = 0);

//@��  ��:д�벹����У����������ֵ
//@��  ��:
//        pParams, Ҫд��Ĳ���ֵ�׵�ַ
//        nParamCount, д��Ĳ�������
//         uUSBKeyIndex, USB�豸����.
//@����ֵ:  S_OK, �ɹ�
//         E_FAIL, ʧ��
EASISDKREG_INTERFACE_API  HRESULT SDKREG_WriteE2PROMCompensateParams(const double* pParams, int nParamCount, UINT uUSBKeyIndex);

}