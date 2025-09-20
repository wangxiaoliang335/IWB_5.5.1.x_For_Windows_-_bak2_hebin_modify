// AdvancedSettingsDlg.cpp : implementation file
//
#include "stdafx.h"
//#include "headers.h"
#include "AdvancedSettingsDlg.h"
#include "AttachToScreenDlg.h"
//#include "MonitorSelectDlg.h"

// CAdvancedSettingsDlg dialog

IMPLEMENT_DYNAMIC(CAdvancedSettingsDlg, CScrollablePropertyPage)


//CAdvancedSettingsDlg::CAdvancedSettingsDlg(const TAdvancedSettings* pSettings, CWnd* pParent /*= NULL*/)
//CAdvancedSettingsDlg::CAdvancedSettingsDlg(const TSensorConfig& sensorConfig, const GlobalSettings& globalSettings, CWnd* pParent /*= NULL*/, LPCTSTR lpszAdditionalCaption)

CAdvancedSettingsDlg::CAdvancedSettingsDlg()
: CScrollablePropertyPage(CAdvancedSettingsDlg::IDD)
 ,m_bInitDone(FALSE)
 ,m_pSensor(NULL)
{

}

CAdvancedSettingsDlg::~CAdvancedSettingsDlg()
{
}

void CAdvancedSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	TSensorModeConfig* pSensorModeConfig = NULL;

	pSensorModeConfig = &m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

	TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

    char szBUf[1024] = { 0 };
    sprintf(szBUf, " CAdvancedSettingsDlg DoDataExchange this:%p, m_pSensor:%p, m_pSensor ID:%d, &this->m_tSensorConfig:%p, eProjectionMode:%d, CameraType:%d, eSelectedLensType:%d\n", this, m_pSensor, m_pSensor->GetID(), &m_tSensorConfig,
        m_tGlobalSettings.eProjectionMode, m_pSensor->GetCameraType(), m_tSensorConfig.eSelectedLensType);
    OutputDebugStringA(szBUf);

	DDX_Text(pDX, IDC_EDIT_SPOTPROPORTION, pSensorModeConfig->advanceSettings.nSpotProportion);
	DDX_Control(pDX, IDC_SPIN_SPOTPROPORTION, m_ctlSpotProportion);

    //DDX_Text(pDX, IDC_EDIT_X_COORD_OFFSET, m_tSensorConfig.nXCoordOffset);
    //DDX_Text(pDX, IDC_EDIT_Y_COORD_OFFSET, m_tSensorConfig.nYCoordOffset);

    DDX_Control(pDX, IDC_SPIN_X_CROOD_OFFSET, m_ctlXCoordOffset);
    DDX_Control(pDX, IDC_SPIN_Y_CROOD_OFFSET, m_ctlYCoordOffset);

	DDX_Control(pDX, IDC_SPIN_SET_AUTOCALIBRATION_BRIGHTNESS, m_ctlNormalUserBrightness);
	DDX_Text(pDX, IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS, lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateExpectedBrightness);
	DDX_Control(pDX, IDC_SPIN_AUTOCALIBRATE_AVERAGE_BRIGHTNESS, m_ctlAutoCalibrationAveBrightness);
	DDX_Text(pDX, IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY, lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateHilightGray);
	DDX_Control(pDX, IDC_SPINAUTOCALIBRATE_HILIGHT_GRAY, m_ctlAutoCalibrateHiLightGray);

	DDX_Text(pDX, IDC_EDIT_VIDEODISPLAYDELAY, lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.videoDislayDelay);
	DDX_Control(pDX, IDC_SPIN_VIDEODISPLAYDELAY, m_ctlVideoIsplayDelay);

	DDX_Radio(pDX, IDC_RADIO_PEN_TOUCH, (int&)pSensorModeConfig->advanceSettings.m_eTouchType);

	DDX_Radio(pDX, IDC_DESKTOPMODE, (int&)m_tGlobalSettings.eProjectionMode);

	//�����ִ��ͱʴ��Ĳ�ͬ���ò�ͬ������ͷ���Ȳ�����
	//<<add by vera_zhao 2019.10.08
	switch (pSensorModeConfig->advanceSettings.m_eTouchType)
	{
	case E_DEVICE_PEN_TOUCH_WHITEBOARD:
		DDX_Text(pDX, IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT, lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
		break;
	case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
		DDX_Text(pDX, IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT, lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
		break;
	case E_DEVICE_FINGER_TOUCH_CONTROL:
		DDX_Text(pDX, IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT, lensCfg.normalUsageSettings_FingerTouchControl.cameraParams.Prop_VideoProcAmp_Brightness);
		break;
	case E_DEVICE_PALM_TOUCH_CONTROL:
		DDX_Text(pDX, IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT, lensCfg.normalUsageSettings_PalmTouchControl.cameraParams.Prop_VideoProcAmp_Brightness);

		break;
	default:
		break;
	}

	//����ͷģʽ����
	DDX_Text(pDX, IDC_EDIT_INSTALLBRIGHTNESS, lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness);
	DDX_Text(pDX, IDC_EDIT_INSTALLGAMMA, lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma);

	//����ϵ��
	DDX_Text(pDX, IDC_EDIT_SMOOTH_COEF, pSensorModeConfig->advanceSettings.nSmoothCoefficient);

	//>>>end
	//delete by vera_zhao
	//     DDX_Text(pDX, IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT,
	//        (pSensorModeConfig->advanceSettings.m_eTouchType == E_DEVICE_FINGER_TOUCH_WHITEBOARD)?
	//        lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness
	//        :
	//        lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness
	//        );

	DDX_Check(pDX, IDC_CHECK_RECORD_VIDEO, m_tGlobalSettings.bRecordVideo);
	DDX_Check(pDX, IDC_CHECK_REAR_PROJECTOR, pSensorModeConfig->advanceSettings.bIsRearProjection);

	/////////////////
	DDX_Check(pDX, IDC_CHECK_DYNAMICMASKFRAMECONTROL, pSensorModeConfig->advanceSettings.bIsDynamicMaskFrame);
	DDX_Check(pDX, IDC_CHECK_ANTIJAMMINGCONTROL, pSensorModeConfig->advanceSettings.bIsAntiJamming);

	DDX_Check(pDX, IDC_CHECK_DISABLEREFLECTIONPOINT, pSensorModeConfig->advanceSettings.bDisableReflectionSpot);
	DDX_Check(pDX, IDC_CHECK_SINGLEPOINTMODE, m_tGlobalSettings.bSinglePointMode);

	DDX_Control(pDX, IDC_SPIN_INSTALLBRIGHTNESS, m_ctlInstallBrightness);
	DDX_Control(pDX, IDC_SPIN_INSTALLGAMMA, m_ctlInstallGramma);

	//DDX_Control(pDX, IDC_SLIDER_SMOOTH, m_ctrlSliderSmooth);
	DDX_Control(pDX, IDC_EDIT_SMOOTH_COEF, m_ctrlEditSmoothCoef);
	DDX_Control(pDX, IDC_SPIN_SMOOTH_COEF, m_ctrlSpinSmoothCoef);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingsDlg, CScrollablePropertyPage)
	ON_BN_CLICKED(IDOK, &CAdvancedSettingsDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_AUTO_MASK_INFLATE_RADIUS, &CAdvancedSettingsDlg::OnEnChangeEditAutoMaskInflateRadius)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_SETTINGS, &CAdvancedSettingsDlg::OnBnClickedButtonDefaultSettings)

	ON_EN_CHANGE(IDC_EDIT_SPOTPROPORTION, &CAdvancedSettingsDlg::OnEnChangeEditSpotProportion)
    ON_EN_CHANGE(IDC_EDIT_X_COORD_OFFSET, &CAdvancedSettingsDlg::OnEnChangeEditXCoordOffset)
    ON_EN_CHANGE(IDC_EDIT_Y_COORD_OFFSET, &CAdvancedSettingsDlg::OnEnChangeEditYCoordOffset)

	ON_EN_CHANGE(IDC_EDIT_MULT_ERASER, &CAdvancedSettingsDlg::OnEnChangeEditMultEraser)
	ON_EN_CHANGE(IDC_EDIT_FIXED_BLOB_SET_TIME_VALUE, &CAdvancedSettingsDlg::OnEnChangeEditFixedBlobSetTimeValue)

	ON_BN_CLICKED(IDC_BUTTON_MANUAL_COLLECT_SPOT, &CAdvancedSettingsDlg::OnBnClickedButtonManualCollectSpot)
 //  ON_BN_CLICKED(IDC_CHECK_GESTURE_RECOGNITION, &CAdvancedSettingsDlg::OnBnClickedCheckGestureRecognition)

	ON_EN_CHANGE(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT, &CAdvancedSettingsDlg::OnEnChangeEditSetNormalUserBrightness)
	ON_BN_CLICKED(IDC_RADIO_PEN_TOUCH, &CAdvancedSettingsDlg::OnBnClickedRadioPenTouch)
	ON_BN_CLICKED(IDC_RADIO_FINGER_TOUCH, &CAdvancedSettingsDlg::OnBnClickedRadioFingerTouch)
	ON_EN_CHANGE(IDC_EDIT_AUTOMASKDETECTTHRESHOLD, &CAdvancedSettingsDlg::OnEnChangeEditAutomaskdetectthreshold)

	ON_BN_CLICKED(IDC_CHECK_RECORD_VIDEO, &CAdvancedSettingsDlg::OnBnClickedCheckRecordVideo)
	ON_EN_CHANGE(IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS, &CAdvancedSettingsDlg::OnEnChangeEditSetAutocalibrateAverageBrightness)
	ON_EN_CHANGE(IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY, &CAdvancedSettingsDlg::OnEnChangeEditAutocalibrateHilightGray)
	ON_BN_CLICKED(IDC_CHECK_REAR_PROJECTOR, &CAdvancedSettingsDlg::OnBnClickedCheckRearProjector)
    ON_CBN_SELCHANGE(IDC_COMBO_MONITOR_LIST, &CAdvancedSettingsDlg::OnCbnSelchangeComboMonitorList)
 
    ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_THROW_RATIO_015, IDC_RADIO_THROW_RATIO_136, &CAdvancedSettingsDlg::OnBnClickRadioLensType)

	ON_BN_CLICKED(IDC_WALLMODE, &CAdvancedSettingsDlg::OnBnClickedRadioWallMode)
	ON_BN_CLICKED(IDC_DESKTOPMODE, &CAdvancedSettingsDlg::OnBnClickedRadioDeskTopMode)
    
	ON_BN_CLICKED(IDC_BUTTON_ATTACH_TO_A_SCREEN, &CAdvancedSettingsDlg::OnBnClickedButtonAttachToAScreen)
	ON_BN_CLICKED(IDC_CHECK_DYNAMICMASKFRAMECONTROL, &CAdvancedSettingsDlg::OnBnClickedCheckDynamicmaskframecontrol)
	ON_BN_CLICKED(IDC_CHECK_ANTIJAMMINGCONTROL, &CAdvancedSettingsDlg::OnBnClickedCheckAntijammingcontrol)

	ON_BN_CLICKED(IDC_CHECK_SINGLEPOINTMODE, &CAdvancedSettingsDlg::OnBnClickedCheckSinglepointmode)
	ON_BN_CLICKED(IDC_CHECK_DISABLEREFLECTIONPOINT, &CAdvancedSettingsDlg::OnBnClickedCheckDisablereflectionpoint)
	ON_EN_CHANGE(IDC_EDIT_VIDEODISPLAYDELAY, &CAdvancedSettingsDlg::OnEnChangeEditVideodisplaydelay)

	ON_EN_CHANGE(IDC_EDIT_INSTALLBRIGHTNESS, &CAdvancedSettingsDlg::OnChangeEditInstallbrightness)
	ON_EN_CHANGE(IDC_EDIT_INSTALLGAMMA, &CAdvancedSettingsDlg::OnChangeEditInstallgamma)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CAdvancedSettingsDlg message handlers

