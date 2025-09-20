#pragma once

#include "WGRPermissionPublic.h"

class CWGRPermissionParse
{
private:
    CWGRPermissionParse(void);
    ~CWGRPermissionParse(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //�����ֵ�����ض����ܵ������
    static bool IsTheEventPermited(E_TRIGGEREVENT evt, const DWORD &permissionSet);

    //////////////////////////////////////////////////////////////////////////
    //���ض����ܵ������ת��Ϊ���ֵ��
    //������permissionSet[in, out]��[in]��ǰ��permissionSetֵ��[out]����evt����Ӧ������Ժ��permissionSetֵ
    static void ConvertToPermissionSet(E_TRIGGEREVENT evt, bool isPermited, DWORD &permissionSet);
};
