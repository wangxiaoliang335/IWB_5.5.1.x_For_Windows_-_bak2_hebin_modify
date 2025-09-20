#pragma once
#include "../inc/VideoProcAmpProperty.h"
#define  AUTO_MASK_AREA_INFLATE_RADIUS_MIN 0
#define  AUTO_MASK_AREA_INFLATE_RADIUS_MAX 10


#define Y_POS_MIN 0
#define Y_POS_MAX 479


#define GUIDELINE1_DEFAULT_POS 120
#define GUIDELINE2_DEFAULT_POS 380

#define  SPOT_MIN                20
#define  SPOT_MAX                80
#define  SPOT_DEFAULT_POS        30

#define SPOT_DEFAULT_CROOD_OFFSET 0

#define SPOT_X_CROOD_OFFSET_MIN  -3000
#define SPOT_X_CROOD_OFFSET_MAX  3000

#define SPOT_Y_CROOD_OFFSET_MIN  -3000
#define SPOT_Y_CROOD_OFFSET_MAX  3000

//�������ʱ��ߴ�С���׼��߳ߴ�ı�ֵ
#define  MULT_ERASER_MIN          3
#define  MULT_ERASER_MAX          7
#define  MULT_ERASER_DEFAULT_POS  5

#define  FIXED_BLOB_SET_TIME_MIN  5
#define  FIXED_BLOB_SET_TIME_MAX  40
#define  FIXED_BLOB_SET_TIME      5

//�Զ�У�����ȵ�������Сֵ
#define  AUTOCALIBRATION_BRIGHTNESS_MIX  -255     //����720p������ͷ�����и�ֵ���֣���������Сֵ����СһЩ                

//�Զ�У�����ȵ��������ֵ
#define  AUTOCALIBRATION_BRIGHTNESS_MAX  255

//����ʹ��ʱ��FP������ȱʡֵ
#define  AUTOCALIBRATION_BRIGHTNESS_DEFAULT_FP     150   
//����ʹ��ʱ��3D��������ȱʡֵ
#define  AUTOCALIBRATION_BRIGHTNESS_DEFAULT_3D      25  


#define  AUTOMASKDETECTTHRESHOLD_MIX                0

#define  AUTOMASKDETECTTHRESHOLD_DEFAULT            180
#define  AUTOCALIBRATEAVERAGEBRIGHTNESS_DEFAULT     255
#define  AUTOCALIBRATELIGHTGRAY                     255

#define  VIDEODISPLAYDELAY_MIN                     0
#define  VIDEODISPLAYDELAY_MAX                     200


//��С�������
#define MIN_DETECT_THRESHOLD_VAL 25
//����������
#define MAX_DETECT_THRESHOLD_VAL 250
//ȱʡ�������
#define DEFAULT_DETECT_THRESHOLD_VAL  (MIN_DETECT_THRESHOLD_VAL + 8 *(MAX_DETECT_THRESHOLD_VAL - MIN_DETECT_THRESHOLD_VAL)/10)

//��СGammaֵ
#define MIN_GAMMA_VAL 0
//���Gammaֵ
#define MAX_GAMMA_VAL 500 
//ȱʡGammaֵ
#define DEFAULT_GAMMA_VAL  1 //zwn 2013.06.21

#define MIN_SPOT_VAL   0
#define MAX_SPOT_VAL   100




#define DEFAULT_FAVORITE_VIDEO_FORMAT_NAME _T("640 X 480 YUY2")
#define DEFAULT_FAVORITE_CAP_DEV_NAME      _T("USB ��Ƶ�豸")

namespace PROFILE
{
    //_declspec(selectany) extern const TCHAR* PROFILE_NAME = _T("Settings.ini");
    _declspec(selectany) extern CAtlString PROFILE_NAME            = _T("Settings.ini");
    //_declspec(selectany) extern CAtlString CONFIG_FILE_NAME        = _T("Config.xml");
    _declspec(selectany) extern CAtlString CONFIG_FILE_NAME        = _T("Video.dll");

	_declspec(selectany) extern CAtlString STATICMASK_NAME         = _T("StaticScreenAreaMask.bmp");
	_declspec(selectany) extern CAtlString DYNAMICMASK_NAME        = _T("DynamicScreenAreaMask.bmp");


    _declspec(selectany) extern CAtlString SCREEN_MAP_NAME          = _T("ScreenMap.bmp");

    _declspec(selectany) extern CAtlString  SETTINGS_BASE_DIRECTORY = _T("");
    
    _declspec(selectany) extern CAtlString WGR_CONFIG_FILE_NAME =_T("WGRConfig.xml");


    _declspec(selectany) extern CAtlString SCREEN_LAYOUT_FILE_NAME = _T("ScreenLayout.xml");


    //_declspec(selectany) extern CAtlString INSTALL_TIP_APP_NAME = _T("InstallTip\\IntallTip.exe");

    const int DEFAULT_SPOT_MINIMUM_WIDTH = 3;
    const int DEFAULT_SPOT_MINIMUM_HEIGHT = 1;

    const int DEFAULT_SPOT_MINIMUM_AREA = 4;
    const int DEFAULT_SPOT_MANIMUM_AREA = 40;

    enum ELenType
    {
        E_LEN_FAR_FOCUS  = 0,
        E_LEN_NEAR_FOCUS_1 = 1,
        E_LEN_NEAR_FOCUS_2 = 2,
        E_LEN_NEAR_FOCUS_3 = 3,
    };

    //inline BOOL GetProfileFullPath(TCHAR* lpszBuf, unsigned int numberOfElements)
    //{
    //    int nRet = GetModuleFileName(GetModuleHandle(NULL), lpszBuf, numberOfElements);
    //    if(nRet == 0 || nRet == numberOfElements)
    //    {
    //        return FALSE;
    //    }

    //    TCHAR* lpszFileName = :: PathFindFileName(lpszBuf);

    //    _tcscpy_s(lpszFileName, numberOfElements - (lpszFileName - lpszBuf), PROFILE_NAME);

    //    return TRUE;
    //}


