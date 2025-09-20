#pragma once

#ifdef HOTKEYMONITOR_EXPORTS
#define HOTKEY_API _declspec(dllexport)
#else 
#define HOTKEY_API _declspec(dllimport)
#endif

//@����:ע��һ���ȼ�, ��ϵͳ�м�⵽��������, ָ���Ĵ��ڻ���յ�WM_HOTKEY��Ϣ
//@����:hWnd, ����WM_HOTKEY��Ϣ�Ĵ��壬���hWnd == NULL, ��WM_HOTKEY���͸�����AddHotkey���̵߳���Ϣ�����С�
//      id, �ȼ���Ψһ��ʶ����Ϊ�˱����ͻ��ʹ��GlobalAddAtom�������ַ����������id
//      fsModifiers, ����������ֵ�����
//                 MOD_ALT Either ALT key must be held down. 
//                 MOD_CONTROL Either CTRL key must be held down. 
//                 MOD_KEYUP Both key up events and key down events generate a WM_HOTKEY message 
//                 MOD_SHIFT Either SHIFT key must be held down. 
//                 MOD_WIN Either WINDOWS key was held down. These keys are labeled with the Microsoft Windows logo. 
//     vk,�����ֵ
extern "C"  HOTKEY_API BOOL AddHotKey(HWND hWnd, int id, UINT fsModifiers, UINT vk);

//@����:�Ƴ�һ���ȼ�,
//@����:id, �ȼ��ı�ʶ��
extern "C"  HOTKEY_API BOOL RemoveHotKey(int id);

//@����:�����ȼ����
extern "C" HOTKEY_API BOOL StartHotKeyMonitor();


//@����:ֹͣ�ȼ����
extern "C" HOTKEY_API BOOL StopHotKeyMonitor();