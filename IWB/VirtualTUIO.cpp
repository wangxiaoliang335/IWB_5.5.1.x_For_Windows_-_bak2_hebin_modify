#include "stdafx.h"
#include "VirtualTUIO.h"


CVirtualTUIO::CVirtualTUIO()
	:tuioServer(NULL),
	tuioServer1(NULL),
	tcp_sender(NULL),
	tcp_sender1(NULL),
	m_nCxScreen(1920),
	m_nCyScreen(1080),
	szIP("127.0.0.1"),
	m_nPort(3333)
{
}

CVirtualTUIO::~CVirtualTUIO()
{
	if(tuioServer != NULL)
	{
	     CloseTUIOServer();
    }

	if (tuioServer1 != NULL)
	{
		CloseTUIOServer1();
	}
}

void CVirtualTUIO::OpenTUIOServer(bool bStart)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csForTUIO);

	if(bStart)
	{
	   LoadTUIOConfig();
	}
	if (tuioServer == NULL)
	{
		if ( (strcmp(szIP,"127.0.0.1")==0) && m_nPort ==3333)
		{
			/////UDP
	        tuioServer = new TuioServer();
		    ////TCP
	        tcp_sender = new TcpSender(m_nPort);
		}
		else {
			/////UDP
			tuioServer = new TuioServer(szIP, m_nPort);

			////TCP
			tcp_sender = new TcpSender(szIP, m_nPort);
		}
		if (tuioServer!= NULL)
		{
	        tuioServer->addOscSender(tcp_sender);
		    InitTuio();
		}
	}	

	if (tuioServer1 == NULL)
	{
		if ((strcmp(szIP, "127.0.0.1") == 0) && m_nPort == 3333)
		{
			/////UDP
			tuioServer1 = new TuioServer(true);
			////TCP
			tcp_sender1 = new TcpSender(m_nPort+1);
		}
		else {
			/////UDP
			tuioServer1 = new TuioServer(szIP, m_nPort+1);

			////TCP
			tcp_sender1 = new TcpSender(szIP, m_nPort+1);
		}
		if (tuioServer1 != NULL)
		{
			tuioServer1->addOscSender(tcp_sender1);
			InitTuio1();
		}
	}
}
void CVirtualTUIO::InitTuio()
{
	TuioTime::initSession();
	frameTime = TuioTime::getSessionTime();

	m_nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	m_nCyScreen = GetSystemMetrics(SM_CYSCREEN);

//	tuioServer->setSourceName("VirtualTUIO");
	tuioServer->enableCursorProfile(false);
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);
}

void CVirtualTUIO::InitTuio1()
{
	TuioTime::initSession();

	frameTime1 = TuioTime::getSessionTime();

	m_nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	m_nCyScreen = GetSystemMetrics(SM_CYSCREEN);

	tuioServer1->enableCursorProfile(false);
	tuioServer1->enableObjectProfile(false);
	tuioServer1->enableBlobProfile(false);
}

void CVirtualTUIO::CloseTUIOServer()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csForTUIO);

	Reset();
	if (tcp_sender !=NULL)
	{
	    delete tcp_sender;
		tcp_sender = NULL;
	}
	if (tuioServer!= NULL)
	{
	    delete tuioServer; 
		tuioServer = NULL;
	}

	if (tcp_sender1 != NULL)
	{
		delete tcp_sender1;
		tcp_sender1 = NULL;
	}
	if (tuioServer1 != NULL)
	{
		delete tuioServer1;
		tuioServer1 = NULL;
	}
}

void CVirtualTUIO::CloseTUIOServer1()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csForTUIO);

	Reset();
	if (tcp_sender1 != NULL)
	{
		delete tcp_sender1;
		tcp_sender1 = NULL;
	}
	if (tuioServer1 != NULL)
	{
		delete tuioServer1;
		tuioServer1 = NULL;
	}
}