    //@����:�������ļ���ȡ���ػ���ʶ��(Locale Identifier)�� ���ʧ���򷵻�ϵͳȱʡ�ı��ػ���ʶ����
    inline LCID GetCfg_LCID()
    {

        LCID cfgLCID = 0x00000000;
        LCID defaultLCID = GetSystemDefaultLCID();

        //TCHAR szProfileFullPath[MAX_PATH];
		do
		{
			//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
			//{
			//    cfgLCID = defaultLCID;
			//    break;
			//}
			CAtlString s = PROFILE_NAME;
			BOOL bRet = GetPrivateProfileStruct(_T("Multi Lang"), _T("Locale Identifier"), &cfgLCID, sizeof(LCID), PROFILE_NAME);
			if(!bRet)
			{
				cfgLCID = defaultLCID;
			}

		}while(0);

        return cfgLCID;
    }
////
////
////
////    inline BOOL SaveLCD(LCID cfgLCID)
////    {
////
////        BOOL bRet = TRUE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    bRet = FALSE;
////            //    break;
////            //}
////
////            BOOL bRet = WritePrivateProfileStruct(_T("Multi Lang"), _T("Locale Identifier"), &cfgLCID, sizeof(LCID), PROFILE_NAME);
////        }while(0);
////
////        return bRet;
////    }
////
////
////    //@����:��ȡ���ߵ���С�ߴ��趨ֵ
////    //
////    //@����:ҫ�ߵ���С�ߴ�
////    //
////    inline SIZE GetLightSpotMinimumSize()
////    {
////        SIZE szSpot ;//ҫ�߳ߴ�
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            szSpot.cx = GetPrivateProfileInt(_T("LightSpot"), _T("MinimumWidth" ), DEFAULT_SPOT_MINIMUM_WIDTH, PROFILE_NAME);
////            szSpot.cy = GetPrivateProfileInt(_T("LightSpot"), _T("MinimumHeight"), DEFAULT_SPOT_MINIMUM_HEIGHT, PROFILE_NAME);
////
////        }while(0);
////
////        return szSpot;
////    }
////
////    //@����;����ҫ�ߵ���С�ߴ�
////    //@����:szSpot, ҫ�ߵ���С�ߴ硣
////    inline BOOL SaveLightSpotMinimumSize(const SIZE& szSpot)
////    {
////        BOOL bRet = TRUE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    bRet = FALSE;
////            //    break;
////            //}
////
////            TCHAR szBuf[128];
////            _stprintf_s(szBuf, _countof(szBuf), _T("%02d"), szSpot.cx );
////
////            WritePrivateProfileString(_T("LightSpot"), _T("MinimumWidth" ), szBuf, PROFILE_NAME);
////
////            _stprintf_s(szBuf, _countof(szBuf), _T("%02d"), szSpot.cy );
////            WritePrivateProfileString(_T("LightSpot"), _T("MinimumHeight"), szBuf, PROFILE_NAME);
////
////        }while(0);
////
////        return bRet;
////    }
////
////    //commented out by toxuke@gmail.com, 2013/06/13
////    //����:У���������������Ѿ�����ָ����
////    ////@����:��ȡУ������
////    //inline ELenType GetLenType()
////    //{
////    //    ELenType eCalibrateType = E_LEN_FAR_FOCUS;
////
////    //    TCHAR szProfileFullPath[MAX_PATH];
////    //    do
////    //    {
////    //        if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        {
////    //            break;
////    //        }
////
////    //        eCalibrateType = (ELenType)GetPrivateProfileInt(_T("Calibrate"), _T("LenType" ), (int)E_LEN_NEAR_FOCUS_1, szProfileFullPath);
////
////
////    //    }while(0);
////
////    //    return eCalibrateType;
////    //}
////
////
////    ////@����;����У������
////    ////@����:eCalibrateType, У������
////    //inline BOOL SaveLenType(ELenType eCalibrateType)
////    //{
////    //    BOOL bRet = TRUE;
////
////    //    TCHAR szProfileFullPath[MAX_PATH];
////
////    //    do
////    //    {
////    //        if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        {
////    //            bRet = FALSE;
////    //            break;
////    //        }
////
////    //        TCHAR szBuf[128];
////    //        _stprintf_s(szBuf, _countof(szBuf), _T("%02d"), (int)eCalibrateType);
////
////    //        WritePrivateProfileString(_T("Calibrate"), _T("LenType" ), szBuf, szProfileFullPath);
////
////    //    }while(0);
////
////    //    return bRet;
////    //}
////
    //<<added by toxuke@gmail.com, 2013/06/13
    //@����:��ȡ�ֶ�У��ʱ��ÿһ�е�У����ĸ���
    inline int GetManualCalibratePtNumInEachRow()
    {
        int nCalibrateNumberInEachRow  = 2;

        //TCHAR szProfileFullPath[MAX_PATH];
        do
        {
            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
            //{
            //    break;
            //}

            nCalibrateNumberInEachRow = GetPrivateProfileInt(_T("Calibrate"), _T("ManualCalibratePtNumInEachRow" ), 5, PROFILE_NAME);


        }while(0);

        return nCalibrateNumberInEachRow;
    }

///////////////////////////////
	inline BOOL SaveManualCalibratePtNumInEachRow(int Row)
	{
		BOOL bRet = TRUE ;
	//	TCHAR szProfileFullPath[MAX_PATH];
		do 
		{
// 			if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
// 			{
// 				bRet = FALSE;
// 				break;
// 			}

			TCHAR szBuf[128];
			_stprintf_s(szBuf, _countof(szBuf),_T("%d"),Row);
			WritePrivateProfileString(_T("Calibrate"), _T("ManualCalibratePtNumInEachRow" ), szBuf ,PROFILE_NAME);

		} while (0);

		return bRet;
	}


    //@����:��ȡ�ֶ�У��ʱ��ÿһ�е�У����ĸ���
    inline int GetManualCalibratePtNumInEachCol()
    {
        int nCalibrateNumberInEachCol  = 2;

        //TCHAR szProfileFullPath[MAX_PATH];
        do
        {
            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
            //{
            //    break;
            //}

            nCalibrateNumberInEachCol = GetPrivateProfileInt(_T("Calibrate"), _T("ManualCalibratePtNumInEachCol" ), 5, PROFILE_NAME);


        }while(0);

        return nCalibrateNumberInEachCol;
    }

