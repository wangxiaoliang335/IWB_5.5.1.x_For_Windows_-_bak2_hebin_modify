#pragma once

#include <string>
#include <list>
#include <map>
#include "..\..\..\inc\wgr\WGRPermissionPublic.h"


//typedef enum
//{
//    E_MT_NONE       ,//δ�޸�
//    E_MT_ADD        ,//������
//    E_MT_DEL        ,//��ɾ��
//    E_MT_UPDATE     ,//����
//}E_MODIFIED_TYPE;
#define KSTANDAR_XSCRNRESOLUTION        1024
#define KSTANDAR_YSCRNRESOLUTION        768
//#define KSTANDAR_XSCRNPHYSIZE    85
//#define KSTANDAR_YSCRNPHYSIZE    85
#define KSTANDAR_SCRNDLGPHYSICALLENGTH  (80.0*25.4f)

typedef struct _ST_WGR_ACTUALPARAMS
{
    //��Ļ�ֱ���
    int xScreenResolution;
    int yScreenResolution;
    //��Ļ����ߴ�
    float     fScreenDiagonalPhysicalLength;//�����Խ�������ߴ�,��λ:����(mm);
}ST_WGR_ACTUALPARAMS;

typedef struct _ST_WGR_EXCEPTION
{
    std::string procName;
    std::string apppName;
    DWORD		permissionSets;	
    //E_MODIFIED_TYPE modifiedType;

    _ST_WGR_EXCEPTION()
    {
        permissionSets = 0;
        //modifiedType = E_MT_NONE;
    }
}ST_WGR_EXCEPTION;

typedef struct _ST_WGR_PERMISSION_CONFIG
{
    DWORD generalPermissionSets;
    std::list<ST_WGR_EXCEPTION> lstExceptions;

    _ST_WGR_PERMISSION_CONFIG()
    {
        //Ĭ�ϳ��ó���֧����������
        generalPermissionSets = PERMFLAGSET_A_REGION | PERMFLAGSET_B_REGION | PERMFLAGSET_C_REGION 
                        | PERMFLAGSET_D_REGION | PERMFLAGSET_E_REGION;

        //lstExceptions.resize(4);        
    }
}ST_WGR_PERMISSION_CONFIG;

typedef struct _ST_WGR_CONFIGDATA
{
    ST_WGR_PERMISSION_CONFIG permissionConfig;
    bool                     isLogPermitted;
    int                      multiplePalmtoStd;          //���ƹ�����׼������ߵı���
}ST_WGR_CONFIGDATA;

typedef struct _ST_GR_WEIGHTCONF
{//Ҫ��k0������k1
    byte k0;
    byte k1;
}ST_GR_WEIGHTCONF; 

//////////////////////////////////////////////////////////////////////////
//GLBoard
typedef struct _ST_GLBOARDGR_CONFIG
{
    /*std::string procName;
    std::string apppName;*/
    std::string clsName;
    std::string wndTitle;    
    DWORD		permissionSets;
}ST_GLBOARDGR_CONFIG;

typedef struct _ST_GLBOARDGR_CONFIGDATA
{
    //ST_GLBOARDGR_CONFIG permissionConfig;
    bool                isGLBoardGRPermit;
    bool                isGLBoardGRLogPermitted;
    int                 multipleEraserToStd;          //���ƹ�����׼������ߵı���

    _ST_GLBOARDGR_CONFIGDATA()
    {
        isGLBoardGRPermit = true;
        isGLBoardGRLogPermitted = false;
        multipleEraserToStd = 4;
    }
}ST_GLBOARDGR_CONFIGDATA;

typedef struct _ST_GR_CONFIGDATA
{
    ST_WGR_CONFIGDATA       wgrConfig;
    ST_GLBOARDGR_CONFIGDATA glboardConfig;
}ST_GR_CONFIGDATA;

class CWGRCofig
{
public:
    CWGRCofig(void);
    ~CWGRCofig(void);

public:
    bool IsLogPermitted()
    {
        return m_wgr_CfgData.isLogPermitted;
    }

    void SetLogPermitted(bool bPermssion)
    {
        m_wgr_CfgData.isLogPermitted = bPermssion;
    }

    void SetHIDMode(bool bIsTouchPad);

    int GetMultiplePalm2Std() {return m_wgr_CfgData.multiplePalmtoStd;}
    void SetMultiplePalm2Std(int multiple) {m_wgr_CfgData.multiplePalmtoStd = multiple;}

    //<added by Jiqw 2015/05/15 11:12
    //����δ��GLBoard�������������ļ���Ϊ��͵������ʱ��GLBoard�İ����ֵ���Ƿ���GLBoard�������÷���Windows���������ļ���
    /*
       @���ܣ���ȡ�����ð���ͱ�׼������ߵı�ֵ
    */
    int GetMultipleEraser2Std() {return m_glbgr_cfgData.multipleEraserToStd;}
    void SetMultipleEraser2Std(int multiple) {m_glbgr_cfgData.multipleEraserToStd = multiple;}

    /*
       @���ܣ���ȡ�������Ƿ���GLBoard����
    */
    bool IsGLBoardGRPermit() {return m_glbgr_cfgData.isGLBoardGRPermit;}
    void SetGLBoardGRPermission(bool bEnable) {m_glbgr_cfgData.isGLBoardGRPermit = bEnable;}
    //>

    /*
    @���ܣ���ȡ��ǰλ�ô������������������ֵ
    @������ptCur����ǰ��λ��
    @����ֵ����ǰ�������е�����ʶ�����ֵ
    */
    DWORD GetPermission(const POINT &pt);