BOOL CVirtualTUIO::InputTUIOPoints(const TContactInfo* pPenInfos, int nPenCount, const TContactInfo* pPenInfos1, int nPenCount1, DWORD dwCameraId, DWORD dwCameraId1)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csForTUIO);

	if (NULL == tuioServer) return FALSE;

	frameTime = TuioTime::getSessionTime();
	tuioServer->initFrame(frameTime);

	for (int i = 0; i < nPenCount; i++)
	{
		TuioCursor* match = NULL;

		float X = ((float)pPenInfos[i].pt.x /*+ g_tSysCfgData.globalSettings.nXCoordOffset*5*/) / m_nCxScreen;
		float Y = ((float)pPenInfos[i].pt.y /*+ g_tSysCfgData.globalSettings.nYCoordOffset*5*/) / m_nCyScreen;

		//if (!ActiveCursorList.empty())
		//{
	 //      for (std::list<VecTuioCursor>::iterator iter = ActiveCursorList.begin(); iter != ActiveCursorList.end(); iter++)
	 //      {
		//	  if ((*iter).nId == pPenInfos[i].uId)
		//	  {
		//		  TuioCursor *cursor = (*iter).tuiocursor;
		//	      match = (*iter).tuiocursor;
		//		  ////�ҵ��ǰ���״̬
		//		  if (pPenInfos[i].ePenState == E_PEN_STATE_DOWN)
		//		  {	
		//			    tuioServer->updateTuioCursor(cursor, X, Y);
		//				(*iter).bInvalid = false;
		//		  }
		//		  else {				  
		//			   //�ҵ��ǵ���״̬
		//			  if (!(*iter).bInvalid)
		//			  {
		//			     tuioServer->removeTuioCursor(cursor);
  //                       (*iter).bInvalid = true;
		//			  }
		//		  }
		//	  }
	 //      }
		//}
		//if (match ==NULL)
		//{
			VecTuioCursor vecCursor;
			vecCursor.tuiocursor = tuioServer->addTuioCursor(X, Y, pPenInfos[i].dwCameraId, pPenInfos[i].gunId);
			vecCursor.nId = pPenInfos[i].uId;
			vecCursor.bInvalid = false;
			//ActiveCursorList.push_back(vecCursor);
			
			//wchar_t szTmp[1024] = { 0 };
			//wsprintf(szTmp, L" xxxxxx TUIO dwCameraId:%d\n", pPenInfos[i].dwCameraId);
			//OutputDebugString(szTmp);
		//}
	}

	//std::list<VecTuioCursor>::iterator iter;
	//for (iter = ActiveCursorList.begin(); iter != ActiveCursorList.end();)
	//{
	//	if ((*iter).bInvalid)
	//	{
	//		iter  = ActiveCursorList.erase(iter);
	//	}
	//	else
	//	{
	//		iter++;
	//	}
	//}
//	tuioServer->stopUntouchedMovingCursors();
	tuioServer->commitFrame(); //ʱ��
	tuioServer->removeTuioCursor();

	//<<debug	
	/*
	if (nPenCount == 0 && ActiveCursorList.size() != 0)
	{
		int nDebug = 0;
		nDebug = 1;
	}

	static std::vector<TContactInfo> vevLastContactInfo;
	static int lastPencCount = nPenCount;

	if (nPenCount)
	{
		vevLastContactInfo.assign(pPenInfos, pPenInfos + nPenCount);
	}
	*/
	//debug>>

	/******************************************************************/
	if (NULL == tuioServer1) return FALSE;
	frameTime1 = TuioTime::getSessionTime();
	tuioServer1->initFrame(frameTime1);
	for (int i = 0; i < nPenCount1; i++)
	{
		TuioCursor* match = NULL;
		float X = ((float)pPenInfos1[i].pt.x /*+ g_tSysCfgData.globalSettings.nXCoordOffset*5*/) / m_nCxScreen;
		float Y = ((float)pPenInfos1[i].pt.y /*+ g_tSysCfgData.globalSettings.nYCoordOffset*5*/) / m_nCyScreen;

		VecTuioCursor vecCursor;
		vecCursor.tuiocursor = tuioServer1->addTuioCursor(X, Y, pPenInfos1[i].dwCameraId, pPenInfos1[i].gunId);
		vecCursor.nId = pPenInfos1[i].uId;
		vecCursor.bInvalid = false;
	}

	tuioServer1->commitFrame(); //ʱ��
	tuioServer1->removeTuioCursor();
	/******************************************************************/
	return TRUE;
}

void CVirtualTUIO::Reset()
{
	//ActiveCursorList.clear();
}

void CVirtualTUIO::ReopenTUIOServer()
{
	CloseTUIOServer();
	OpenTUIOServer(false);
}