	////////////////
	inline BOOL SaveManualCalibratePtNumInEachCol(int Col)
	{
		BOOL bRet = TRUE ;
//		TCHAR szProfileFullPath[MAX_PATH];
		do 
		{
// 			if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
// 			{
// 				bRet = FALSE;
// 				break;
// 			}

			TCHAR szBuf[128];
			_stprintf_s(szBuf, _countof(szBuf),_T("%d"),Col);
			WritePrivateProfileString(_T("Calibrate"), _T("ManualCalibratePtNumInEachCol" ), szBuf ,PROFILE_NAME);

		} while (0);

		return bRet;
	}
////
////    //inline BOOL IsFingerTouchType()
////    //{
////    //    BOOL bYes = FALSE;
////
////    //    //TCHAR szProfileFullPath[MAX_PATH];
////    //    do
////    //    {
////    //        //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        //{
////    //        //    break;
////    //        //}
////
////    //        TCHAR szReturnString[1024];
////    //        GetPrivateProfileString(_T("Device"), _T("IsFingerTouchDevice" ), _T("No"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////    //        if(_tcsicmp(szReturnString, _T("Yes")) == 0)
////    //        {
////    //            bYes = TRUE;
////    //        }
////
////
////    //    }while(0);
////
////    //    return bYes;
////    //}
////    //>>
////
////    //@����:��ȡ�����趨
////    inline BOOL GetCheckCalibrateProcess()
////    {
////        BOOL bEnable = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szReturnString[1024];
////            GetPrivateProfileString(_T("Debug"), _T("CheckCalibrateProcess" ), _T("No"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            if(_tcsicmp(szReturnString, _T("Yes")) == 0)
////            {
////                bEnable = TRUE;
////            }
////
////
////        }while(0);
////
////        return bEnable;
////    }
////
////
////
////    inline BOOL IsDebugMode()
////    {
////        BOOL bEnable = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szReturnString[1024];
////            GetPrivateProfileString(_T("Debug"), _T("DebugMode" ), _T("No"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            if(_tcsicmp(szReturnString, _T("Yes")) == 0)
////            {
////                bEnable = TRUE;
////            }
////
////
////        }while(0);
////
////        return bEnable;
////    }
////
////
////    //@����:
////    inline BOOL GetDisableOpticalPenMouseControl()
////    {
////        BOOL bDisable = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////
////            TCHAR szReturnString[1024];
////            GetPrivateProfileString(_T("Settings"), _T("DisableOpticalPenMouseControl" ), _T("No"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            if(_tcsicmp(szReturnString, _T("Yes")) == 0)
////            {
////                bDisable = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("No")) == 0)
////            {
////                bDisable = FALSE;
////            }
////
////
////        }while(0);
////
////        return bDisable;
////
////    }
////
////
////    //@����:�����ֶ��趨����Ļ����
////    //@˵��:��Ļ���������640X480Ϊ�ο���׼��
////    inline RECT LoadManualScreenArea()
////    {
////        BOOL bRet = FALSE;
////        RECT rcArea;
////
////        rcArea.left = 0;
////        rcArea.top = 0;
////        rcArea.right = 640;
////        rcArea.bottom = 480;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szReturnString[1024];
////            GetPrivateProfileString(_T("Manual Screen Area Specification"), _T("Left" ), _T("0"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            rcArea.left = _ttoi(szReturnString);
////
////            GetPrivateProfileString(_T("Manual Screen Area Specification"), _T("Top" ), _T("0"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            rcArea.top = _ttoi(szReturnString);
////
////            GetPrivateProfileString(_T("Manual Screen Area Specification"), _T("Right" ), _T("640"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            rcArea.right = _ttoi(szReturnString);
////
////
////            GetPrivateProfileString(_T("Manual Screen Area Specification"), _T("Bottom" ), _T("480"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            rcArea.bottom = _ttoi(szReturnString);
////
////        }while(0);
////
////        return rcArea;
////    }
////
////
////
////    //@����:�����ֶ��趨����Ļ����
////    //@˵��:��Ļ���������640X480Ϊ�ο���׼��
////    inline BOOL SaveManualScreenArea(const RECT& rcArea)
////    {
////        BOOL bRet = FALSE;
////
////        ///TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), rcArea.left);
////            WritePrivateProfileString(_T("Manual Screen Area Specification"), _T("Left" ), szVal, PROFILE_NAME);
////
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), rcArea.top);
////            WritePrivateProfileString(_T("Manual Screen Area Specification"), _T("Top" ), szVal, PROFILE_NAME);
////
////
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), rcArea.right);
////            WritePrivateProfileString(_T("Manual Screen Area Specification"), _T("Right" ), szVal, PROFILE_NAME);
////
////
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), rcArea.bottom);
////            WritePrivateProfileString(_T("Manual Screen Area Specification"), _T("Bottom" ),szVal, PROFILE_NAME);
////
////            bRet = TRUE;
////        }while(0);
////
////        return bRet;
////    }
////
////
////
////
////    //@����:��ȡ�����ֶ�ָ����Ļ�����趨
////    //@˵��:
////    inline BOOL GetDisableManualScreenArea()
////    {
////
////        BOOL bDisable = TRUE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szReturnString[1024];
////            GetPrivateProfileString(_T("Manual Screen Area Specification"), _T("Disable Manual Screen Area" ), _T("No"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            if(_tcsicmp(szReturnString, _T("Yes")) == 0)
////            {
////                bDisable = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("No")) == 0)
////            {
////                bDisable = FALSE;
////            }
////
////
////        }while(0);
////
////        return bDisable;
////    }
////
////
////
////    //@����:�����ֽ����ֶ�ָ����Ļ�����趨
////    //@˵��:
////    inline BOOL SaveDisableManualScreenArea(BOOL bDisable)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%s"), bDisable?_T("Yes"):_T("No"));
////            WritePrivateProfileString(_T("Manual Screen Area Specification"), _T("Disable Manual Screen Area"), szVal, PROFILE_NAME);
////            bRet = TRUE;
////        }while(0);
////
////        return bRet;
////    }
////
////	inline int GetAutoCalibrateImageAvgBrightness()
////	{
////		int nExpectedBrightness = 10;
////
////		//TCHAR szProfileFullPath[MAX_PATH];
////		do
////		{
////			TCHAR szReturnString[1024];
////			memset(szReturnString, 0, sizeof(szReturnString));
////			GetPrivateProfileString(_T("Calibrate"), _T("AutoCalibrateExptecedBrightness"), _T("150"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////			nExpectedBrightness = _ttoi(szReturnString);
////
////		}while(0);
////
////		return nExpectedBrightness;
////	}
////
////	inline BOOL SaveAutoCalibrateImageAvgBrightness(int nExpectedBrightness)
////	{
////		BOOL bRet = FALSE ;
////		do 
////		{
////			TCHAR szval[1024];
////			memset(szval,0,sizeof(szval));
////			_stprintf_s(szval,_countof(szval),_T("%d"),nExpectedBrightness);
////			WritePrivateProfileString(_T("Calibrate"), _T("AutoCalibrateExptecedBrightness"), szval ,PROFILE_NAME);
////			bRet =TRUE;
////		} while (0);
////
////		return bRet;
////	}
////
////    //@�� ��:����Video Processing Amplifier����ֵ
////    //@�� ��:videoProcAmpProperty,
////    //
////    //inline BOOL SaveVideoProcAmpProperty(const TVideoProcAmpProperty& videoProcAmpProperty)
////    //{
////    //    BOOL bRet = FALSE;
////
////    //    TCHAR szProfileFullPath[MAX_PATH];
////    //    do
////    //    {
////    //        if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        {
////    //            break;
////    //        }
////
////    //        TCHAR szVal[32];
////    //        //1.Brighness
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Brightness);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Brightness"), szVal, szProfileFullPath);
////
////    //        //2.Contrast
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Contrast);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Contrast"), szVal, szProfileFullPath);
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Hue);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Hue"), szVal, szProfileFullPath);
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Satuation);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Satuation"), szVal, szProfileFullPath);
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Sharpness);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Sharpness"), szVal, szProfileFullPath);
////
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_Gamma);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Gamma"), szVal, szProfileFullPath);
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_ColorEnable);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("ColorEnable"), szVal, szProfileFullPath);
////
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcAmp_WhiteBalance);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("WhiteBalance"), szVal, szProfileFullPath);
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcMap_BacklightCompensation);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("BacklightCompensation"), szVal, szProfileFullPath);
////
////
////    //        _stprintf_s(szVal, _countof(szVal), _T("%d"), videoProcAmpProperty.Prop_VideoProcMap_Gain);
////    //        WritePrivateProfileString(_T("VideoProcAmp Property"), _T("Gain"), szVal, szProfileFullPath);
////
////
////    //        bRet = TRUE;
////    //    }while(0);
////
////    //    return bRet;
////    //}
////
////
////    //@����:��ini�ļ���ָ�������ö��ж�ȡ����ͷ����
////    //@����:videoParams, [out], ��ȡ������ͷ����
////    // lpszSectionName, ����
////    // defaultParams, ȱʡ����
////    inline BOOL LoadCameraParams(TVideoProcAmpProperty& videoParams, LPCTSTR lpszSectionName, const TVideoProcAmpProperty& defaultParams)
////    {
////        videoParams = defaultParams;
////
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////
////            TCHAR szReturnString[1024];
////           
////            //1.Brighness
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Brightness);
////            GetPrivateProfileString(lpszSectionName, _T("Brightness" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Brightness = _ttoi(szReturnString);
////
////            //2.Contrast
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Contrast);
////            GetPrivateProfileString(lpszSectionName, _T("Contrast" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Contrast = _ttoi(szReturnString);
////
////            //3.Hue
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Hue);
////            GetPrivateProfileString(lpszSectionName, _T("Hue" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Hue = _ttoi(szReturnString);
////
////            //4.Satuation
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Satuation);
////            GetPrivateProfileString(lpszSectionName, _T("Satuation" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Satuation = _ttoi(szReturnString);
////
////            //5.Sharpness
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Sharpness);
////            GetPrivateProfileString(lpszSectionName, _T("Sharpness" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Sharpness = _ttoi(szReturnString);
////
////            //6.Gamma
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_Gamma);
////            GetPrivateProfileString(lpszSectionName, _T("Gamma" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_Gamma = _ttoi(szReturnString);
////
////            //7.Color Enable
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_ColorEnable);
////            GetPrivateProfileString(lpszSectionName, _T("ColorEnable" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_ColorEnable = _ttoi(szReturnString);
////
////            //8.White Balance
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcAmp_WhiteBalance);
////            GetPrivateProfileString(lpszSectionName, _T("WhiteBalance" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcAmp_WhiteBalance = _ttoi(szReturnString);
////
////            //9.BacklightCompensation
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcMap_BacklightCompensation);
////            GetPrivateProfileString(lpszSectionName, _T("BacklightCompensation" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcMap_BacklightCompensation = _ttoi(szReturnString);
////
////            //10.Gain,����
////            _stprintf_s(szReturnString, _countof(szReturnString), _T("%d"), defaultParams.Prop_VideoProcMap_Gain);
////            GetPrivateProfileString(lpszSectionName, _T("Gain" ), szReturnString, szReturnString, _countof(szReturnString), PROFILE_NAME);
////            videoParams.Prop_VideoProcMap_Gain = _ttoi(szReturnString);
////
////            bRet = TRUE;
////
////        }while(0);
////
////        return bRet;
////
////    }
////
////    //@�� ��:��ȡFP����ʹ��ʱ������ͷ����
////    inline BOOL LoadCameraParamsForNormalUsageFP(TVideoProcAmpProperty& cameraParams)
////    {
////        static TVideoProcAmpProperty defaultParams = {
////                   1,//����
////                 255,//�Աȶ�
////                   0,//ɫ��
////                   0,//���Ͷ�
////                   1,//������
////                   1,//Gammaֵ
////                   0,//��ɫ����
////                5000,//��ƽ��
////                   2,//���ⲹ��
////               17578 //����
////        };
////
////        return LoadCameraParams(cameraParams, _T("CameraParametersForNormalUsage-FP"), defaultParams);
////    }
////	//@�� �ܣ� ��ȡ3D����ʹ��ʱ������ͷ����
////	inline BOOL LoadCameraParamsForNormalUsage3D(TVideoProcAmpProperty& cameraParams)
////	{
////		static TVideoProcAmpProperty defaultParams = {
////			1,//����
////			255,//�Աȶ�
////			0,//ɫ��
////			0,//���Ͷ�
////			1,//������
////			1,//Gammaֵ
////			0,//��ɫ����
////			5000,//��ƽ��
////			2,//���ⲹ��
////			17578 //����
////		};
////
////		return LoadCameraParams(cameraParams, _T("CameraParametersForNormalUsage-3D"), defaultParams) ;
////
////	}
////
////    //@�� ��:��ȡ�Զ�У��ʱ������ͷ����
////    inline BOOL CameraParametersForAutoCalibration(TVideoProcAmpProperty& cameraParams)
////    {
////        static TVideoProcAmpProperty defaultParams = {
////                  30,//����
////                 255,//�Աȶ�
////                   0,//ɫ��
////                   0,//���Ͷ�
////                  32,//������
////                 3,//Gammaֵ
////                  0,//��ɫ����
////               5000,//��ƽ��
////                  2,//���ⲹ��
////               13226//����
////        };
////
////        return LoadCameraParams(cameraParams, _T("CameraParametersForAutoCalibration"), defaultParams);
////    }
////
////    //@�� ��:��ȡ�Զ�����ʱ������ͷ����
////    inline BOOL CameraParametersForAutoMasking(TVideoProcAmpProperty& cameraParams)
////    {
////        static TVideoProcAmpProperty defaultParams = {
////                125,//����
////                 50,//�Աȶ�
////                  0,//ɫ��
////                104,//���Ͷ�
////                  8,//������
////                 100,//Gammaֵ
////                  0,//��ɫ����
////               5000,//��ƽ��
////                  2,//���ⲹ��
////                  0 //����
////        };
////
////        return LoadCameraParams(cameraParams, _T("CameraParametersForAutoMasking"), defaultParams);
////    }
////
////
////    //@�� ��:��ȡ��װ����ʱ������ͷ����
////    inline BOOL CameraParametersForInstallation(TVideoProcAmpProperty& cameraParams)
////    {
////        static TVideoProcAmpProperty defaultParams = {
////                  60,//����
////                 128,//�Աȶ�
////                   0,//ɫ��
////                  64,//���Ͷ�
////                  8,//������
////                  4,//Gammaֵ
////                  0,//��ɫ����
////               5000,//��ƽ��
////                  2,//���ⲹ��
////              13226 //����
////        };
////
////        return LoadCameraParams(cameraParams, _T("CameraParametersForInstallation"), defaultParams);
////    }
////
////	//@�� ��:��ȡ��װ�Ƿ������ʱ������ͷ����
////	inline BOOL CameraParametersForInstallation1(TVideoProcAmpProperty& cameraParams)
////	{
////		static TVideoProcAmpProperty defaultParams = {
////			150,//����
////			150,//�Աȶ�
////			0,//ɫ��
////			64,//���Ͷ�
////			1,//������
////			3,//Gammaֵ
////			0,//��ɫ����
////			5000,//��ƽ��
////			2,//���ⲹ��
////			17578 //����
////		};
////
////		return LoadCameraParams(cameraParams, _T("CameraParametersForInstallation1"), defaultParams);
////	}
////
////
////
////
////    ////@����:��ȡ�Զ�����ʱ��ʹ�õ�����
////    inline int GetAutoMaskingTheshold()
////    {
////        int nThreshold = 200;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("AutoMask"), _T("AutoMaskDetectThreshold"), _T("200"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            nThreshold = _ttoi(szReturnString);
////
////
////        }while(0);
////
////        return nThreshold;
////    }
////
////	inline BOOL SaveAutoMaskingTheshold(int Theshold)
////	{
////		BOOL bRet = FALSE;
////		//TCHAR szProfileFullPath[MAX_PATH];
////		do 
////		{
////			//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////			//{
////			//	break;
////			//}
////			TCHAR szVal[1024];
////			memset(szVal,0,sizeof(szVal));
////			_stprintf_s(szVal,_countof(szVal),_T("%d"),Theshold);
////			WritePrivateProfileString(_T("AutoMask"), _T("AutoMaskDetectThreshold"),szVal,PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("AutoMask"), _T("AutoMaskDetectThreshold"),szVal,PROFILE_SYSTEM_NAME);
////			}
////
////			bRet = TRUE;
////
////		} while (0);
////
////		return bRet;
////	}
////
////    ////@����:��ȡ����ʹ��ʱ������Gammaֵ��
////    //inline int GetNormalOptimumGammaValue()
////    //{
////    //    int nGammaValue = 100;
////
////    //    TCHAR szProfileFullPath[MAX_PATH];
////    //    do
////    //    {
////    //        TCHAR szReturnString[1024];
////    //        memset(szReturnString, 0, sizeof(szReturnString));
////    //        if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        {
////    //            break;
////    //        }
////
////    //        GetPrivateProfileString(_T("Calibrate"), _T("NormalOptimumGamma"), _T("100"), szReturnString, _countof(szReturnString), szProfileFullPath);
////    //        nGammaValue = _ttoi(szReturnString);
////
////
////    //    }while(0);
////
////    //    return nGammaValue;
////    //}
////
////
////
////    ////@����:��ȡ�Զ�����ʱ��Gammaֵ��
////    //inline int GetAutoMaskGammaValue()
////    //{
////    //    int nGammaValue = 46;
////
////    //    TCHAR szProfileFullPath[MAX_PATH];
////    //    do
////    //    {
////    //        TCHAR szReturnString[1024];
////    //        memset(szReturnString, 0, sizeof(szReturnString));
////    //        if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////    //        {
////    //            break;
////    //        }
////
////    //        GetPrivateProfileString(_T("AutoMask"), _T("AutoMaskGamma"), _T("46"), szReturnString, _countof(szReturnString), szProfileFullPath);
////    //        nGammaValue = _ttoi(szReturnString);
////
////
////    //    }while(0);
////
////    //    return nGammaValue;
////    //}
////
////
////
////    inline int GetAutoCalibrateMaskAreaSwellSize()
////    {
////        int nSwellSize = 10;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Calibrate"), _T("AutoCalibrateMaskAreaSwellSize"), _T("1"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            nSwellSize = _ttoi(szReturnString);
////
////
////        }while(0);
////        return nSwellSize;
////    }
////
////
////    inline int GetAutoCalibrateMaxDetectLayer()
////    {
////        int nDetectLayer = 10;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Calibrate"), _T("AutoCalibrateMaxDetectLayer"), _T("10"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            nDetectLayer = _ttoi(szReturnString);
////
////
////        }while(0);
////        return nDetectLayer;
////    }
////
////  
////    //@����:��ȡ�Զ�У��ʱ�����̸��������ĻҶ�ֵ(0~255)
////    //      
////    inline BYTE GetAutoCalibrateHilightGray()
////    {
////        BYTE nGrayVal = 255;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            GetPrivateProfileString(_T("Calibrate"), _T("AutoCalibrateHilightGray"), _T("255"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            nGrayVal = _ttoi(szReturnString);
////
////        }while(0);
////
////        return nGrayVal;
////    }
////
////	inline BOOL SaveAutoCalibrateHilightGray(BYTE nGrayVal)
////	{
////		BOOL bRet = FALSE ;
////		do 
////		{
////			TCHAR szval[1024];
////			memset(szval,0,sizeof(szval));
////			_stprintf_s(szval,_countof(szval),_T("%d"),nGrayVal);
////			WritePrivateProfileString(_T("Calibrate"), _T("AutoCalibrateHilightGray"), szval ,PROFILE_NAME);
////			bRet =TRUE;
////		} while (0);
////
////		return bRet;
////	}
////
////
////    //@����:�����Զ����θ��ŵ�ʱ, ������������Ͱ뾶
////    inline BOOL SaveClutterMaskAreaInflationRadius(int nRadius)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), nRadius);
////
////			WritePrivateProfileString(_T("Advanced Settings"), _T("Clutter Masking Area Inflation Radius"), szVal, PROFILE_NAME);
////
////			if( PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Clutter Masking Area Inflation Radius"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////        }while(0);
////
////        return bRet;
////    }
////
////
////    //@����:�����Զ����θ��ŵ�ʱ, ������������Ͱ뾶
////    inline int GetClutterMaskAreaInflationRadius()
////    {
////        int nRadius = 1;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Clutter Masking Area Inflation Radius"), _T("1"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            nRadius = _ttoi(szReturnString);
////
////        }while(0);
////
////
////        return nRadius;
////
////    }
////
////
////
////    //@����:�����һ��ˮƽ�����ߵĴ�ֱλ��
////    //@����:nYPos, ��ֱλ��
////    inline BOOL SaveFirstGuidelineYPos(int nYPos)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%d"), nYPos);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Y Pos"), szVal, PROFILE_NAME);
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Y Pos"), szVal, PROFILE_SYSTEM_NAME);
////			}
////            bRet = TRUE;
////        }while(0);
////
////        return bRet;
////    }
////
////
////    //@����:��ȡ��һ��ˮƽ�����ߵĴ�ֱλ��
////    inline int GetFirstGuidelineYPos()
////    {
////        int YPos = GUIDELINE1_DEFAULT_POS;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Y Pos"), _T("150"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////            YPos = _ttoi(szReturnString);
////
////        }while(0);
////
////        return YPos;
////    }
////
////
////    //@����:�����һ��ˮƽ�����ߵĴ�ֱλ��
////    //@����:nYPos, ��ֱλ��
////    inline BOOL SaveFirstGuidelineVisibility(int bShow)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%s"), bShow?_T("YES"): _T("NO"));
////			bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Visible"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME !=PROFILE_SYSTEM_NAME)
////			{
////				bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Visible"), szVal, PROFILE_SYSTEM_NAME);
////			}
////        }while(0);
////
////        return bRet;
////    }
////
////
////
////    //@����:�жϵ�һ���������Ƿ����
////    inline BOOL IsFirstGuidelineVisible()
////    {
////        BOOL bShow = TRUE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("First Guide Line Visible"), _T("YES"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////
////            if(_tcsicmp(szReturnString, _T("YES")) == 0)
////            {
////                bShow = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("NO")) == 0)
////            {
////                bShow = FALSE;
////            }
////
////        }while(0);
////
////        return bShow;
////    }
////
////    //@����:����ڶ���ˮƽ�����ߵĴ�ֱλ��
////    //@����:nYPos, ��ֱλ��
////    inline BOOL SaveSecondGuidelineYPos(int nYPos)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////			TCHAR szVal[1024];
////			_stprintf_s(szVal, _countof(szVal), _T("%d"), nYPos);
////			WritePrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Y Pos"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME !=PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Y Pos"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////        }while(0);
////
////        return bRet;
////    }
////
////
////    //@����:��ȡ�ڶ���ˮƽ�����ߵĴ�ֱλ��
////    inline int GetSecondGuidelineYPos()
////    {
////        int YPos = GUIDELINE1_DEFAULT_POS;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Y Pos"), _T("350"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            YPos = _ttoi(szReturnString);
////
////        }while(0);
////
////        return YPos;
////    }
////
////    //@����:����ڶ���ˮƽ�����ߵĴ�ֱλ��
////    //@����:nYPos, ��ֱλ��
////    inline BOOL SaveSecondGuidelineVisibility(int bShow)
////    {
////        BOOL bRet = FALSE;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%s"), bShow?_T("YES"):_T("NO"));
////            bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Visible"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Visible"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////        }while(0);
////
////        return bRet;
////    }
////
////    //@����:�жϵ�һ���������Ƿ����
////    inline BOOL IsSecondGuidelineVisible()
////    {
////        BOOL bShow = TRUE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Second Guide Line Visible"), _T("YES"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////
////            if(_tcsicmp(szReturnString, _T("YES")) == 0)
////            {
////                bShow = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("NO")) == 0)
////            {
////                bShow = FALSE;
////            }
////
////        }while(0);
////
////        return bShow;
////    }
////
////
////    inline int GetDebugLevel()
////    {
////        BOOL nLevel = 0;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Debug"), _T("DebugLevel"), _T("0"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            nLevel = _ttoi(szReturnString);
////
////        }while(0);
////
////        return nLevel;
////    }
////
////    inline BOOL IsSaveIntermidateFile()
////    {
////        BOOL bSave = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Debug"), _T("SaveIntermediateFile"), _T("NO"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////
////            if(_tcsicmp(szReturnString, _T("YES")) == 0)
////            {
////                bSave = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("NO")) == 0)
////            {
////                bSave = FALSE;
////            }
////
////        }while(0);
////
////        return bSave;
////    }
////
////    //@���ܣ������߱���
////    inline BOOL SaveSpotProportionPos(int nSpot)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            memset(szVal,0,sizeof(szVal));
////            _stprintf_s(szVal,_countof(szVal),_T("%d"),nSpot);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("Spot Proportion"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME )
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Spot Proportion"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////
////        }while(0);
////
////        return bRet;
////    }
////
////
////    ///@���ܣ���ȡ��߱���
////    inline int GetSpotProportionPos()
////    {
////        int nSpot =40;
////
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString,0,sizeof(szReturnString));
////            //if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Spot Proportion"), _T("40"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            nSpot = _ttoi(szReturnString);
////        }while(0);
////
////        return nSpot;
////    }
////
////    //@���ܣ��������͹�ߵı�����ϵ
////    inline BOOL SaveMultiplesEraser(int nEraser)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            memset(szVal,0,sizeof(szVal));
////            _stprintf_s(szVal,_countof(szVal),_T("%d"),nEraser);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("Eraser multiples"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Eraser multiples"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////
////        }while(0);
////
////        return bRet;
////    }
////
////    inline int GetMultiplesEraser()
////    {
////        int sEraser = MULT_ERASER_DEFAULT_POS;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString,0,sizeof(szReturnString));
////            //if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Eraser multiples"), _T("4"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            sEraser = _ttoi(szReturnString);
////        }while(0);
////
////        return sEraser;
////    }
////
////    inline BOOL SaveFixedBlobSetTime(int nFixedBlobSetTime)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            memset(szVal,0,sizeof(szVal));
////            _stprintf_s(szVal , _countof(szVal) , _T("%d"), nFixedBlobSetTime);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("Set FixedBlob Time"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME )
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Set FixedBlob Time"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////        } while (0);
////
////        return bRet;
////    }
////
////    inline int GetFixedBlobSetTime()
////    {
////        int sFixedBlobSetTime = FIXED_BLOB_SET_TIME;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString,0,sizeof(szReturnString));
////
////            //if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Set FixedBlob Time"), _T("5"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            sFixedBlobSetTime = _ttoi(szReturnString);
////
////        } while (0);
////
////        return sFixedBlobSetTime;
////    }
////
////    //added by toxuke@gmail.com, 2012/08/20
////    //@����:����У����ˮƽƫ���ֵ
////    inline BOOL SaveXDeviationCompensationVal(int nXDeviationCompensationVal)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            memset(szVal,0,sizeof(szVal));
////            _stprintf_s(szVal,_countof(szVal),_T("%d"),nXDeviationCompensationVal);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("X Deviation Correct Value"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("X Deviation Correct Value"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////            bRet = TRUE;
////
////        }while(0);
////        return bRet;
////    }
////
////    //added by toxuke@gmail.com, 2012/08/20
////    //@����:��ȡУ����ˮƽƫ���ֵ
////    inline int GetXDeviationCompensationVal()
////    {
////        int nXDeviationCompensationVal = 0;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("X Deviation Correct Value"), _T("0"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            nXDeviationCompensationVal = _ttoi(szReturnString);
////
////        }while(0);
////        return nXDeviationCompensationVal;
////    }
////
////    //added by toxuke@gmail.com, 2012/08/20
////    //@����:����У���Ĵ�ֱƫ���ֵ
////    inline BOOL SaveYDeviationCorrectVal(int nYDeviationCompensationVal)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////            TCHAR szVal[1024];
////            memset(szVal,0,sizeof(szVal));
////            _stprintf_s(szVal,_countof(szVal),_T("%d"),nYDeviationCompensationVal);
////            WritePrivateProfileString(_T("Advanced Settings"), _T("Y Deviation Correct Value"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////			{
////				WritePrivateProfileString(_T("Advanced Settings"), _T("Y Deviation Correct Value"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////
////            bRet = TRUE;
////
////        }while(0);
////        return bRet;
////    }
////
////    //added by toxuke@gmail.com, 2012/08/20
////    //@����:��ȡУ���Ĵ�ֱƫ���ֵ
////    inline int GetYDeviationCorrectVal()
////    {
////        int nYDeviationCompensationVal = 0;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("Y Deviation Correct Value"), _T("0"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            nYDeviationCompensationVal = _ttoi(szReturnString);
////
////        }while(0);
////        return nYDeviationCompensationVal;
////    }
////
////
    inline LPCTSTR GetDisplayFontName()
    {

        LCID lcid = GetCfg_LCID();


        TCHAR szLangCode[4];

        const TCHAR* lpszFontName = _T("Arial");

        if (lcid == LOCALE_SYSTEM_DEFAULT)
        {
            lstrcpy(szLangCode, _T("LOC"));
        }
        else
        {
            int nResult;
            nResult = ::GetLocaleInfo(lcid, LOCALE_SABBREVLANGNAME, szLangCode, _countof(szLangCode));
            if (nResult == 0)
            {
                return NULL;
            }
            if(0 == _tcscmp(szLangCode, _T("KOR"))) //����
            {
                lpszFontName = _T("GulimChe");
            }
            else if(0 == _tcscmp(szLangCode, _T("CHN")))//��������
            {
                lpszFontName = _T("MS Song");
            }
            else if(0 == _tcscmp(szLangCode, _T("CHT")))//��������
            {
                lpszFontName = _T("MS Song");
            }
			else if (0 == _tcscmp(szLangCode,_T("ESP")))
			{
				 lpszFontName = _T("GulimChe");
			}
        }
        return lpszFontName;
    }
