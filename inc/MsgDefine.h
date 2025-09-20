#pragma once
#define WM_MANUAL_CALIBRATE_DATA       (WM_USER+1)
//�ֶ�У�����֪ͨ��Ϣ
//WPARAM, 1:�ɹ�; 0:ʧ��
//LPARAM, ����
#define WM_FINISH_MAUNUAL_CALIBRATING  (WM_USER+2)
//#define WM_ABORT_MANUAL_CALIBRATING  (WM_USER+3)
#define WM_GRAPHNOTIFY                 (WM_USER+4)

//֡��֪ͨ��Ϣ
//WPARAM, ��ǰ֡��
//LPARAM, ����ͷ���
#define WM_FPSNOTIFY                 (WM_USER+5)



//����У������������Ϣ
#define WM_CALC_ADJUST_MATRIX (WM_USER+6)

//���ü������
//WPARAM ����ֵ 0~100
#define WM_SET_DETECT_THRESHOLD (WM_USER+7)

//�Զ�У������
#define WM_AUTO_CALIBRATE_DATA  (WM_USER+8)


//�Զ�У�����
#define WM_AUTO_CALIBRATE_DONE  (WM_USER+9)

#define TRAY_NOTIFY_MSG      (WM_USER + 10)
#define  CLUTTER_DETECTION_DONE_MSG  (WM_USER + 11)

//��ʼ�׶η�������, �Զ����ù��
#define WM_INITIAL_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER (WM_USER + 12)


//���õ�����λ��
//WPARAM, ��һλ0��ʾ��һ��������,1��ʾ�ڶ���������
//LPARAM, ���ֱ�ʾ��ֱλ��
#define WM_CHANGE_GUIDELINE_Y_POS (WM_USER + 13)


//���õ�����λ��
//WPARAM, ��һλ0��ʾ��һ��������,1��ʾ�ڶ���������
//LPARAM, ��һλ��ʾ�Ƿ���ʾ������
#define WM_SHOW_GUIDELINE (WM_USER + 14)

//����ȡ��ͼ�����Ϣ
//#define  WM_CALIBRATE_FRAME_DATA  (WM_USER+15)
//���òɼ����
#define  WM_COLLECT_SPOT_DATA     (WM_USER+16) 

//��ɲɼ���ߵĹ���
//WPARAM,��������
#define  WM_FINISH_COLLECTSPOT    (WM_USER+17) 

//�жϲɼ���ߵĹ���
#define  WM_BREAK_COLLECTSPOT     (WM_USER+18)

#define  WM_CHANGE_SPOTPROPORTION  (WM_USER+19)
#define  WM_CHANGE_MULTERASER      (WM_USER+20)


//������Ļ��������
//WPARAM, 0
//LPARAM, 0:���ʧ��
//               1:���ɹ�
#define WM_SEARCH_SCREEN_AREA_DONE (WM_USER+21)


//�ֶ��˹���������
//WPARAM, 0
//LPARAM, 0: ʧ��
//               1: �ɹ�
#define WM_MANUAL_CORRECT_DONE     (WM_USER+22)

#define WM_MANUAL_COLLECT_SPOT      (WM_USER+23)
#define WM_DISPLAYWINDOW             (WM_USER+24)
#define  WM_CHANGE_FIXEDBOLBSETTIME  (WM_USER+25)

//////�����еĹ����У������й̶��Ĺ�ߴ��ڣ�

#define WM_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER (WM_USER + 26)
#define WM_CHANGE_AUTOMASKDETECTTHRESHOLD     (WM_USER + 27)
//<<added by toxuke@gmail.com, 2013/04/19
//#define WM_CHANGE_ENABLE_GESTURE_RECOGNITION (WM_USER + 27)
//>>
#define WM_CHANGE_NORMALUSER_BRIGHTNESS (WM_USER + 28)

#define WM_CHANGE_ENABLE_GESTURE_TOUCH        (WM_USER + 29)
#define WM_CHANGE_ENABLE_PEN_TOUCH            (WM_USER + 30)
#define WM_RECORD_VIDEO                       (WM_USER + 31)

#define WM_CHANGE_AUTOCALIBRIATION_AVERAGE_BRIGHTNESS  (WM_USER + 32)
#define WM_CHANGE_AUTOCALIBRIATION_LIGHTGRAY           (WM_USER+ 33)
#define WM_REAR_PROJECTION                             (WM_USER+ 34)
#define WM_CHANGE_VIDEODISPLAYDELAY                    (WM_USER+ 35)


//����ͷ״̬
//WPARAM, LPCTSTR�ַ���
//LPARAM, ����ͷ���
#define WM_CAMERA_STATUS_NOTIFY (WM_USER + 35)


//////////////////////////////////////////////////////////////////////////
//������Ƶ��ʾ���ڣ���Ҫ�ڰ�װ����ʱ������Ƶ�滭������������
#define WM_CHANGE_DISPLAY_WINDOW          (WM_USER + 36)
////////////////

//////////////////////////////////////////////////////////////////////////
#define WM_PUT_DETECT_BACKSPLASH_DATA       (WM_USER + 37)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//��װ�򵼹����У�֪ͨ��ʼ/ֹͣ�Զ�������Ƿ���ʧ
#define WM_START_DETECT_BACKSPLASH_VANISH   (WM_USER + 38)
#define WM_STOP_DETECT_BACKSPLASH_VANISH    (WM_USER + 39)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//���û�ʹ��ĳ����
#define WM_ENABLEORDISABLE_WINDOW           (WM_USER + 40)
//////////////////////////////////////////////////////////////////////////




//added by 15077726@qq.com, 2015/11/3
//<<У�����ȵ���ʹ��
#define WM_CALIBRATE_MANUAL_RESAMPLE       (WM_USER + 41)
//>>


#define WM_APPLY_SENSOR_CONFIG            (WM_USER + 42)


#define WM_CREEN_LAYOUT_DESIGN_BUTTON_CLICK       (WM_USER + 43)

//�ĵ�궨������־
#define WM_END_4_BASE_POINT_CALIBRATE       (WM_USER + 44)

//����ͷ��������
#define WM_CHANGE_INSTALL_BRIGHTNESS  (WM_USER+ 45)
#define WM_CHANGE_INSTALL_GRAMMA      (WM_USER+ 46)

#define WM_TIP_TEXT (WM_USER+50)