bool CVirtualTUIO::LoadTUIOConfig()
{
	const TCHAR* lpszXMLFileName = _T("TUIOConfig.xml");
	TiXmlDocument oXMLDoc;
	if (!oXMLDoc.LoadFile(CT2CA(lpszXMLFileName), TIXML_ENCODING_UTF8))
	{
		return false;
	}
	TiXmlElement* pRootElement = oXMLDoc.RootElement();
	if (pRootElement == NULL) return false;

	TiXmlNode* pChild = NULL;
	do
	{
		pChild = pRootElement->IterateChildren(pChild);
		if (NULL == pChild) break;
		const char* lpszElementName = pChild->Value();
		if (_stricmp(lpszElementName, "Address") == 0)
		{
			const char* NodeName = pChild->Value();//�ڵ�����
			const char* lpszIP = ((TiXmlElement*)pChild)->Attribute("IP");
			const char* lpszPort = ((TiXmlElement*)pChild)->Attribute("Port");
			if (lpszIP == NULL || strlen(lpszIP) == 0 || lpszPort == NULL || strlen(lpszPort) == 0)
			{
				return false;
			}
			strcpy_s(szIP, _countof(szIP), lpszIP);
			m_nPort = atoi(lpszPort);
		}
		else if(_stricmp(lpszElementName, "Screen") == 0)
		{
			const char* NodeName = pChild->Value();//�ڵ�����
			const char* lpszWidth = ((TiXmlElement*)pChild)->Attribute("Width");
			const char* lpszHeight = ((TiXmlElement*)pChild)->Attribute("Height");
			if (lpszWidth == NULL || strlen(lpszWidth) == 0 || lpszHeight == NULL || strlen(lpszHeight) == 0)
			{
				return false;
			}

			m_nCxScreen = atoi(lpszWidth);
			m_nCyScreen = atoi(lpszHeight);
		}
	} while (pChild);

	return true;
}

bool CVirtualTUIO::SaveTUIOConfig()
{
	const TCHAR* lpszConfigFilePath = _T("TUIOConfig.xml");
	TiXmlDocument oXMLDoc;
	TiXmlDeclaration Declaration("1.0", "UTF-8", "no");
	oXMLDoc.InsertEndChild(Declaration);

	TiXmlElement * pConfig = new TiXmlElement("Config");
	oXMLDoc.LinkEndChild(pConfig);

	TiXmlElement * pElement = new TiXmlElement("Address");
	pElement->SetAttribute("IP", szIP);
	pElement->SetAttribute("Port", m_nPort);
	pConfig->LinkEndChild(pElement);

	pElement = new TiXmlElement("Screen");
	pElement->SetAttribute("Width", m_nCxScreen);
	pElement->SetAttribute("Height", m_nCyScreen);

	pConfig->LinkEndChild(pElement);

	//��UTF-8�����ʽ����
	TiXmlPrinter  printer;
	oXMLDoc.Accept(&printer);
	char UTF8BOM[3] = { '\xEF','\xBB','\xBF' };
	std::ofstream theFile;
	theFile.open(CT2W(lpszConfigFilePath), ios_base::out | ios_base::trunc);

	if (theFile.is_open())
	{
		theFile.write(UTF8BOM, 3);

		//����GB2312����ҳ��
		UINT CP_GB2312 = 936;

		//MBCS ת Unicode
		CA2W wcharContent(printer.CStr(), CP_GB2312);

		//����UTF8����ĳ���
		int utf8_length =
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wcharContent,
				-1,
				NULL,
				0,
				NULL,
				NULL);

		char* utf_8_buf = (char*)malloc(utf8_length);

		//UnicodeתΪUTF8����
		WideCharToMultiByte(
			CP_UTF8,
			0,
			wcharContent,
			-1,
			utf_8_buf,
			utf8_length,
			NULL,
			NULL);

		theFile.write(utf_8_buf, strlen(utf_8_buf));

		theFile.close();
		free(utf_8_buf);
	}

	return false;
}
//@���ܣ�����TUIOʱ��IP��ַ�Ͷ˿ں�
////////////////////////////////
void  CVirtualTUIO::SetTUIOParams(DWORD IP,int nPort, int nScreenWindth, int nScreenHeight)
{
	char szTempIP[24];
	sprintf_s(szTempIP,_countof(szTempIP),"%d.%d.%d.%d", (IP & 0xFF000000)>>24 , (IP & 0x00FF0000) >> 16, (IP & 0x0000FF00) >> 8, (IP & 0x000000FF));

	if (strcmp(szTempIP,szIP) != 0 || m_nPort != nPort)
	{
	   strcpy_s(szIP,_countof(szIP), szTempIP);
	   m_nPort = nPort;

	   ReopenTUIOServer();
	}
	m_nCxScreen = nScreenWindth ;
	m_nCyScreen = nScreenHeight ;

    SaveTUIOConfig();
}

//@���ܣ��õ�TUIOʱ��IP��ַ
/////////////////////////
DWORD  CVirtualTUIO::GetIPadress()
{
	DWORD   dwIP;
	dwIP = inet_addr(szIP);
	return dwIP;
}
//@���ܣ��õ�TUIOʱ�Ķ˿ں�
/////////////////////////
int CVirtualTUIO::GetPort()
{
	return m_nPort;
}
int CVirtualTUIO::GetScreenWidth()
{
	return m_nCxScreen;
}
int CVirtualTUIO::GetScreenHeight()
{
	return m_nCyScreen;
}

void CVirtualTUIO::SetTUIOScreenDisplayChange(int nScreenX,int nScreenY)
{
	m_nCxScreen = nScreenX ;
	m_nCyScreen = nScreenY ;
}