////
////
////
////    //added by toxuke@gmail.com, 2013/04/18
////    //@����:����ʹ������ʶ��Ĺ���
////    inline BOOL SaveGestureRecognitionSetting(BOOL bEnable)
////    {
////        BOOL bRet = FALSE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            TCHAR szVal[1024];
////            _stprintf_s(szVal, _countof(szVal), _T("%s"), bEnable?_T("FP2"): _T("3D"));
////			bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("The Software Type"), szVal, PROFILE_NAME);
////
////			if (PROFILE_NAME != PROFILE_SYSTEM_NAME )
////			{
////				bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("The Software Type"), szVal, PROFILE_SYSTEM_NAME);
////			}
////
////        }while(0);
////
////        return bRet;
////    }
////
////    //added by toxuke@gmail.com, 2013/04/18
////    //@����:��ȡ����ʶ���ܵ�ʹ��״̬
////    inline BOOL IsGestureRecognitionAvailable()
////    {
////        BOOL bAvailable = TRUE;
////        //TCHAR szProfileFullPath[MAX_PATH];
////        do
////        {
////            TCHAR szReturnString[1024];
////            memset(szReturnString, 0, sizeof(szReturnString));
////            //if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////            //{
////            //    break;
////            //}
////
////            GetPrivateProfileString(_T("Advanced Settings"), _T("The Software Type"), _T("FP2"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////            if(_tcsicmp(szReturnString, _T("FP2")) == 0)
////            {
////                bAvailable = TRUE;
////            }
////            else if(_tcsicmp(szReturnString, _T("3D")) == 0)
////            {
////                bAvailable = FALSE;
////            }
////
////        }while(0);
////
////        return bAvailable;
////    }
////
////////@���ܣ���������Զ�У��������
////inline BOOL SaveAutoCalibrationBrightness(int nBrightness)
////{
////	BOOL bRet = FALSE ;
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do 
////	{
////		//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////
////		TCHAR szVal[1024];
////		memset(szVal,0,sizeof(szVal));
////		_stprintf_s(szVal,_countof(szVal),_T("%d"),nBrightness);
////		WritePrivateProfileString(_T("CameraParametersForAutoCalibration"), _T("Brightness"), szVal, PROFILE_NAME);
////		bRet = TRUE;
////
////	} while (0);
////
////	return bRet;
////}
////
//////@����:����Զ�У��ʱ�����ȿ���ϵ��
////inline int GetAutoCalibrationBrightness()
////{
////	int nAutocalibrationBrightness = 90 ;
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do 
////	{
////		//if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////
////		TCHAR szReturnString[1024];
////		memset(szReturnString,0,sizeof(szReturnString));
////		GetPrivateProfileString(_T("CameraParametersForAutoCalibration"), _T("Brightness"), _T("90"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		nAutocalibrationBrightness = _ttoi(szReturnString);
////
////	} while (0);
////
////	return nAutocalibrationBrightness;
////}
////
////
///////@���ܣ����涨λ�������ʱʹ�õ���Ƶ��������
////inline BOOL SaveVideoParamsType(int nParamsType)
////{
////	BOOL bRet = FALSE ;
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do 
////	{
////		//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////
////		TCHAR szVal[1024];
////		memset(szVal,0,sizeof(szVal));
////		_stprintf_s(szVal,_countof(szVal),_T("%d"),nParamsType);
////		WritePrivateProfileString(_T("Device"), _T("VideoParamsType"), szVal, PROFILE_NAME);
////		bRet = TRUE;
////
////	} while (0);
////
////	return bRet;
////}
////
///////@���ܣ���ȡ��λ�������ʱʹ�õ���Ƶ��������
////inline int GetVideoParamsType()
////{
////	int nParamsType = 0 ;
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do 
////	{
////		//if (!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////
////		TCHAR szReturnString[1024];
////		memset(szReturnString,0,sizeof(szReturnString));
////		GetPrivateProfileString(_T("Device"), _T("VideoParamsType"), _T("0"),szReturnString, _countof(szReturnString), PROFILE_NAME);
////		nParamsType = _ttoi(szReturnString);
////
////	} while (0);
////
////	return nParamsType;
////}
////////@���ܣ��Ƿ񱣴�¼�Ƶ�¼��
////inline BOOL IsRecordVideo()
////{
////	BOOL bRecord = FALSE;
////	//TCHAR szProfileFullPath[MAX_PATH];
////
////	do 
////	{
////		TCHAR szReturnString[1024];
////		memset(szReturnString, 0, sizeof(szReturnString));
////		//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////		GetPrivateProfileString(_T("Advanced Settings"), _T("RecordVideo"), _T("NO"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////		if(_tcsicmp(szReturnString, _T("YES")) == 0)
////		{
////			bRecord = TRUE;
////		}
////		else if(_tcsicmp(szReturnString, _T("NO")) == 0)
////		{
////			bRecord = FALSE;
////		}
////
////	} while (0);
////
////     return bRecord;
////}
////
////inline BOOL SaveRecordVideo(int bShow)
////{
////	BOOL bRet = FALSE;
////
////	//TCHAR szProfileFullPath[MAX_PATH];
////
////	do 
////	{
////		//if(!GetProfileFullPath(szProfileFullPath, _countof(szProfileFullPath)))
////		//{
////		//	break;
////		//}
////		TCHAR szVal[1024];
////		_stprintf_s(szVal, _countof(szVal), _T("%s"), bShow?_T("YES"): _T("NO"));
////		bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("RecordVideo"), szVal, PROFILE_NAME);
////
////		if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////		{
////			bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("RecordVideo"), szVal, PROFILE_SYSTEM_NAME);
////		}
////
////	} while (0);
////
////	 return bRet;
////}
////
////inline BOOL IsDoubleScreen()
////{
////	BOOL bDoubleScreen = FALSE ;
////	do 
////	{
////		TCHAR szReturnString[1024];
////		memset(szReturnString, 0, sizeof(szReturnString));
////
////		GetPrivateProfileString(_T("Advanced Settings"), _T("DoubleScreen"), _T("NO"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		if (_tcsicmp(szReturnString, _T("YES")) == 0)
////		{
////			bDoubleScreen = TRUE;
////		}
////		else
////		{
////			bDoubleScreen = FALSE;
////		}
////
////	} while (0);
////
////	return bDoubleScreen ;
////}
////
///////////�Ƿ�����Ͷģʽ
////inline BOOL IsOpenRearProjection()
////{
////	BOOL bOpen = FALSE;
////
////	do 
////	{
////		TCHAR szReturnString[1024];
////		memset(szReturnString, 0, sizeof(szReturnString));
////
////		GetPrivateProfileString(_T("Advanced Settings"), _T("Rear Projection"), _T("NO"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////
////		if(_tcsicmp(szReturnString, _T("YES")) == 0)
////		{
////			bOpen = TRUE;
////		}
////		else if(_tcsicmp(szReturnString, _T("NO")) == 0)
////		{
////			bOpen = FALSE;
////		}
////
////	} while (0);
////
////	return bOpen;
////}
////////�����Ƿ�ѡ��Ͷģʽ����Ϣ
////inline BOOL SaveOpenRearProjection(int bOpen)
////{
////	BOOL bRet = FALSE;
////	do 
////	{
////		TCHAR szVal[1024];
////		_stprintf_s(szVal, _countof(szVal), _T("%s"), bOpen?_T("YES"): _T("NO"));
////		bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("Rear Projection "), szVal, PROFILE_NAME);
////
////		if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////		{
////			bRet = WritePrivateProfileString(_T("Advanced Settings"), _T("Rear Projection "), szVal, PROFILE_SYSTEM_NAME);
////		}
////
////	} while (0);
////
////	return bRet;
////}
////
////inline int GetNormalUseTempBrightness(BOOL GestureType)
////{
////	int nExpectedBrightness = 25;
////
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do
////	{
////		TCHAR szReturnString[1024];
////		memset(szReturnString, 0, sizeof(szReturnString));
////		if (GestureType)
////		{
////			GetPrivateProfileString(_T("CameraParametersForNormalUsage-FP"), _T("Brightness"), _T("150"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		}
////		else
////		{
////			GetPrivateProfileString(_T("CameraParametersForNormalUsage-3D"), _T("Brightness"), _T("25"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		}
////
////		nExpectedBrightness = _ttoi(szReturnString);
////
////	}while(0);
////
////	return nExpectedBrightness;
////}
////
//////@����:��ȡ�Զ�У�������Զ�����ʱ�������Ļ���ƽ������
//////
////inline int GetNormalUseBrightness()
////{
////	int nExpectedBrightness = 10;
////
////	//TCHAR szProfileFullPath[MAX_PATH];
////	do
////	{
////		TCHAR szReturnString[1024];
////		memset(szReturnString, 0, sizeof(szReturnString));
////		if (IsGestureRecognitionAvailable())
////		{
////			GetPrivateProfileString(_T("CameraParametersForNormalUsage-FP"), _T("Brightness"), _T("150"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		}
////		else
////		{
////			GetPrivateProfileString(_T("CameraParametersForNormalUsage-3D"), _T("Brightness"), _T("25"), szReturnString, _countof(szReturnString), PROFILE_NAME);
////		}
////
////		nExpectedBrightness = _ttoi(szReturnString);
////
////	}while(0);
////
////	return nExpectedBrightness;
////}
////
////inline BOOL SaveNormalUseBrightness(int nExpectedBrightness)
////{
////	BOOL bRet = FALSE ;
////	do 
////	{
////		TCHAR szval[1024];
////		memset(szval,0,sizeof(szval));
////		_stprintf_s(szval,_countof(szval),_T("%d"),nExpectedBrightness);
////
////		if (IsGestureRecognitionAvailable())
////		{
////			WritePrivateProfileString(_T("CameraParametersForNormalUsage-FP"), _T("Brightness"), szval ,PROFILE_NAME);
////		}
////		else
////		{
////			WritePrivateProfileString(_T("CameraParametersForNormalUsage-3D"), _T("Brightness"), szval ,PROFILE_NAME);
////		}
////
////		if (PROFILE_NAME != PROFILE_SYSTEM_NAME)
////		{		
////			if (IsGestureRecognitionAvailable())
////			{
////				WritePrivateProfileString(_T("CameraParametersForNormalUsage-FP"), _T("Brightness"), szval ,PROFILE_SYSTEM_NAME);
////			}
////			else
////			{
////				WritePrivateProfileString(_T("CameraParametersForNormalUsage-3D"), _T("Brightness"), szval ,PROFILE_SYSTEM_NAME);
////			}
////
////
////		}
////
////		bRet =TRUE;
////
////	} while (0);
////
////	return bRet;
////}
////
}