BOOL CAdvancedSettingsDlg::OnInitDialog()
{
	/*CDialog::OnInitDialog();*/
	/*
	if (m_strAdditionalCaption.GetLength() > 0)
	{
		CString strCaption;
		GetWindowText(strCaption);
		strCaption += _T("-");
		strCaption += m_strAdditionalCaption;

		SetWindowText(strCaption);

	}*/

	CScrollablePropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ctlSpotProportion.SetRange(20, 80);

    //m_ctlXCoordOffset.SetRange32(-3000, 3000);  //wxl modify
    //m_ctlYCoordOffset.SetRange32(-3000, 3000);  //wxl modify

	m_ctlAutoCalibrationAveBrightness.SetRange(0,255) ;
	m_ctlAutoCalibrateHiLightGray.SetRange(0,255);
	m_ctlVideoIsplayDelay.SetRange(0,200);

	m_ctlInstallGramma.SetRange(0,1000);

	//m_ctrlSliderSmooth.SetRange(0,10);
	//m_ctrlSliderSmooth.SetTicFreq(1);

	if (m_pSensor->GetDeviceInfo().m_nPID == 0x9230 && m_pSensor->GetDeviceInfo().m_nVID == 0x05a3)
	{
		m_ctlNormalUserBrightness.SetRange(-64, 64);
		m_ctlInstallBrightness.SetRange(-64,64);
	}
	else
	{
		m_ctlNormalUserBrightness.SetRange(0, 255);
		m_ctlInstallBrightness.SetRange(0, 255);
	}
	m_bInitDone = TRUE;

	if (theApp.GetUSBKeyTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD)
	{
		//������ܹ�֧���ִ�ģʽ����ʹ��"�ʴ�/�ִ�"��ѡ��
		GetDlgItem(IDC_RADIO_FINGER_TOUCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_PEN_TOUCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL)->EnableWindow(FALSE);

		//���ʹ������Ϊ�ʴ�,�����"��Ӧ��ߴ�С���������"��spin�ؼ���
		TSensorModeConfig* pSensorModeConfig = NULL;
		pSensorModeConfig = &this->m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];
		//ֻ����ָ��ȷ���ص�ʱ�����Ҫ��ߴ�С�ж�
		if ( pSensorModeConfig->advanceSettings.m_eTouchType == E_DEVICE_FINGER_TOUCH_WHITEBOARD)
		{
			GetDlgItem(IDC_EDIT_SPOTPROPORTION)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_SPOTPROPORTION)->EnableWindow(FALSE);
		}
	}
	else
	{	
		//������ܹ�Ϊ�ʴ�ģʽ, �����"�ʴ�/�ִ�"��ѡ��
		GetDlgItem(IDC_RADIO_FINGER_TOUCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PEN_TOUCH)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL)->EnableWindow(FALSE);

		//�����Ӧ�������ڽ�ֹ��
		GetDlgItem(IDC_EDIT_SPOTPROPORTION)->EnableWindow(FALSE);
	}

	 //////////////
	 if (E_PROJECTION_DESKTOP == m_tGlobalSettings.eProjectionMode)
	 {
		 GetDlgItem(IDC_DESKTOPMODE)->EnableWindow(TRUE);
		 GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL)->EnableWindow(TRUE);
	 }
	 else
	 {
		 GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL)->EnableWindow(FALSE);
	 }

	 //���ذ�ť
	 GetDlgItem(IDC_CHECK_DOUBLE_SCREEN)->ShowWindow(SW_HIDE);
	 GetDlgItem(IDC_BUTTON_ATTACH_TO_A_SCREEN)->ShowWindow(SW_HIDE);

	//��ѡʵ��ѡ��ľ�ͷ����
	CheckDlgButton(IDC_RADIO_THROW_RATIO_015 + int(this->m_tSensorConfig.eSelectedLensType), BST_CHECKED);
	
	//CString strText;
	//GetDlgItem(IDC_EDIT_SMOOTH)->GetWindowTextW(strText);
	//m_ctrlSliderSmooth.SetPos(atoi(CT2A(strText)));

	 //delete by vera_zhao 2020.2.24
     //ֻ����Ϣ, ˫��ƴ��
