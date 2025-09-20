#pragma once
HRESULT FindPin(IBaseFilter* pFilter, PIN_DIRECTION pindir, LPCWSTR pPinName, IPin** ppPin);

//@��  ��:����ָ�����豸·��������Ƶ�����豸
//@��  ��:ppSrcFilter, ָ�򱣴�"��������(capture filter)"����ָ����ڴ��ַ��
//       lpszDevPath, �豸·����
//@����ֵ:S_OK, �ɹ���
//        ����, ʧ��
//@˵��:����豸·��Ϊ��, �򷵻ص�һ���ҵ�����Ƶ�����豸����
// 
HRESULT FindVideoCaptureDevice(IBaseFilter **ppSrcFilter, LPCWSTR lpszDevPath=NULL);


HRESULT  PrintFilterGraph(IFilterGraph* pFG);
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveFromRot(DWORD pdwRegister);

 void PrintObjRef(IUnknown* pUnk);

HRESULT GetNextFilter(
    IBaseFilter *pFilter, // Pointer to the starting filter
    PIN_DIRECTION Dir,    // Direction to search (upstream or downstream)
    IBaseFilter **ppNext); // Receives a pointer to the next filter.

// Define a typedef for a list of filters.
typedef CGenericList<IBaseFilter> CFilterList;
//Find all the immediate upstream or downstream peers of a filter.	
HRESULT GetPeerFilters(
    IBaseFilter *pFilter,     // Pointer to the starting filter
    PIN_DIRECTION Dir,        // Direction to search (upstream or downstream)
    CFilterList &FilterList); // Collect the results in this list.


//@����: ��ȡVideoProcAmp������ֵ
//@����:pCaptureFilter, ָ��Video Capture Filter�����ָ�� 
//      pVideoProcAmpProp, ָ�򱣴�VideoProcAmp���Ե����ݻ�������ָ��
BOOL ReadVideoProcAmpProperty(IBaseFilter *pCaptureFilter, TVideoProcAmpProperty* pVideoProcAmpProp);


//@����: ����VideoProcAmp������ֵ
//@����:pCaptureFilter, ָ��Video Capture Filter�����ָ�� 
//      pVideoProcAmpProp, ָ������VideoProcAmp���Ե����ݻ�������ָ��
BOOL SetVideoProcAmpProperty(IBaseFilter *pCaptureFilter, const TVideoProcAmpProperty* pVideoProcAmpProp);
BOOL IRCUTSwtich(IBaseFilter *pCaptureFilter,BOOL bOn,int PID ,int VID);

//@����:GPIO����
BOOL GPIOControl(IBaseFilter *pCaptureFilter, BOOL bOn);