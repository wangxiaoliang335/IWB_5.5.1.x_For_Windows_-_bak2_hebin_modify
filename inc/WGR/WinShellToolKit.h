#pragma once

/*
  ʹ����ʹ��ǰ��Ҫͨ��CoInitialize����ʼ��com������
  �����Ҫ��ʹ�ú���Ҫͨ��CoUninitialize������ʼ��com����
*/

//////////////////////////////////////////////////////////////////////////
//�궨��
typedef enum
{
    ESCRLT_UP   , //��ֱ����
    ESCRLT_DOWN , //��ֱ����
    ESCRLT_LEFT , //ˮƽ����
    ESCRLT_RIGHT, //ˮƽ����
}E_ScroolType;

class CWinShellInstances;

class CWinShellUtility
{
public:
    CWinShellUtility();
    ~CWinShellUtility();

public:
    //������ʾ����
    bool ShowWindowsSidebySide();
    //��С��ȫ������
    bool MinimizeAllWindows();
    bool UndoMinizeAllWindows();
    //�����л�
    bool SwitchWindows(char *shortCut);

    //ˮƽ������ǰ�˴���
    bool HScroolForegroundWindow(E_ScroolType scollDir);

    //��ֱ������ǰ�˴���
    bool VScroolForegroundWindow(E_ScroolType scollDir);

    //��С����ǰ����
    bool MiniCurrentWindow(const POINT &ptCur);

    //��󻯵�ǰ����
    bool MaxiCurrentWindow(const POINT &ptCur);

    //��ʾ����
    bool DisplayDesktop(const POINT &ptCur);
   
private:
    //��ȡ�������ȴ���
    HWND GetTopParent(const POINT &ptCur);

private:
    CWinShellInstances *m_shell_inst;
};