//     GetDlgItem(IDC_CHECK_DOUBLE_SCREEN)->EnableWindow(FALSE);
//     //CheckDlgButton(IDC_CHECK_DOUBLE_SCREEN, theApp.GetScreenType() == EDoubleScreenMode?BST_CHECKED:BST_UNCHECKED);
//     CheckDlgButton(IDC_CHECK_DOUBLE_SCREEN, theApp.GetScreenMode() >= EScreenModeDouble ? BST_CHECKED : BST_UNCHECKED);
//
//     //��ѡʵ��ѡ��ľ�ͷ����
//      CheckDlgButton(IDC_RADIO_THROW_RATIO_015 + int(this->m_tSensorConfig.eSelectedLensType), BST_CHECKED);
// 
//
//     //�����˫������ʾ"��Ļѡ��"��ť
//     if(theApp.GetScreenMode() >= EScreenModeDouble)
//     {
//        GetDlgItem(IDC_BUTTON_ATTACH_TO_A_SCREEN)->ShowWindow(SW_SHOW);
//     }
//     else
//     {//��������"��Ļѡ��"��ť
//        GetDlgItem(IDC_BUTTON_ATTACH_TO_A_SCREEN)->ShowWindow(SW_HIDE);
//     }
	
    //2020/09/29
    //��������/ǽ��ѡ��ѡ��
    GetDlgItem(IDC_DESKTOPMODE)->EnableWindow(FALSE);
    GetDlgItem(IDC_WALLMODE)->EnableWindow(FALSE);

	m_ctrlSpinSmoothCoef.SetRange(0, 10);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvancedSettingsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	OnOK();
}