    /*
    @���ܣ���ȡһ����̵�����ʶ�����ֵ    
    @����ֵ��һ����̵�����ʶ�����ֵ
    */
    DWORD GetGeneralPermission();    
    void SetGeneralPermission(const DWORD &perm);

    bool IsGerneralGRPermit() {return m_wgr_CfgData.permissionConfig.generalPermissionSets != INIT_WITH_ALL_PROHIBITED;}
    void SetGeneralPermission(bool bFlag) {
        if(bFlag) {m_wgr_CfgData.permissionConfig.generalPermissionSets = INIT_WITH_ALL_PERMITED;}
        else      {m_wgr_CfgData.permissionConfig.generalPermissionSets = INIT_WITH_ALL_PROHIBITED;}
    }
    /*
    @���ܣ���ȡ������̵�����ʶ�����ֵ
    @������procName�������������
    @����ֵ��������̵�����ʶ�����ֵ
    */
    DWORD GetExceptionPermission(const std::string &procName);
    void SetExcetionPermission(const DWORD &perm, const std::string &procName); 

    /*
    @���ܣ���ȡ��������б�
    */
    const std::list<ST_WGR_EXCEPTION> &GetExceptionalProcessList()  const;
    /*
    @���ܣ�����������
    @������procName�������������
           appName����Ϊ�ա����������������������
           perm��������̵����ֵ
    @����ֵ��true����ӳɹ���false�����ʧ��
    */
    bool AddExceptionalProcess(const std::string &procName, const std::string &appName, const DWORD &perm);
    /*
    @���ܣ�ɾ���������
    @������procName�������������    
    */
    void DeleteExceptionalProcess(const std::string &procName);

    bool IsTheProcessHasAdded(const std::string &procName);

    //int GetCurrentProcessInfo(std::string *procNameArry, const int &arrySize);
    int GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize);

    /*
        @������Ļ�ֱ����
    */
    void UpdateScreenResolutionParams(int nScreenWidth, int nScreenHeight)
    {
        m_wgr_ActualParams.xScreenResolution = nScreenWidth;
        m_wgr_ActualParams.yScreenResolution = nScreenHeight;    
    }

    /*
        @��������ߴ�
    */
    void UpdateScreenDiagonalPhysicalLength(float fPhysicalLength)
    {
        m_wgr_ActualParams.fScreenDiagonalPhysicalLength = fPhysicalLength;
    }

    int MapStandardX2ActualValue(int xStandard);
    int MapStandardY2ActualValue(int yStandard);

    /*
       @���ܣ���ȡ��Ȩ�Ĺ������ֵ
       @������nActualMass��ʵ�ʵĹ������ֵ��
       @������nExtRectArea����ߵ���Ӿ��������
       @������nStdAreaMulti,��׼�������������Ӧ������ı�׼�����ֵ

       @˵����
       //����nExtRectArea����Ӿ��ε�������Ҹ���Ӿ�����nActualMass���ϴ�����ʿ����ƶ�ʱ�����Բ������·�����ƽ��
       //
       @ƽ��ʱ��������nActualMass < nStdAreaMulti����nExtRectArea > nStdAreaMulti
       @������
       ��k0, k1Ϊ������ k0 < k1 �� 2*k0 > k1
       lambda = (mass / lAreaInVideo) * (k0 / k1)            

       T = (1 - lambda) * mass + lambda * lAreaInVideo

       T = [(k0+k1)*mass + k1 - 1] / k1 - (k0*mass*mass + k1*lAreaInVideo - 1) / k1*lAreaInVideo
       //
    */
    int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti);

    void SetDoubleScreen(bool isDoubleFlag);

    //Ĭ������£����������̵����ƹر��ˣ�����Ӧ���������̵�����Ҳ��ǿ�ƹر�
    bool IsAllGestureClosed();
    bool IsAllPalmBasedGestureClosed();
    bool IsAllFingerBasedGestureClosed();

	//added by 15077726@qq.com
	//<<2017/12/26
	//�пͻ�ֱ�Ӷϵ����������������CWGRCofigû�л��������������б����ļ���
	//�����Ҫ�����öԻ����������������á�
	BOOL Save();
	//>>
private:
    //@����:���������ļ�
    //@����:lpszConfigFilePath, �����ļ�������··��
    BOOL LoadConfig(LPCTSTR lpszConfigFilePath);


    //@����:���������ļ�
    //@����:lpszConfigFilePath, �����ļ������·��
    BOOL SaveConfig(LPCTSTR lpszConfigFilePath);


private:
    ST_WGR_CONFIGDATA   m_wgr_CfgData;
    ST_GR_WEIGHTCONF    m_gr_WeightConf;

    //<added by Jiqw 2015/05/15 11:12
    //����δ��GLBoard�������������ļ���Ϊ��͵������ʱ��GLBoard�İ����ֵ���Ƿ���GLBoard�������÷���Windows���������ļ���
    ST_GLBOARDGR_CONFIGDATA m_glbgr_cfgData;
    //>

    bool                m_bIsTouchPad;      //��ǰ�Ƿ��Ǵ���ģʽ������ģʽ�£�����������������

    ST_WGR_ACTUALPARAMS m_wgr_ActualParams; //��ǰ�����İװ���Ļʵ�ʲ���
    bool                m_isDoubleScreen;   //˫������£���ȡ�ķֱ���ֵҪ����
};

extern CWGRCofig g_oWGRConfig;
