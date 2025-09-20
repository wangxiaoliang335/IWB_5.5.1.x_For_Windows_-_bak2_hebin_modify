#pragma once
#ifdef EASI_TOUCHSCREEN_INTERFACE_EXPORTS
    #define EASI_TOUCHSCREEN_API _declspec(dllexport)
#else
    #define EASI_TOUCHSCREEN_API _declspec(dllimport)
#endif

#define TIP_UP   0x02   //���
#define TIP_DOWN 0x03   //�±�

//#define EASI_TOUCH_MAXIMUM_X 0x7FFF
//#define EASI_TOUCH_MAXIMUM_Y 0x7FFF
#define EASI_TOUCH_MAXIMUM_X (1920*8)
#define EASI_TOUCH_MAXIMUM_Y (1080*8)

//��󴥿ص����
#define MAX_CONTACT_COUNT 30

//ÿ��Report�а�������󴥿ص����
#define MAX_CONTACT_COUNT_PER_REPORT 10

struct EASI_TouchPoint
{
    UCHAR  bStatus  ;//bit0: Tip Switch; bit1:In Range; bit2...7: Padding bits
    UCHAR  ContactId;//contact indentifiers
    USHORT wXData   ;//ˮƽ����,ȡֵ��Χ:0~0x7FFF(EASI_TOUCH_MAXIMUM_X)
    USHORT wYData   ;//��ֱ����,ȡֵ��Χ:0~0x7FFF(EASI_TOUCH_MAXIMUM_Y

    int iCameraId;  //�������豸id
};

#define ALL_BTN_UP      0x00
#define LEFT_BTN_DOWN   0x01
#define RIGHT_BTN_DOWN  0x02
#define MIDDLE_BTN_DOWN 0x04
#define BACK_BTN_DOWN   0x08
#define FOWARD_BTN_DWON 0x40

#define EASI_MOUSE_MAXIMUM_X  0x7FFF
#define EASI_MOUSE_MAXIMUM_Y  0x7FFF


struct EASI_MouseInput
{
    //Buttions bit map:
    //D0: Left Button
    //D1: Right Button
    //D2: Middle Button
    //D3: Back Button
    //D4: Forward Button;
    //D5..7: Padding bits
    UCHAR Buttons;
    USHORT X     ;//ˮƽ����,ȡֵ��Χ:0~0x7FFF(EASI_MOUSE_MAXIMUM_X)
    USHORT Y     ;//��ֱ����,ȡֵ��Χ:0~0x7FFF(EASI_MOUSE_MAXIMUM_X)
    UCHAR Wheel  ;//��ֱ����,ȡֵ��Χ:(-127)0x81~0x7F(127)
    UCHAR ACPan  ;//ˮƽ����,ȡֵ��Χ:(-127)0x81~0x7F(127) Application Control Pan, Horizontal Scrolling
};


enum ETouchScreenMode
{
    EASI_MOUSE = 0,
    EASI_SINGLE_OUCH = 1,
    EASI_MULTI_TOUCH = 2

};

//@����:���������������豸�ӿ�GUID�š�
extern "C" EASI_TOUCHSCREEN_API const GUID& EASI_GetDeviceInterfaceGUID();

//@����:���豸,�����豸���
extern "C" EASI_TOUCHSCREEN_API HANDLE  EASI_OpenDevice();

//ȡ������
//extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_SelectDeviceMode(ETouchScreenMode eMode);

//@����:д�봥�����ݵ���������
//@����:hDevice, ����EASI_OpenDevice���ص��豸���
//      pTouchPoint, ָ��EASI_TouchPoint�����ָ��
//      nPointCount, ���ص����Ŀ
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_WriteVirtualTouchScreen(HANDLE hDevice, const EASI_TouchPoint *pTouchPoint, int nPointCount);

//@����:д��������ݵ�����������ȥ
//@����:hDevice, ���ú���EASI_OpenDevice���ص��豸���
//      pMouseInput�� ָ������������ݵ�ָ��
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_WriteVirtualMouse(HANDLE hDevice, const EASI_MouseInput *pMouseInput);

//@����:�ر��豸���
//@����:hDevice, ���ú���EASI_OpenDevice���ص��豸���
extern "C" EASI_TOUCHSCREEN_API BOOL    EASI_CloseDevice(HANDLE hDevice);


extern "C" EASI_TOUCHSCREEN_API const GUID  GUID_DEVINTERFACE_EASI_UMDF_TouchScreen;