void CAdvancedSettingsDlg::OnEnChangeEditAutoMaskInflateRadius()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CString strText;
	GetDlgItem(IDC_EDIT_AUTO_MASK_INFLATE_RADIUS)->GetWindowText(strText);

	int nDilateRadius = atoi(CT2A(strText));

	if(nDilateRadius > AUTO_MASK_AREA_INFLATE_RADIUS_MAX)
	{
		nDilateRadius = AUTO_MASK_AREA_INFLATE_RADIUS_MAX;

		strText.Format(_T("%d"), nDilateRadius);
		GetDlgItem(IDC_EDIT_AUTO_MASK_INFLATE_RADIUS)->SetWindowText(strText);
	}	
	else  if(nDilateRadius < AUTO_MASK_AREA_INFLATE_RADIUS_MIN)
	{
		nDilateRadius = AUTO_MASK_AREA_INFLATE_RADIUS_MIN;
		strText.Format(_T("%d"), nDilateRadius);
		GetDlgItem(IDC_EDIT_AUTO_MASK_INFLATE_RADIUS)->SetWindowText(strText);
	}	
    SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnBnClickedButtonDefaultSettings()     //ȱʡֵ����
{
	//����ǽ������ж�

	TSensorModeConfig* pSensorModeConfig = NULL;
	pSensorModeConfig = &this->m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];
	
    TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

	pSensorModeConfig->advanceSettings.nSpotProportion = SPOT_DEFAULT_POS;

    m_tSensorConfig.nXCoordOffset = SPOT_DEFAULT_CROOD_OFFSET;
    m_tSensorConfig.nYCoordOffset = SPOT_DEFAULT_CROOD_OFFSET;

    //�����ִ��ͱʴ���̬����
	switch(pSensorModeConfig->advanceSettings.m_eTouchType)
	{
	   case E_DEVICE_PEN_TOUCH_WHITEBOARD:
		   lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness = lensCfg.normalUsageSettings_PenTouchWhiteBoard.defaultParams.Prop_VideoProcAmp_Brightness;
		   break;
	   case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
		   lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness = lensCfg.normalUsageSettings_FingerTouchWhiteBoard.defaultParams.Prop_VideoProcAmp_Brightness;
		   break;
	   case E_DEVICE_FINGER_TOUCH_CONTROL:
		   lensCfg.normalUsageSettings_FingerTouchControl.cameraParams.Prop_VideoProcAmp_Brightness = lensCfg.normalUsageSettings_FingerTouchControl.defaultParams.Prop_VideoProcAmp_Brightness;
		   break;
	   case E_DEVICE_PALM_TOUCH_CONTROL:
		   lensCfg.normalUsageSettings_PalmTouchControl.cameraParams.Prop_VideoProcAmp_Brightness = lensCfg.normalUsageSettings_PalmTouchControl.defaultParams.Prop_VideoProcAmp_Brightness;
		   break;
	   default:
		   break;

	}

	/////����ͷģʽ�Ĳ�������
	lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = lensCfg.installTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness;
	lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = lensCfg.installTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma;


    lensCfg.autoMaskSettings.cAutoMaskDetectThreshold =  lensCfg.autoMaskSettings.cAutoMaskDetectThresholdDefault;

	//m_nAutoCalibrationAveBrightness = AUTOCALIBRATEAVERAGEBRIGHTNESS_DEFAULT;
    //lensCfg.autoCalibrateSettings.calibrateImageParamsList[0].autoCalibrateExpectedBrightness = AUTOCALIBRATEAVERAGEBRIGHTNESS_DEFAULT;
    lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateExpectedBrightness =  lensCfg.autoCalibrateSettingsList[0].defaultCalibrateImageParams.autoCalibrateExpectedBrightness;
    
	//m_nAutoCalibrateHiLightGray =         AUTOCALIBRATELIGHTGRAY;
    //lensCfg.autoCalibrateSettings.calibrateImageParamsList[0].autoCalibrateHilightGray  = AUTOCALIBRATELIGHTGRAY;
    lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateHilightGray  = lensCfg.autoCalibrateSettingsList[0].defaultCalibrateImageParams.autoCalibrateHilightGray;

	switch (pSensorModeConfig->advanceSettings.m_eTouchType)
	{
	   case E_DEVICE_PEN_TOUCH_WHITEBOARD:
		   if (lensCfg.autoMaskSettings.cAutoMaskDetectThreshold > lensCfg.normalUsageSettings_PenTouchWhiteBoard.cYThreshold)
		   {
			   //m_nAutoMaskDetectThreshold = m_nThreshold;   //���Ĭ�ϵ�ֵ������������õ�ֵ�Ļ����������ֵΪĬ�ϵ�ֵ��
			   lensCfg.autoMaskSettings.cAutoMaskDetectThreshold = lensCfg.normalUsageSettings_PenTouchWhiteBoard.cYThreshold;
		   }
		  break;
	   case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
		   if (lensCfg.autoMaskSettings.cAutoMaskDetectThreshold > lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cYThreshold)
		   {
			   //m_nAutoMaskDetectThreshold = m_nThreshold;   //���Ĭ�ϵ�ֵ������������õ�ֵ�Ļ����������ֵΪĬ�ϵ�ֵ��
			   lensCfg.autoMaskSettings.cAutoMaskDetectThreshold = lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cYThreshold;
		   }
		   break;
	   case E_DEVICE_FINGER_TOUCH_CONTROL:
		   if(lensCfg.autoMaskSettings.cAutoMaskDetectThreshold > lensCfg.normalUsageSettings_FingerTouchControl.cYThreshold)
		   {
			   lensCfg.autoMaskSettings.cAutoMaskDetectThresholdDefault = lensCfg.normalUsageSettings_FingerTouchControl.cYThreshold;
		   }
		   break;
	   case E_DEVICE_PALM_TOUCH_CONTROL:
		   if(lensCfg.autoMaskSettings.cAutoMaskDetectThreshold > lensCfg.normalUsageSettings_PalmTouchControl.cYThreshold)
		   {
			   lensCfg.autoMaskSettings.cAutoMaskDetectThresholdDefault = lensCfg.normalUsageSettings_PalmTouchControl.cYThreshold;
		   }
		   break;
	   default:
		   break;
	}
	
    //ȱʡ������¼��
    this->m_tGlobalSettings.bRecordVideo = FALSE;

    //ȱʡ��ʹ�ñ�Ͷģʽ
	pSensorModeConfig->advanceSettings.bIsRearProjection = FALSE;

	//ȱʡ������̬���ι���
	pSensorModeConfig->advanceSettings.bIsDynamicMaskFrame = TRUE;
	
	//ȱʡ��ʹ�ÿ����Ź���
	pSensorModeConfig->advanceSettings.bIsAntiJamming = FALSE;

    //�������ݵ��ؼ���ȥ
	UpdateData(FALSE);

    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditXCoordOffset()
{
    if (!m_bInitDone)
    {
        return;
    }
    CString strText;
    GetDlgItem(IDC_EDIT_X_COORD_OFFSET)->GetWindowText(strText);
    int npos = _ttoi(strText);
    int bForceValidate = FALSE;

    if (npos < SPOT_X_CROOD_OFFSET_MIN)
    {
        npos = SPOT_X_CROOD_OFFSET_MIN;
        bForceValidate = TRUE;
    }

    if (npos > SPOT_X_CROOD_OFFSET_MAX)
    {
        npos = SPOT_X_CROOD_OFFSET_MAX;
        bForceValidate = TRUE;
    }

    if (bForceValidate)
    {
        m_tSensorConfig.nXCoordOffset = npos;
        strText.Format(_T("%d"), npos);
        GetDlgItem(IDC_EDIT_X_COORD_OFFSET)->SetWindowText(strText);
    }
    else
    {
        //::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_SPOTPROPORTION, (WPARAM)0, (LPARAM)npos);
    }
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditYCoordOffset()
{
    if (!m_bInitDone)
    {
        return;
    }
    CString strText;
    GetDlgItem(IDC_EDIT_Y_COORD_OFFSET)->GetWindowText(strText);
    int npos = _ttoi(strText);
    int bForceValidate = FALSE;

    if (npos < SPOT_Y_CROOD_OFFSET_MIN)
    {
        npos = SPOT_Y_CROOD_OFFSET_MIN;
        bForceValidate = TRUE;
    }

    if (npos > SPOT_Y_CROOD_OFFSET_MAX)
    {
        npos = SPOT_Y_CROOD_OFFSET_MAX;
        bForceValidate = TRUE;
    }

    if (bForceValidate)
    {
        m_tSensorConfig.nYCoordOffset = npos;
        strText.Format(_T("%d"), npos);
        GetDlgItem(IDC_EDIT_Y_COORD_OFFSET)->SetWindowText(strText);
    }
    else
    {
        //::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_SPOTPROPORTION, (WPARAM)0, (LPARAM)npos);
    }
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditSpotProportion()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_SPOTPROPORTION)->GetWindowText(strText);
	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < SPOT_MIN)
	{
		npos = SPOT_MIN;
		bForceValidate = TRUE;
	}

	if(npos > SPOT_MAX)
	{
		npos = SPOT_MAX;
		bForceValidate = TRUE;
	}

	if(bForceValidate)
	{
		strText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_SPOTPROPORTION)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CHANGE_SPOTPROPORTION,(WPARAM)0,(LPARAM)npos);
	}
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditMultEraser()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_MULT_ERASER)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < MULT_ERASER_MIN)
	{
		npos = MULT_ERASER_MIN;
		bForceValidate = TRUE;
	}

	if (npos > MULT_ERASER_MAX)
	{
		npos = MULT_ERASER_MAX;
		bForceValidate = TRUE;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_MULT_ERASER)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_MULTERASER, (WPARAM)0, (LPARAM)npos);
	}
    SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnBnClickedButtonManualCollectSpot()
{
	// TODO: Add your control notification handler code here
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_MANUAL_COLLECT_SPOT,0,0);
}

