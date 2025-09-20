#pragma once

//////////////////////////////////////////////////////////////////////////
//
//***
#define PERMFLAGSET_A_REGION                        0x00000003
#define PERMFLAGSET_A_REGION_0                      0x00000001
#define PERMFLAGSET_A_REGION_1                      0x00000002
//#define PERMFLAGSET_A_REGION_2                      0x00000004
//#define PERMFLAGSET_A_REGION_3                      0x00000008

#define PERMFLAGSET_B_REGION                        0x00000070
#define PERMFLAGSET_B_REGION_0                      0x00000010
#define PERMFLAGSET_B_REGION_1                      0x00000020
#define PERMFLAGSET_B_REGION_2                      0x00000040
//#define PERMFLAGSET_B_REGION_3                      0x00000080

#define PERMFLAGSET_C_REGION                        0x00000100
#define PERMFLAGSET_C_REGION_0                      0x00000100
//#define PERMFLAGSET_C_REGION_1                      0x00000200
//#define PERMFLAGSET_C_REGION_2                      0x00000400
//#define PERMFLAGSET_C_REGION_3                      0x00000800

#define PERMFLAGSET_D_REGION                        0x00001000
#define PERMFLAGSET_D_REGION_0                      0x00001000
//#define PERMFLAGSET_D_REGION_1                      0x00002000
//#define PERMFLAGSET_D_REGION_2                      0x00004000
//#define PERMFLAGSET_D_REGION_3                      0x00008000

#define PERMFLAGSET_E_REGION                        0x00070000
#define PERMFLAGSET_E_REGION_0                      0x00010000
#define PERMFLAGSET_E_REGION_1                      0x00020000
#define PERMFLAGSET_E_REGION_2                      0x00040000
//#define PERMFLAGSET_E_REGION_3                      0x00080000

#define PERMFLAGSET_F_REGION                        0x00F00000
//#define PERMFLAGSET_F_REGION_0                      0x00100000
//#define PERMFLAGSET_F_REGION_1                      0x00200000
//#define PERMFLAGSET_F_REGION_2                      0x00400000
//#define PERMFLAGSET_F_REGION_3                      0x00800000

#define PERMFLAGSET_G_REGION                        0x0G000000

#define PERMFLAGSET_H_REGION                        0xH0000000
//***

/*
@��ʽ��ʹ��һ��DWORD�洢����ʶ�����ֵ
@���ܣ��洢�¼��Ƿ��������ı�־λ
@ʹ������䣺
|0000|0000|0000|0000|0000|0000|0000|0000|
| H  | G  | F  | E  | D  |  C |  B | A  |

A���򡪡���Ӧ������
A = 0 , ��ζ�����������еĹ���ȫ����ֹ��
A = 1 , ��ζ���������ˮƽ���ֹ��������������ܽ�ֹ��
A = 2 , ��ζ�����������ֱ���ֹ��������������ܽ�ֹ��
A = 3 , ��ζ���������ˮƽ����ֱ���ֹ��������������ܽ�ֹ��
A = ...

B���򡪡���Ӧ3 ~ 5����
B = 0 , ��ζ��3 ~ 5�������й���ȫ����ֹ��
B = 1 , ��ζ��3 ~ 5����Ĵ����л����ܿ�����
B = 2 , ��ζ��3 ~ 5�������󻯵�ǰ���ڹ��ܿ�����
B = 4 , ��ζ��3 ~ 5�������С����ǰ���ڹ��ܿ�����
B = 8 , ��ζ��3 ~ 5�����ALT_RARROW���ܿ�����

C���򡪡���Ӧ6�����ϵĴ���
C = 0 , ��ζ��6�����ϴ������й���ȫ����ֹ��
C = 1 , ��ζ��6�����ϴ������ʾ���湦�ܿ���

D���򡪡���Ӧ�����±�Եλ��
D = 0 , ��ζ���±�Եλ�����й���ȫ����ֹ��
D = 1 , ��ζ���±�Եλ��ƽ�̴��ڹ��ܿ�����

E���򡪡���Ӧ���Ʒ�����λ��
E = 0 , ��ζ�����Ʒ�����λ�����й���ȫ����ֹ��
E = 1 , ��ζ�����Ʒ�����λ�ùرյ�ǰ���ڹ��ܿ�����
E = 2 , ��ζ�����Ʒ�����λ��ˢ�´��ڹ��ܿ�����
E = 4 , ��ζ�����Ʒ�����λ���ƶ����ڹ��ܿ�����
//E = 8 , ��ζ�����Ʒ�����λ�ñ����ĵ����ܿ�����
*/

typedef enum
{
    //////////////////////////////////////////////////////////////////////////
    E_TE_ALL,
    E_TE_PART,                      //��E_TE_ALL��Բ���⣬�����������������в��ֵ����״̬
    //��ָ��״
    E_TEF_ALL,                      //������ָ��״֧���¼�
    E_TEF_PART,                     //��E_TEF_ALL��Բ����

    E_TEF_2TOUCHS_ALL,              //�����������¼�
    E_TEF_2TOUCHS_PART,
    E_TEF_2TOUCHS_HMW,              //������ˮƽ������
    E_TEF_2TOUCHS_VMW,              //�����㴹ֱ������

    E_TEF_35TOUCHS_ALL,             //3 ~ 5���������¼�
    E_TEF_35TOUCHS_PART,
    E_TEF_35TOUCHS_SWHWND,            //3 ~ 5���㴰���л�    
    E_TEF_35TOUCHS_MAXICURWND,      //3 ~ 5������󻯵�ǰ����
    E_TEF_35TOUCHS_MINICURWND,      //3 ~ 5������С����ǰ����
    E_TEF_35TOUCHS_ALTRARROW,       //3 ~ 5����Alt+RArrow

    E_TEF_6MTOUCHS_ALL,             //6���ϴ��������¼�
    E_TEF_6MTOUCHS_PART,
    E_TEF_6MTOUCHS_DESPDESKTOP,     //6���ϴ�����ʾ����
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //������״
    E_TEP_ALL,                      //����������״֧���¼�
    E_TEP_PART,

    E_TEP_LOWLIM_ALL,               //�±�Եλ�������¼�
    E_TEP_LOWLIM_PART,
    E_TEP_LOWLIM_SHOWWNDSBS,        //�±�Եˮƽƽ�̴���

    E_TEP_NSPLOC_ALL,               //������λ�������¼�
    E_TEP_NSPLOC_PART,
    E_TEP_NSPLOC_CLSCURWND,         //�رյ�ǰ����
    E_TEP_NSPLOC_REFRESHWND,        //ˢ�´���
    E_TEP_NSPLOC_MOVWND,            //�ƶ�����
    E_TEP_NSPLOC_SVDOC,             //�����ĵ�
    //////////////////////////////////////////////////////////////////////////
}E_TRIGGEREVENT;

#ifndef INIT_WITH_ALL_PERMITED
#define INIT_WITH_ALL_PERMITED  (PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif

#ifndef INIT_WITH_ALL_PROHIBITED
#define INIT_WITH_ALL_PROHIBITED    0
#endif

#ifndef INIT_WITH_FINGER_PERMITED
#define INIT_WITH_FINGER_PERMITED  (PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION)
#endif

#ifndef INIT_WITH_PALM_PERMITED
#define INIT_WITH_PALM_PERMITED  (PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif

#ifndef ALLPERMITED_EXCEPT_2TOUCHS
#define ALLPERMITED_EXCEPT_2TOUCHS (PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION)
#endif