void CAdvancedSettingsDlg::OnEnChangeEditFixedBlobSetTimeValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if(!m_bInitDone) return ;
	CString strText;
	GetDlgItem(IDC_EDIT_FIXED_BLOB_SET_TIME_VALUE)->GetWindowText(strText);
	int npos = _ttoi(strText);
    int bForceValidate = FALSE;

	if (npos < FIXED_BLOB_SET_TIME_MIN)
	{
		npos = FIXED_BLOB_SET_TIME_MIN;
		bForceValidate = TRUE         ;
	}

	if (npos > FIXED_BLOB_SET_TIME_MAX)
	{
		npos = FIXED_BLOB_SET_TIME_MAX;
		bForceValidate = TRUE ;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_FIXED_BLOB_SET_TIME_VALUE)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CHANGE_FIXEDBOLBSETTIME,(WPARAM)0,(LPARAM)npos);
	}

    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditSetNormalUserBrightness()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < AUTOCALIBRATION_BRIGHTNESS_MIX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MIX;
		bForceValidate = TRUE;
	}
	if (npos > AUTOCALIBRATION_BRIGHTNESS_MAX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MAX;
		bForceValidate = TRUE;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CHANGE_NORMALUSER_BRIGHTNESS,(WPARAM)0, (LPARAM)npos);
	}
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnBnClickedRadioFingerTouch()
{
	// TODO: Add your control notification handler code here

	 TSensorModeConfig* pSensorModeConfig = NULL;
	 ///m_tGlobalSettings.eProjectionMode=0�����棬m_tGlobalSettings.eProjectionMode=1��ǽ��
	 pSensorModeConfig = &m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

     TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

	 if (IsDlgButtonChecked(IDC_RADIO_FINGER_TOUCH) == BST_CHECKED)
	 {
		 GetDlgItem(IDC_EDIT_SPOTPROPORTION)->EnableWindow(TRUE);

		 //��������ʹ��ʱ������ϵ��
		 CString strBrightnessCoefficient;//����ϵ��
		 strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
		 GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->SetWindowText(strBrightnessCoefficient);

		 //���µ�������ȥ
		 UpdateData(TRUE);
		 SetModified(TRUE);
	 }
}

void CAdvancedSettingsDlg::OnBnClickedRadioWallMode()
{
	if (IsDlgButtonChecked(IDC_WALLMODE) == BST_CHECKED)
	{
		///m_tSensorConfig.vecSensorModeConfig[1]�ŵ���ǽ�������

		m_tGlobalSettings.eProjectionMode = E_PROJECTION_WALL;

		GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL)->EnableWindow(FALSE);

		CString strText;
		strText.Format(_T("%d"), m_tSensorConfig.vecSensorModeConfig[1].advanceSettings.nSpotProportion);
		GetDlgItem(IDC_EDIT_SPOTPROPORTION)->SetWindowText(strText);

		//��������ʹ��ʱ������ϵ��
		TLensConfig& lensCfg = m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

		m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType = GetActualTouchType();

		CString strBrightnessCoefficient;//����ϵ��
		switch(m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType)
		{
		    case E_DEVICE_PEN_TOUCH_WHITEBOARD:
				strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
			    break;
		    case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
				strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
			    break;
			case E_DEVICE_FINGER_TOUCH_CONTROL:
				strBrightnessCoefficient.Format(_T("%d"),lensCfg.normalUsageSettings_FingerTouchControl.cameraParams.Prop_VideoProcAmp_Brightness );
				break;
			case E_DEVICE_PALM_TOUCH_CONTROL:
				strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_PalmTouchControl.cameraParams.Prop_VideoProcAmp_Brightness);
				break;
		    default:
			    break;
		}

		GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->SetWindowText(strBrightnessCoefficient);

		strBrightnessCoefficient.Format(_T("%d"), lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness);
		GetDlgItem(IDC_EDIT_INSTALLBRIGHTNESS)->SetWindowText(strBrightnessCoefficient);
		CString  strGammaCoefficient;
		strGammaCoefficient.Format(_T("%d"), lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma);

		GetDlgItem(IDC_EDIT_INSTALLGAMMA)->SetWindowText(strGammaCoefficient);
	     

		//���´��ط�ʽ
		switch (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType)
		{
		    case E_DEVICE_PEN_TOUCH_WHITEBOARD:

				((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(true);
				((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

				((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);
			     break;
			case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

				((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(true);

				((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);


				break;
			case E_DEVICE_FINGER_TOUCH_CONTROL:

				((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

				((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(true);
				((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);

				break;
			case E_DEVICE_PALM_TOUCH_CONTROL:

				((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

				((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(true);

				break;
		    default:
			    break;
		}

		CString  autoCalibrateExpectedBrightness;
		autoCalibrateExpectedBrightness.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateExpectedBrightness);
		GetDlgItem(IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS)->SetWindowText(autoCalibrateExpectedBrightness);

		CString autoCalibrateHilightGray;
		autoCalibrateHilightGray.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateHilightGray);
		GetDlgItem(IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY)->SetWindowText(autoCalibrateHilightGray);

		CString VideoDisplayDelay;
		VideoDisplayDelay.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.videoDislayDelay);
		GetDlgItem(IDC_EDIT_VIDEODISPLAYDELAY)->SetWindowText(VideoDisplayDelay);

		///����ǽ�������Ĳ�ͬ�����в���������
		if (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bIsDynamicMaskFrame)
		{
			((CButton*)GetDlgItem(IDC_CHECK_DYNAMICMASKFRAMECONTROL))->SetCheck(true);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_DYNAMICMASKFRAMECONTROL))->SetCheck(false);
		}

		if (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bIsAntiJamming)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL))->SetCheck(true);
		}
		else {
			((CButton*)GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL))->SetCheck(false);
		}

		if (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bDisableReflectionSpot)
		{
			((CButton*)GetDlgItem(IDC_CHECK_DISABLEREFLECTIONPOINT))->SetCheck(true);
		}
		else {
			((CButton*)GetDlgItem(IDC_CHECK_DISABLEREFLECTIONPOINT))->SetCheck(false);
		}

		//���µ�������ȥ
		UpdateData(TRUE);
		SetModified(TRUE);
	}
}
void CAdvancedSettingsDlg::OnBnClickedRadioDeskTopMode()
{
	if (IsDlgButtonChecked(IDC_DESKTOPMODE) == BST_CHECKED)
	{
		m_tGlobalSettings.eProjectionMode = E_PROJECTION_DESKTOP;

		GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL)->EnableWindow(TRUE);

		CString strText;
		strText.Format(_T("%d"), m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.nSpotProportion);
		GetDlgItem(IDC_EDIT_SPOTPROPORTION)->SetWindowText(strText);

		//��������ʹ��ʱ������ϵ��
		TLensConfig& lensCfg = m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

		m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType = GetActualTouchType();

		CString strBrightnessCoefficient;//����ϵ��
		switch (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType)
		{
		  case E_DEVICE_PEN_TOUCH_WHITEBOARD:
			  strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
			break;
		  case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
			   strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
			   break;
		  case E_DEVICE_FINGER_TOUCH_CONTROL:
			  strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_FingerTouchControl.cameraParams.Prop_VideoProcAmp_Brightness);
			  break;
		  case E_DEVICE_PALM_TOUCH_CONTROL:
			  strBrightnessCoefficient.Format(_T("%d"),lensCfg.normalUsageSettings_PalmTouchControl.cameraParams.Prop_VideoProcAmp_Brightness );
			  break;
		   default:
			  break;
		}

		GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->SetWindowText(strBrightnessCoefficient);

		strBrightnessCoefficient.Format(_T("%d"), lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness);
		GetDlgItem(IDC_EDIT_INSTALLBRIGHTNESS)->SetWindowText(strBrightnessCoefficient);
		CString  strGammaCoefficient;
		strGammaCoefficient.Format(_T("%d"), lensCfg.installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma);

		GetDlgItem(IDC_EDIT_INSTALLGAMMA)->SetWindowText(strGammaCoefficient);


		/////���´��ط�ʽ
		switch (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.m_eTouchType)
		{
		   case E_DEVICE_PEN_TOUCH_WHITEBOARD:

			   ((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(true);
			   ((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

			   ((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);
			    break;
		   case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

			   ((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(true);

			   ((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);
			    break;
		   case E_DEVICE_FINGER_TOUCH_CONTROL:

			   ((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

			   ((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(true);
			   ((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(false);
			    break;
		   case E_DEVICE_PALM_TOUCH_CONTROL:

			   ((CButton*)GetDlgItem(IDC_RADIO_PEN_TOUCH))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_FINGER_TOUCH))->SetCheck(false);

			   ((CButton*)GetDlgItem(IDC_RADIO_FINGERTOUCHCONTROL))->SetCheck(false);
			   ((CButton*)GetDlgItem(IDC_RADIO_PALMTOUCHCONTROL))->SetCheck(true);
			    break;
		   default:
			    break;
		}

		CString  autoCalibrateExpectedBrightness;
		autoCalibrateExpectedBrightness.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateExpectedBrightness);
		GetDlgItem(IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS)->SetWindowText(autoCalibrateExpectedBrightness);

		CString autoCalibrateHilightGray;
		autoCalibrateHilightGray.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateHilightGray);
		GetDlgItem(IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY)->SetWindowText(autoCalibrateHilightGray);

		CString VideoDisplayDelay;
		VideoDisplayDelay.Format(_T("%d"), lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.videoDislayDelay);
		GetDlgItem(IDC_EDIT_VIDEODISPLAYDELAY)->SetWindowText(VideoDisplayDelay);

		///����ǽ�������Ĳ�ͬ�����в���������
		if(m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bIsDynamicMaskFrame)
		{
			((CButton*)GetDlgItem(IDC_CHECK_DYNAMICMASKFRAMECONTROL))->SetCheck(true);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_DYNAMICMASKFRAMECONTROL))->SetCheck(false);
		}

		if(m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bIsAntiJamming)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL))->SetCheck(true);
		}
		else {
			((CButton*)GetDlgItem(IDC_CHECK_ANTIJAMMINGCONTROL))->SetCheck(false);
		}

		if (m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode].advanceSettings.bDisableReflectionSpot)
		{
			((CButton*)GetDlgItem(IDC_CHECK_DISABLEREFLECTIONPOINT))->SetCheck(true);
		}
		else {
			((CButton*)GetDlgItem(IDC_CHECK_DISABLEREFLECTIONPOINT))->SetCheck(false);
		}


		//���µ�������ȥ
		UpdateData(TRUE);
		SetModified(TRUE);
	}
}

void CAdvancedSettingsDlg::OnBnClickedRadioPenTouch()
{
	// TODO: Add your control notification handler code here
	//////�ж���ǽ�滹������
	TSensorModeConfig* pSensorModeConfig = NULL;
	pSensorModeConfig = &m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

    TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];
	if (IsDlgButtonChecked(IDC_RADIO_PEN_TOUCH) == BST_CHECKED)
	{
        //�ʴ�ģʽ��,���ܹ����ù�߱���
		GetDlgItem(IDC_EDIT_SPOTPROPORTION)->EnableWindow(FALSE);
		
        //GetDlgItem(IDC_EDIT_MULT_ERASER)->EnableWindow(FALSE);


        //��������ʹ��ʱ������ϵ��
        CString strBrightnessCoefficient;//����ϵ��
        strBrightnessCoefficient.Format(_T("%d"), lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness);
        GetDlgItem(IDC_EDIT_SET_NORMALUSAGE_BRIGHTNESS_COEFFICIENT)->SetWindowText(strBrightnessCoefficient);


        //���µ�������ȥ
		UpdateData(TRUE) ;
        SetModified(TRUE);
	}
}

void CAdvancedSettingsDlg::OnEnChangeEditAutomaskdetectthreshold()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bInitDone)
	{
		return;
	}
	TSensorModeConfig* pSensorModeConfig = NULL;

	pSensorModeConfig = & m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

    TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[m_pSensor->GetCameraType()][m_tSensorConfig.eSelectedLensType];

	CString StrText;

	GetDlgItem(IDC_EDIT_AUTOMASKDETECTTHRESHOLD)->GetWindowText(StrText);

	int npos = _ttoi(StrText);
	BOOL  bForceValidate = FALSE;

	if (npos < AUTOMASKDETECTTHRESHOLD_MIX)
	{
		npos = AUTOMASKDETECTTHRESHOLD_MIX;
		bForceValidate = TRUE;
	}
	//if (npos > m_nThreshold)
	switch (pSensorModeConfig->advanceSettings.m_eTouchType)
	{
	    case E_DEVICE_PEN_TOUCH_WHITEBOARD:
			if (npos > lensCfg.normalUsageSettings_PenTouchWhiteBoard.cYThreshold)
			{
				npos = lensCfg.normalUsageSettings_PenTouchWhiteBoard.cYThreshold;
				bForceValidate = TRUE;
			}
		    break;
		case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
			if (npos > lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cYThreshold)
			{
				npos = lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cYThreshold;
				bForceValidate = TRUE;
			}
			break;
		case E_DEVICE_FINGER_TOUCH_CONTROL:
			if (npos > lensCfg.normalUsageSettings_FingerTouchControl.cYThreshold)
			{
				npos = lensCfg.normalUsageSettings_FingerTouchControl.cYThreshold;
				bForceValidate = TRUE;
			}
			break;
		case E_DEVICE_PALM_TOUCH_CONTROL:
			if (npos > lensCfg.normalUsageSettings_PalmTouchControl.cYThreshold)
			{
				npos = lensCfg.normalUsageSettings_PalmTouchControl.cYThreshold;
				bForceValidate = TRUE;
			}
			break;
		default:
			break;

	}

	if (bForceValidate)
	{
		StrText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_AUTOMASKDETECTTHRESHOLD)->SetWindowText(StrText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CHANGE_AUTOMASKDETECTTHRESHOLD,(WPARAM)0,(LPARAM)npos);

	}
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnBnClickedCheckRecordVideo()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_RECORD_VIDEO) == BST_CHECKED)
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_RECORD_VIDEO,(WPARAM)0, (LPARAM)TRUE);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_RECORD_VIDEO,(WPARAM)0, (LPARAM)FALSE);
	}
    SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnEnChangeEditSetAutocalibrateAverageBrightness()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS)->GetWindowText(strText);
	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < AUTOCALIBRATION_BRIGHTNESS_MIX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MIX;
		bForceValidate = TRUE;
	}
	if (npos > AUTOCALIBRATION_BRIGHTNESS_MAX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MAX;
		bForceValidate = TRUE;
	}

	if ((bForceValidate))
	{
		strText.Format(_T("%d"),npos);
		GetDlgItem(IDC_EDIT_SET_AUTOCALIBRATE_AVERAGE_BRIGHTNESS)->GetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CHANGE_AUTOCALIBRIATION_AVERAGE_BRIGHTNESS,(WPARAM)0,(LPARAM)npos);
	}

    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnEnChangeEditAutocalibrateHilightGray()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!m_bInitDone)
	{
		return ;
	}

	CString strText;
	GetDlgItem(IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE ;

	if (npos < AUTOCALIBRATION_BRIGHTNESS_MIX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MIX ;
		bForceValidate = TRUE ;
	}
	if (npos > AUTOCALIBRATION_BRIGHTNESS_MAX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MAX ;
		bForceValidate = TRUE ;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"), npos) ;
		GetDlgItem(IDC_EDIT_AUTOCALIBRATE_HILIGHT_GRAY)->GetWindowText(strText) ;
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd() , WM_CHANGE_AUTOCALIBRIATION_LIGHTGRAY , (WPARAM)0 ,(LPARAM)npos) ;
	}
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnBnClickedCheckRearProjector()
{
	// TODO: Add your control notification handler code here
	if (IsDlgButtonChecked(IDC_CHECK_REAR_PROJECTOR) ==BST_CHECKED)
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_REAR_PROJECTION ,(WPARAM)0,(LPARAM)TRUE);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_REAR_PROJECTION ,(WPARAM)0,(LPARAM)FALSE);
	}
	
    SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnCbnSelchangeComboMonitorList()
{
    // TODO: Add your control notification handler code here
}


const TSensorConfig&   CAdvancedSettingsDlg::GetSensorConfig()const
{
    return this->m_tSensorConfig;
}

const GlobalSettings& CAdvancedSettingsDlg::GetGlobalSettings()const
{
    return this->m_tGlobalSettings;
}


void CAdvancedSettingsDlg::OnBnClickRadioLensType(UINT uID)
{
    if (IsDlgButtonChecked(uID) == BST_CHECKED)
    {
        ELensType lensType = (ELensType)(uID - IDC_RADIO_THROW_RATIO_015);

        if(lensType != m_tSensorConfig.eSelectedLensType)
        {
            //�����ͷ����
            m_tSensorConfig.eSelectedLensType  = lensType;

            ////���¿ؼ���ʾ����
            UpdateData(FALSE);

            ////ʹ��Ӧ�ð�ť
            SetModified(TRUE);
        }
    }

}

void CAdvancedSettingsDlg::OnBnClickedButtonAttachToAScreen()
{
	// TODO: Add your control notification handler code here

	CAttachToScreenDlg attachToScreenDlg(
		m_tSensorConfig.bAutoAttachMonitor,
		m_tGlobalSettings.bDoubleScreenTouchMerge,
		m_tSensorConfig.nAttachedMonitorId,
		m_tSensorConfig.eMonitorAreaType,

		this);


	if (IDOK == attachToScreenDlg.DoModal())
	{
		m_tSensorConfig.bAutoAttachMonitor = attachToScreenDlg.IsAutoAttachMode();
		m_tSensorConfig.nAttachedMonitorId = attachToScreenDlg.GetMonitorId();
		m_tSensorConfig.eMonitorAreaType = attachToScreenDlg.GetMonitorAreaType();
		m_tGlobalSettings.bDoubleScreenTouchMerge = attachToScreenDlg.IsDoubleScreenTouchMerge();

		//ʹ��Ӧ�ð�ť
		SetModified(TRUE);
	}//if
}


BOOL CAdvancedSettingsDlg::OnApply()
{
	UpdateData(TRUE);
	CPropertyPage::OnApply();
	SetModified(FALSE);

	//֪ͨ������Ӧ������
	::SendMessage(
		AfxGetMainWnd()->GetSafeHwnd(),
		WM_APPLY_SENSOR_CONFIG,
		(WPARAM)this,
		(LPARAM)m_pSensor);
	//
	return TRUE;
}

void CAdvancedSettingsDlg::OnBnClickedCheckDynamicmaskframecontrol()
{
	////ʹ��Ӧ�ð�ť
	SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnBnClickedCheckAntijammingcontrol()
{
	// TODO: Add your control notification handler code here
	////ʹ��Ӧ�ð�ť
	SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnBnClickedCheckSinglepointmode()
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnBnClickedCheckDisablereflectionpoint()
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnEnChangeEditVideodisplaydelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CScrollablePropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_VIDEODISPLAYDELAY)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if(npos <VIDEODISPLAYDELAY_MIN )
	{
		npos = VIDEODISPLAYDELAY_MIN;
		bForceValidate = TRUE;
	}
	if(npos >VIDEODISPLAYDELAY_MAX)
	{
		npos = VIDEODISPLAYDELAY_MAX;
		bForceValidate = TRUE;
	}
	if(bForceValidate)
	{
		strText.Format(_T("%d"), npos);
		GetDlgItem(IDC_EDIT_VIDEODISPLAYDELAY)->GetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_VIDEODISPLAYDELAY, (WPARAM)0, (LPARAM)npos);
	}

	SetModified(TRUE);
}


void CAdvancedSettingsDlg::OnChangeEditInstallbrightness()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CScrollablePropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_INSTALLBRIGHTNESS)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < AUTOCALIBRATION_BRIGHTNESS_MIX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MIX;
		bForceValidate = TRUE;
	}
	if (npos > AUTOCALIBRATION_BRIGHTNESS_MAX)
	{
		npos = AUTOCALIBRATION_BRIGHTNESS_MAX;
		bForceValidate = TRUE;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"), npos);
		GetDlgItem(IDC_EDIT_INSTALLBRIGHTNESS)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_INSTALL_BRIGHTNESS, (WPARAM)0, (LPARAM)npos);
	}
	SetModified(TRUE);

}


void CAdvancedSettingsDlg::OnChangeEditInstallgamma()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CScrollablePropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (!m_bInitDone)
	{
		return;
	}
	CString strText;
	GetDlgItem(IDC_EDIT_INSTALLGAMMA)->GetWindowText(strText);

	int npos = _ttoi(strText);
	int bForceValidate = FALSE;

	if (npos < MIN_GAMMA_VAL)
	{
		npos = MIN_GAMMA_VAL;
		bForceValidate = TRUE;
	}
	if (npos > MAX_GAMMA_VAL)
	{
		npos = MAX_GAMMA_VAL;
		bForceValidate = TRUE;
	}

	if (bForceValidate)
	{
		strText.Format(_T("%d"), npos);
		GetDlgItem(IDC_EDIT_INSTALLGAMMA)->SetWindowText(strText);
	}
	else
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CHANGE_INSTALL_GRAMMA, (WPARAM)0, (LPARAM)npos);
	}
	SetModified(TRUE);
}

void CAdvancedSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CScrollablePropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
	if (!m_bInitDone)
	{
		return;
	}
	/*int npos =  m_ctrlSliderSmooth.GetPos();
	CString strText;
	strText.Format(_T("%d"), npos);
	GetDlgItem(IDC_EDIT_SMOOTH)->SetWindowTextW(strText);
	SetModified(TRUE);*/

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
