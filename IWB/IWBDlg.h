// OpticalPenDlg.h : header file
//

#pragma once

//#include "FilterGraphBuilder.h"

#include "afxcmn.h"
#include "afxwin.h"
#include "StatusBarEx.h"
#include "InstallTip/TipProcServer.h"
#include "CameraFmtDialog.h"
#include <thread>

//#include <iostream>
//#include <string>
//#include <cstring>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <thread>
//#include <atomic>
//#include <mutex>
//#include <condition_variable>
//
//
//#pragma comment(lib, "ws2_32.lib")
//
//constexpr int BUFFER_SIZE = 1024;
//constexpr int PORT = 8888;
//
//class UDPServer {
//public:
//	UDPServer(int port = PORT) : port_(port), running_(false), serverSocket_(INVALID_SOCKET) {}
//
//	~UDPServer() {
//		stop();
//	}
//
//	// �����������߳�
//	bool start() {
//		if (running_) {
//			std::cerr << "Server is already running" << std::endl;
//			return false;
//		}
//
//		// ��ʼ�� Winsock
//		WSADATA wsaData;
//		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//			std::cerr << "WSAStartup failed" << std::endl;
//			return false;
//		}
//
//		// ���� Socket
//		serverSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//		if (serverSocket_ == INVALID_SOCKET) {
//			std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
//			WSACleanup();
//			return false;
//		}
//
//		// ���÷�������ַ
//		serverAddr_.sin_family = AF_INET;
//		serverAddr_.sin_addr.s_addr = INADDR_ANY;
//		serverAddr_.sin_port = htons(port_);
//
//		// �����׽���ѡ������ַ����
//		int opt = 1;
//		if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR,
//			reinterpret_cast<const char*>(&opt), sizeof(opt)) == SOCKET_ERROR) {
//			std::cerr << "setsockopt failed: " << WSAGetLastError() << std::endl;
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//			WSACleanup();
//			return false;
//		}
//
//		// �� Socket
//		if (bind(serverSocket_, reinterpret_cast<SOCKADDR*>(&serverAddr_), sizeof(serverAddr_)) == SOCKET_ERROR) {
//			std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//			WSACleanup();
//			return false;
//		}
//
//		std::cout << "UDP Server started on port " << port_ << std::endl;
//
//		// ���������߳�
//		{
//			std::lock_guard<std::mutex> lock(stopMutex_);
//			running_ = true;
//		}
//		receiverThread_ = std::thread(&UDPServer::receiveLoop, this);
//		return true;
//	}
//
//	// ֹͣ������
//	void stop() {
//		{
//			std::lock_guard<std::mutex> lock(stopMutex_);
//			if (!running_) {
//				return;
//			}
//			running_ = false;
//		}
//
//		// �ر� Socket (���ʹ recvfrom �������)
//		if (serverSocket_ != INVALID_SOCKET) {
//			// �Ƚ��׽�����Ϊ������ģʽ�����������ڹرղ�����
//			u_long nonBlocking = 1;
//			ioctlsocket(serverSocket_, FIONBIO, &nonBlocking);
//
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//		}
//
//		// �ȴ������߳̽���
//		if (receiverThread_.joinable()) {
//			receiverThread_.join();
//		}
//
//		// ���� Winsock
//		WSACleanup();
//	}
//
//private:
//	// �������ݵ��̺߳���
//	void receiveLoop() {
//		char buffer[BUFFER_SIZE];
//		sockaddr_in clientAddr{};
//		int clientAddrLen = sizeof(clientAddr);
//
//		while (true) {
//			// �������״̬
//			{
//				std::lock_guard<std::mutex> lock(stopMutex_);
//				if (!running_) {
//					break;
//				}
//			}
//
//			// ���տͻ�������
//			memset(buffer, 0, BUFFER_SIZE);
//			int bytesReceived = recvfrom(
//				serverSocket_,
//				buffer,
//				BUFFER_SIZE,
//				0,
//				reinterpret_cast<SOCKADDR*>(&clientAddr),
//				&clientAddrLen
//			);
//
//			if (bytesReceived == SOCKET_ERROR) {
//				int error = WSAGetLastError();
//
//				// �������״̬�������ڹرչ����д�ӡ����
//				std::lock_guard<std::mutex> lock(stopMutex_);
//				if (running_) {
//					std::cerr << "Failed to receive data: " << error << std::endl;
//
//					// �������������ֹͣ����
//					if (error != WSAEWOULDBLOCK && error != WSAECONNRESET) {
//						running_ = false;
//						break;
//					}
//				}
//				else {
//					// �׽����ѹرգ������˳�ѭ��
//					break;
//				}
//				continue;
//			}
//
//			// ��ʾ���յ�����Ϣ
//			char clientIP[INET_ADDRSTRLEN];
//			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
//			std::cout << "Received " << bytesReceived << " bytes from "
//				<< clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
//			std::cout << "Data: " << buffer << std::endl;
//
//			// ������Ϣ���ͻ���
//			int bytesSent = sendto(
//				serverSocket_,
//				szUSBBuffer,
//				bytesReceived,
//				0,
//				reinterpret_cast<const SOCKADDR*>(&clientAddr),
//				clientAddrLen
//			);
//
//			if (bytesSent == SOCKET_ERROR) {
//				std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
//			}
//		}
//	}
//
//private:
//	int port_;
//	std::atomic<bool> running_;
//	SOCKET serverSocket_;
//	sockaddr_in serverAddr_;
//	std::thread receiverThread_;
//	std::mutex stopMutex_;      // ����ͬ��ֹͣ����
//};


//#include <vector>
//#include <iostream>
//#include <string>
//#include <cstring>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <thread>
//#include <atomic>
//#include <mutex>
//#include <condition_variable>
//#include <openssl/rsa.h>
//#include <openssl/pem.h>
//#include <openssl/err.h>
//#include <openssl/bio.h>
//
//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "libcrypto.lib")
//
//constexpr int BUFFER_SIZE = 1024;
//constexpr int PORT = 1235;
//
////const int BUFFER_SIZE = 1024;
//const int SERVER_PORT = 8888;
//
//class UDPServer {
//public:
//	UDPServer(int port = PORT) : port_(port), running_(false), serverSocket_(INVALID_SOCKET),
//		rsaKeyPair_(nullptr), clientPublicKey_(nullptr) {}
//
//	~UDPServer() {
//		stop();
//		if (rsaKeyPair_) {
//			RSA_free(rsaKeyPair_);
//		}
//		if (clientPublicKey_) {
//			RSA_free(clientPublicKey_);
//		}
//	}
//
//	// �����������߳�
//	bool start() {
//		if (running_) {
//			std::cerr << "Server is already running" << std::endl;
//			return false;
//		}
//
//		// ��ʼ�� Winsock
//		WSADATA wsaData;
//		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//			std::cerr << "WSAStartup failed" << std::endl;
//			return false;
//		}
//
//		// ��ʼ�� OpenSSL
//		OpenSSL_add_all_algorithms();
//		ERR_load_crypto_strings();
//
//		// ���� RSA ��Կ��
//		if (!generateRSAKeyPair()) {
//			std::cerr << "Failed to generate RSA key pair" << std::endl;
//			WSACleanup();
//			return false;
//		}
//
//		// ���� Socket
//		serverSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//		if (serverSocket_ == INVALID_SOCKET) {
//			std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
//			WSACleanup();
//			return false;
//		}
//
//		// ���÷�������ַ
//		serverAddr_.sin_family = AF_INET;
//		serverAddr_.sin_addr.s_addr = INADDR_ANY;
//		serverAddr_.sin_port = htons(port_);
//
//		// �����׽���ѡ������ַ����
//		int opt = 1;
//		if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR,
//			reinterpret_cast<const char*>(&opt), sizeof(opt)) == SOCKET_ERROR) {
//			std::cerr << "setsockopt failed: " << WSAGetLastError() << std::endl;
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//			WSACleanup();
//			return false;
//		}
//
//		// �� Socket
//		if (bind(serverSocket_, reinterpret_cast<SOCKADDR*>(&serverAddr_), sizeof(serverAddr_)) == SOCKET_ERROR) {
//			std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//			WSACleanup();
//			return false;
//		}
//
//		std::cout << "UDP Server started on port " << port_ << std::endl;
//		std::cout << "RSA key pair generated successfully" << std::endl;
//
//		// ���������߳�
//		{
//			std::lock_guard<std::mutex> lock(stopMutex_);
//			running_ = true;
//		}
//		receiverThread_ = std::thread(&UDPServer::receiveLoop, this);
//		return true;
//	}
//
//	// ֹͣ������
//	void stop() {
//		{
//			std::lock_guard<std::mutex> lock(stopMutex_);
//			if (!running_) {
//				return;
//			}
//			running_ = false;
//		}
//
//		// �ر� Socket
//		if (serverSocket_ != INVALID_SOCKET) {
//			u_long nonBlocking = 1;
//			ioctlsocket(serverSocket_, FIONBIO, &nonBlocking);
//			closesocket(serverSocket_);
//			serverSocket_ = INVALID_SOCKET;
//		}
//
//		// �ȴ������߳̽���
//		if (receiverThread_.joinable()) {
//			receiverThread_.join();
//		}
//
//		// ���� OpenSSL
//		EVP_cleanup();
//		ERR_free_strings();
//
//		// ���� Winsock
//		WSACleanup();
//	}
//
//private:
//	// ���� RSA ��Կ��
//	bool generateRSAKeyPair() {
//		// ���� RSA ����������
//		BIGNUM* e = BN_new();
//		if (!e) {
//			std::cerr << "Failed to create BIGNUM" << std::endl;
//			return false;
//		}
//
//		// ���ù�Կָ��Ϊ 65537
//		if (BN_set_word(e, RSA_F4) != 1) {
//			std::cerr << "Failed to set RSA public exponent" << std::endl;
//			BN_free(e);
//			return false;
//		}
//
//		// ���� RSA ��Կ�� (2048 λ)
//		rsaKeyPair_ = RSA_new();
//		if (!RSA_generate_key_ex(rsaKeyPair_, 2048, e, nullptr)) {
//			std::cerr << "Failed to generate RSA key pair: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
//			BN_free(e);
//			return false;
//		}
//
//		BN_free(e);
//		return true;
//	}
//
//	// �� PEM ��ʽ�ַ������ع�Կ
//	bool loadPublicKeyFromPEM(const std::string& pemKey) {
//		if (clientPublicKey_) {
//			RSA_free(clientPublicKey_);
//			clientPublicKey_ = nullptr;
//		}
//
//		BIO* bio = BIO_new_mem_buf(pemKey.c_str(), -1);
//		if (!bio) {
//			std::cerr << "Failed to create memory BIO" << std::endl;
//			return false;
//		}
//
//		clientPublicKey_ = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
//		BIO_free(bio);
//
//		if (!clientPublicKey_) {
//			std::cerr << "Failed to load public key: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
//			return false;
//		}
//
//		std::cout << "Client public key loaded successfully" << std::endl;
//		return true;
//	}
//
//	// ʹ�ÿͻ��˹�Կ������Ϣ
//	bool encryptMessage(const std::string& plaintext, std::vector<unsigned char>& ciphertext) {
//		if (!clientPublicKey_) {
//			std::cerr << "Client public key not initialized" << std::endl;
//			return false;
//		}
//
//		int rsaSize = RSA_size(clientPublicKey_);
//		ciphertext.resize(rsaSize);
//
//		int result = RSA_public_encrypt(
//			static_cast<int>(plaintext.length()),
//			reinterpret_cast<const unsigned char*>(plaintext.c_str()),
//			ciphertext.data(),
//			clientPublicKey_,
//			RSA_PKCS1_OAEP_PADDING
//		);
//
//		if (result == -1) {
//			std::cerr << "Encryption failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
//			return false;
//		}
//
//		// ������СΪʵ�ʼ��ܺ�ĳ���
//		ciphertext.resize(result);
//		return true;
//	}
//
//	// �������ݵ��̺߳���
//	void receiveLoop() {
//		char buffer[BUFFER_SIZE];
//		sockaddr_in clientAddr{};
//		int clientAddrLen = sizeof(clientAddr);
//
//		while (true) {
//			// �������״̬
//			{
//				std::lock_guard<std::mutex> lock(stopMutex_);
//				if (!running_) {
//					break;
//				}
//			}
//
//			// ���տͻ�������
//			memset(buffer, 0, BUFFER_SIZE);
//			int bytesReceived = recvfrom(
//				serverSocket_,
//				buffer,
//				BUFFER_SIZE,
//				0,
//				reinterpret_cast<SOCKADDR*>(&clientAddr),
//				&clientAddrLen
//			);
//
//			if (bytesReceived == SOCKET_ERROR) {
//				int error = WSAGetLastError();
//
//				// �������״̬�������ڹرչ����д�ӡ����
//				std::lock_guard<std::mutex> lock(stopMutex_);
//				if (running_) {
//					std::cerr << "Failed to receive data: " << error << std::endl;
//
//					// �������������ֹͣ����
//					if (error != WSAEWOULDBLOCK && error != WSAECONNRESET) {
//						running_ = false;
//						break;
//					}
//				}
//				else {
//					// �׽����ѹرգ������˳�ѭ��
//					break;
//				}
//				continue;
//			}
//
//			// ��ʾ���յ�����Ϣ
//			char clientIP[INET_ADDRSTRLEN];
//			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
//			std::cout << "Received " << bytesReceived << " bytes from "
//				<< clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
//
//			/******************************************************/
//			// ����UDP�׽���
//			SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//			if (clientSocket == INVALID_SOCKET) {
//				std::cerr << "�����׽���ʧ��: " << WSAGetLastError() << std::endl;
//				WSACleanup();
//				return;
//			}
//
//			const char* SERVER_IP = "127.0.0.1";
//			// ���÷�������ַ
//			sockaddr_in serverAddr;
//			memset(&serverAddr, 0, sizeof(serverAddr));
//			serverAddr.sin_family = AF_INET;
//			serverAddr.sin_port = htons(SERVER_PORT);
//			inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
//
//			//char buffer[BUFFER_SIZE];
//			int serverAddrLen = sizeof(serverAddr);
//			/******************************************************/
//
//			// ����Ƿ��ǹ�Կ��Ϣ
//			if (strstr(buffer, "-----BEGIN PUBLIC KEY-----") != nullptr) {
//				std::string pemKey(buffer, bytesReceived);
//				if (loadPublicKeyFromPEM(pemKey)) {
//					// �ظ�ȷ��
//					std::string response = "Public key received";
//					int bytesSent = sendto(
//						serverSocket_,
//						response.c_str(),
//						static_cast<int>(response.length()),
//						0,
//						reinterpret_cast<const SOCKADDR*>(&clientAddr),
//						clientAddrLen
//					);
//
//					if (bytesSent == SOCKET_ERROR) {
//						std::cerr << "Failed to send confirmation: " << WSAGetLastError() << std::endl;
//					}
//
//
//					// ������Ϣ��������
//					int sentBytes = sendto(clientSocket, response.c_str(), static_cast<int>(response.length()), 0,
//						(sockaddr*)&serverAddr, serverAddrLen);
//					if (sentBytes == SOCKET_ERROR) {
//						std::cerr << "������Ϣʧ��: " << WSAGetLastError() << std::endl;
//					}
//				}
//
//				// �ر��׽��ֲ�����Winsock
//				closesocket(clientSocket);
//				continue;
//			}
//
//			// ��ͨ��Ϣ����
//			std::cout << "Plaintext: " << buffer << std::endl;
//
//			// ������Ӧ��Ϣ
//			//std::string response = "Encrypted response to: " + std::string(buffer);
//
//			// ������Ӧ
//			std::vector<unsigned char> encryptedResponse;
//			if (encryptMessage(szUSBBuffer, encryptedResponse)) {
//				std::cout << "Encrypted response size: " << encryptedResponse.size() << " bytes" << std::endl;
//
//				// ���ͼ�����Ӧ
//				int bytesSent = sendto(
//					serverSocket_,
//					reinterpret_cast<const char*>(encryptedResponse.data()),
//					static_cast<int>(encryptedResponse.size()),
//					0,
//					reinterpret_cast<const SOCKADDR*>(&clientAddr),
//					clientAddrLen
//				);
//
//				if (bytesSent == SOCKET_ERROR) {
//					std::cerr << "Failed to send encrypted data: " << WSAGetLastError() << std::endl;
//				}
//				else {
//					std::cout << "Sent encrypted response to client" << std::endl;
//				}
//
//				// ������Ϣ��������
//				int sentBytes = sendto(clientSocket, reinterpret_cast<const char*>(encryptedResponse.data()), static_cast<int>(encryptedResponse.size()), 0,
//					(sockaddr*)&serverAddr, serverAddrLen);
//				if (sentBytes == SOCKET_ERROR) {
//					std::cerr << "������Ϣʧ��: " << WSAGetLastError() << std::endl;
//				}
//			}
//			else {
//				std::cerr << "Failed to encrypt response" << std::endl;
//			}
//
//			// �ر��׽��ֲ�����Winsock
//			closesocket(clientSocket);
//		}
//	}
//
//private:
//	int port_;
//	std::atomic<bool> running_;
//	SOCKET serverSocket_;
//	sockaddr_in serverAddr_;
//	std::thread receiverThread_;
//	std::mutex stopMutex_;
//	RSA* rsaKeyPair_;      // ������RSA��Կ��
//	RSA* clientPublicKey_; // �ͻ��˹�Կ
//};

class COwnerDrawMenu:public COwnerDrawMenuBase
{
public:
	//@����:�����Զ���˵���
	virtual void OnDrawMenuItem(LPDRAWITEMSTRUCT lpDrawItem)
	{
		BOOL bRet = FALSE;
		DWORD dwError = 0;
		RECT rcText;
		RECT rcImage;

		HMENU hMenu = GetMenuHandle(this->m_hMenu, lpDrawItem->itemID);
		if(hMenu == NULL) return;

		int  nItemWidth = lpDrawItem->rcItem.right  - lpDrawItem->rcItem.left;
		int nItemHeight = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

		SIZE sizeText   = GetMenuCaptionSize(lpDrawItem->itemID);
		SIZE sizeBitmap = GetItemBitmapSize(lpDrawItem->itemID);

		int cxMenuCheck = GetSystemMetrics(SM_CXMENUCHECK);
		int cyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);

		rcText.left   = lpDrawItem->rcItem.left    + cxMenuCheck;
		rcText.top    = lpDrawItem->rcItem.top     + (nItemHeight - sizeText.cy)/2;
		rcText.right  = rcText.left + sizeText.cx;
		rcText.bottom = lpDrawItem->rcItem.bottom  - (nItemHeight - sizeText.cy)/2;


		rcImage.right = lpDrawItem->rcItem.right;
		rcImage.left  = rcImage.right - sizeBitmap.cx;
		rcImage.top   = lpDrawItem->rcItem.top     + (nItemHeight - sizeBitmap.cy)/2;
		rcImage.bottom= lpDrawItem->rcItem.bottom  - (nItemHeight - sizeBitmap.cy)/2;
		

		TCHAR menuText[256];
		memset(menuText, 0, sizeof(menuText));
		GetMenuString(m_hMenu, lpDrawItem->itemID, menuText, _countof(menuText), MF_BYCOMMAND);

		HBITMAP hItemBitmap = GetMenuItemBitmap(lpDrawItem->itemID);
		HBITMAP hBmpOld = NULL;

        if(lpDrawItem->itemState & ODS_DISABLED)
        {
            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_GRAYTEXT));
        }
        else
        {
            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_MENUTEXT));
        }

		switch(lpDrawItem->itemAction)
		{
			case ODA_DRAWENTIRE:// This bit is set when the entire control needs to be drawn.

				DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
				
				if(hItemBitmap)
				{
					hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
			

					bRet = BitBlt(
						lpDrawItem->hDC,
						rcImage.left,
						rcImage.top,
						sizeBitmap.cx,
						sizeBitmap.cy,
						m_hMemDC,
						0,
						0,
						SRCCOPY);

				

					if(!bRet)
					{
						dwError = GetLastError();
					}

					SelectObject(this->m_hMemDC, hBmpOld);
				}

				TRACE(_T("OnDrawMenuItem itemAction = ODA_DRAWENTIRE\n"));
			break;

			case ODA_FOCUS:// This bit is set when the control gains or loses input focus. The itemState member should be checked to determine whether the control has focus.
				TRACE(_T("OnDrawMenuItem itemAction = ODA_FOCUS\n"));
			break;


			case ODA_SELECT://This bit is set when only the selection status has changed. The itemState member should be checked to determine the new selection state.
				
				{

					if(lpDrawItem->itemState & ODS_SELECTED)
					{
						HBRUSH  hSelectBrush = ::CreateSolidBrush(RGB(49, 106, 197));
						//�������
						FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hSelectBrush);
						INT oldBkMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);
						COLORREF clrOld = SetTextColor(lpDrawItem->hDC, RGB(255,255,255));

						//��������
						DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
						SetTextColor(lpDrawItem->hDC, clrOld);
						 SetBkMode(lpDrawItem->hDC, oldBkMode);
						::DeleteObject(hSelectBrush);

						//����ͼƬ
						if(hItemBitmap)
						{
							hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
					

							bRet = TransparentBlt(
								lpDrawItem->hDC,
								rcImage.left,
								rcImage.top,
								sizeBitmap.cx,
								sizeBitmap.cy,
								m_hMemDC,
								0,
								0,
								sizeBitmap.cx,
								sizeBitmap.cy,
								GetSysColor(COLOR_MENU));

						

							if(!bRet)
							{
								dwError = GetLastError();
							}

							SelectObject(this->m_hMemDC, hBmpOld);
						}//


					}
					else
					{
						
						HBRUSH  hBackgroundBrush = ::CreateSolidBrush(GetSysColor(COLOR_MENU));
						//�������
						FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBackgroundBrush);



						//��������
						DrawText(lpDrawItem->hDC, menuText, _tcslen(menuText), &rcText, DT_CENTER | DT_VCENTER);
						::DeleteObject(hBackgroundBrush);
						

						//����ͼƬ
						if(hItemBitmap)
						{
							hBmpOld = (HBITMAP)SelectObject(this->m_hMemDC, hItemBitmap);
					
							bRet = BitBlt(
								lpDrawItem->hDC,
								rcImage.left,
								rcImage.top,
								sizeBitmap.cx,
								sizeBitmap.cy,
								m_hMemDC,
								0,
								0,
								SRCCOPY);


							if(!bRet)
							{
								dwError = GetLastError();
							}

							SelectObject(this->m_hMemDC, hBmpOld);
						}//

					}
			
					
				}
			
				TRACE(_T("OnDrawMenuItem itemAction = ODA_SELECT\n"));
			break;


		}//switch

		//lpDrawItem
		switch(lpDrawItem->itemState)
		{
			case ODS_CHECKED://The menu item is to be checked. This bit is used only in a menu.
				TRACE(_T("OnDrawMenuItem itemState=ODS_CHECKED\n"));		
				break;

			case ODS_COMBOBOXEDIT://The drawing takes place in the selection field (edit control) of an owner-drawn combo box.
				TRACE(_T("OnDrawMenuItem itemState=ODS_COMBOBOXEDIT\n"));
				break;

			case ODS_DEFAULT://The item is the default item.
				TRACE(_T("OnDrawMenuItem itemState=ODS_DEFAULT\n"));
				break;

			case ODS_DISABLED://The item is to be drawn as disabled.
				TRACE(_T("OnDrawMenuItem itemState=ODS_DISABLED\n"));
				break;

			case ODS_FOCUS://The item has the keyboard focus.
				TRACE(_T("OnDrawMenuItem itemState=ODS_FOCUS\n"));
				break;

			case ODS_GRAYED://The item is to be grayed. This bit is used only in a menu.
				TRACE(_T("OnDrawMenuItem itemState=ODS_GRAYED\n"));
				break;

			case ODS_HOTLIGHT://Windows 98/Me, Windows 2000/XP: The item is being hot-tracked, that is, the item will be highlighted when the mouse is on the item.
				TRACE(_T("OnDrawMenuItem itemState=ODS_HOTLIGHT\n"));
				break;

			case ODS_INACTIVE://Windows 98/Me, Windows 2000/XP: The item is inactive and the window associated with the menu is inactive.
				TRACE(_T("OnDrawMenuItem itemState=ODS_INACTIVE\n"));
				break;

			case ODS_NOACCEL://Windows 2000/XP: The control is drawn without the keyboard accelerator cues.
				TRACE(_T("OnDrawMenuItem itemState=ODS_NOACCEL\n"));
				break;

			case ODS_NOFOCUSRECT://	Windows 2000/XP: The control is drawn without focus indicator cues.
				TRACE(_T("OnDrawMenuItem itemState=ODS_NOFOCUSRECT\n"));
				break;

			case ODS_SELECTED://The menu item's status is selected.
				TRACE(_T("OnDrawMenuItem itemState=ODS_SELECTED\n"));
				break;

		}//switch
	}

	//@����:����˵���ĳߴ�
	virtual void OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItem)
	{
		BOOL bRet = FALSE;
		DWORD dwErr = 0;

		HMENU hMenu = GetMenuHandle(this->m_hMenu, lpMeasureItem->itemID);
		if(hMenu == NULL) return;

		SIZE sizeText   = GetMenuCaptionSize(lpMeasureItem->itemID);
		SIZE sizeBitmap = GetItemBitmapSize(lpMeasureItem->itemID);

		int cxMenuCheck = GetSystemMetrics(SM_CXMENUCHECK);
		int cyMenuCheck = GetSystemMetrics(SM_CYMENUCHECK);

		lpMeasureItem->itemWidth = cxMenuCheck + sizeText.cx + sizeBitmap.cx;
		lpMeasureItem->itemHeight = cyMenuCheck;

		if(long(lpMeasureItem->itemHeight)  < sizeText.cy)
		{
			lpMeasureItem->itemHeight = sizeText.cy;
		}

		if(long(lpMeasureItem->itemHeight)  < sizeBitmap.cy)
		{
			lpMeasureItem->itemHeight = sizeBitmap.cy;
		}
	}

protected:


};

extern int sendParams[2];
//extern volatile bool bStart_Offset;

// CIWBDlg dialog
class CIWBDlg : public CDialog
{
// Construction
public:
	CIWBDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OPTICALPEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	void StartShootCalibrate();
	void LeaveShootCalibrate();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


protected:
	 void   afx_msg OnSize(UINT nType, int cx, int cy);
	HRESULT afx_msg OnTrayNotifyMsg(WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnManualCalibrationDone (WPARAM wParam,LPARAM lParam);
	//HRESULT afx_msg OnAbortManualCalibrating (WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnDisplayChange  (WPARAM wParam,LPARAM lParam);
	HRESULT afx_msg OnSetDetectThreshold(WPARAM wParam, LPARAM lParam);

	HRESULT  afx_msg OnBreakSpotSetting(WPARAM wParam, LPARAM lParam);       //zhaown
    HRESULT  afx_msg OnFinshSpotSetting(WPARAM wParam, LPARAM lParam);
//	HRESULT  afx_msg OnSetDetectSpot(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnClose(WPARAM wParam,LPARAM lParam);
	//afx_msg void OnBnClickedButtonExit();
	afx_msg void OnMenuExit();
	afx_msg void OnCtxmenuShowMainWindow();
	afx_msg void OnCtxMenuManualCalibrate();

	afx_msg void OnMenuRun();
	afx_msg void OnMenuStop();
	//afx_msg void OnMenuManualCalibrate();
//	afx_msg void OnUpdateMenuStop(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateMenuRun(CCmdUI *pCmdUI);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnMenuAbout();
    afx_msg void OnMenuOnlineRegister();
	afx_msg void OnMenuStatus();
	afx_msg void OnMenuDisableOpticalPenControl();	
	//afx_msg void OnOpenCloseTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg void OnProperyPage();
	afx_msg void OnLangSelect();
	//afx_msg void OnMenuAutoCalibrate(UINT uID);
	afx_msg void OnStartMenuAutoCalibrate();
	afx_msg void OnCtxmenuAutorunAtSystemStartup();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuParameterSettings();

	//ͼ�񴫸�����ݲ˵�������
	afx_msg void OnSensorCtxMenu(UINT uID);
    afx_msg void OnGestureSettingMenu(UINT uID);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//afx_msg void OnMaskAreaSpecify();
	//afx_msg void OnDisableManualSreenArea();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAddMaskArea(UINT uID);
	afx_msg void OnEraseMaskArea(UINT uID);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnMaskAreaEditActionRedo();
	afx_msg void OnMaskAreaEditActionUndo();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);

    afx_msg void OnVideoTuningMode();
    afx_msg void OnNormalUsageMode();
//	afx_msg void OnAutoMaskClutterTest();
	afx_msg void OnControlOn();
	afx_msg void OnControlOff();
	afx_msg void OnLaserTunningModel();
	afx_msg void OnMenuManualCalibrate();
    afx_msg void OnMenuShootCalibrate();
	afx_msg void OnMenuManualCalibrate25();
	afx_msg void OnMenuManualCalibrate36();

	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

    afx_msg void OnOperationLightspotsampling();
    afx_msg void OnInstallationanddebuggingMouse();
    afx_msg void OnInstallationanddebuggingTouchpad();

    afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnMenuAutoAddScreenMask();
	afx_msg void OnNcDestroy();

	afx_msg void OnSpotCollectionSetting();
	afx_msg void OnSimulationManualCalibration();
	afx_msg void OnSpotCalibrationSetting();
 	afx_msg void OnEndManualMaskAreaEdit();
	afx_msg void OnClearMaskArea();
	afx_msg void OnCtxmenuAutoCalibrate();
    afx_msg void OnSwapSensorImage();
    afx_msg void OnSwapImageWithSensor(UINT uID);
    afx_msg void OnSwitchToFusionScreenMode(UINT uID);

	afx_msg void OnChangeTouchScreenAspectRatio(UINT uID);

	afx_msg void OnInstallationanddebuggingUpdatefirmware();
	afx_msg void OnEndSession(BOOL bEnding);

	afx_msg void OnInstallationanddebuggingEnableinterpolate();
	afx_msg void OnMenuAdvancessetting();

	afx_msg void OnMenuCSGameParamter();

	afx_msg void OnMenuStartDrawOnlineScreenArea();
	afx_msg void OnMenuClearDrawOnlineScreenArea();

	afx_msg void OnMenuEnableDrawOnlineScreenArea();

	//�������ֱ༭����
	afx_msg void OnMenuTouchScreenLayoutDesigner();

	void LeaveShootCalibrateMode();

    void OnChangeSensorAreaNo(UINT uID);
//=====����======
	void AdjustStatusBar(int cx, int cy);
	BOOL StartRunning();
	BOOL StopRunning();
	void DoAutoCalibrate();

    //@����:���������ļ�
    //@����:lpszConfigFilePath, �����ļ�������··��
    BOOL LoadConfig();

    //@����:���������ļ�
    //@����:lpszConfigFilePath, �����ļ������·��
    BOOL SaveConfig();
    void Exit();

//	afx_msg void OnBnClickedButtonStartStop();
	HRESULT OnFpsNotify         (WPARAM wParam,LPARAM lParam);
    HRESULT OnCameraStatusNotify(WPARAM wParam,LPARAM lParam);
//	HRESULT OnCalcAdjustMatrix(WPARAM wParam, LPARAM lParam);
	HRESULT OnDeviceChange(WPARAM wParam, LPARAM lParam);

	HRESULT OnAutoCalibrateDone(WPARAM wParam, LPARAM lParam);

	HRESULT OnAppCommMsg(WPARAM wParam, LPARAM lParam);

	//@����:��С����������
	void MinimizeToTray();

	//@����:ȡ����Ļ��������༭ģʽ
	void CancelScreenAreadEditMode();

	//@����:�����ֶ�ָ������Ļ����
	//void UpdateScreenMaskArea();

	//@����:�����Ի���
	void CreateOwnerCursor();

	virtual void OnCancel();
	virtual void OnOK();


	virtual BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnInitialStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam);
	LRESULT OnChangeGuidelineYPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowGuideline(WPARAM wParam, LPARAM lParam);

	LRESULT OnChangeSpotProportion(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeMultEraser(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeFixedbolbsettime(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeNormalUserBrightness(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeAutoMaskDetectThreshold(WPARAM wParam, LPARAM lParam);

	LRESULT OnSpotCollectionSetting_Par(WPARAM wParam, LPARAM lParam);
	LRESULT OnStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam);

    LRESULT OnGraphNotify(WPARAM wParam, LPARAM lParam);

    //<<added by toxuke@gmail.com, 2013/04/19
 //   LRESULT OnChangeEnableGestrueRecognition(WPARAM wParam, LPARAM lParam);
 //	LRESULT OnChangeEnableGestureTouch(WPARAM wParam, LPARAM lParam);
 //	LRESULT OnChangeEnablePenTouch(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecordvideo(WPARAM wParam, LPARAM lParam);

	LRESULT OnChangeAutoCalibrateAveBrightness(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeAutoCalibrateLightGray(WPARAM wParam, LPARAM lParam);
	LRESULT OnRearProjection(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeVideoDisplayDelay(WPARAM wParam, LPARAM lParam);


	//afx_msg void OnMenuAutoCalibrationWithHumanIntervention();
	//afx_msg void OnAutoSearchScreenArea();

	LRESULT OnSearchScreenAreaDone(WPARAM wParam, LPARAM lParam);
	//LRESULT OnManualCorrectDone(WPARAM wParam, LPARAM lParam);
	//LRESULT OnDisPlayWindow(WPARAM wParam, LPARAM lParam);


	//void Undo();
	//void Redo();

   //@����;��Ӧ����������������Ϣ��
    LRESULT OnTaskbarRestartMsg(WPARAM, LPARAM lParam);

    //<<added by toxuke@gmail.com, 2014/12/18
    void OnAdvancedSettings(CIWBSensor* pSensor);
    //>>

    //<<added by jiqw, 2015/04/08
    void OnStartIntallerWithTips(CIWBSensor* pSensor);

    /*
    @���ܣ�������Ƶ����ʾ���ڣ���Ҫ�ǵ���װ����ģʽʱ����Ƶ�������һ��ָ��������
    @������wParam��ָ�����ڵľ��
    */
    LRESULT OnChangeDisplayWindow(WPARAM wParam, LPARAM lParam);
    //>>

    /*
    @���ܣ��ڼ�����ģʽ�£�����Ƿ񵲰��ģ��
    */
    HRESULT OnProcessDetectBackSplashData(WPARAM wParam,LPARAM lParam);

    /*
    @���ܣ��ڰ�װ�򵼹����У�����/ֹͣ�����Ƿ���ʧ
    */
    HRESULT OnStartDetectBackSplashVanished(WPARAM wParam,LPARAM lParam);
    HRESULT OnStopDetectBackSplashVanished(WPARAM wParam,LPARAM lParam);

    /*
    @���ܣ����û�ʹ��ĳ�ؼ�
    */
    HRESULT OnEnableOrDisableTheWindow(WPARAM wParam, LPARAM lParam);


    void OnDisplayChangeHelper(const SIZE& sizeNewScreen);


	//@����:Ӧ�ô�����������
	HRESULT OnApplySensorConfig(WPARAM wParam, LPARAM lParam);


    //@����:��Ļ������ư�ť�¼���Ӧ����
	//@����:wParam, ���ⰴťID, 
    HRESULT OnScreenLayoutDesignBtnEvent(WPARAM wParam, LPARAM lParam);


    //@����:������4��궨����Ϣ��Ӧ����
    HRESULT OnEnd4BasePointCalibrate(WPARAM wParam, LPARAM lParam);

	//2018/01/26
	//���¼�������״̬
	void UpdateInfoAboutDongle();
	//
	BOOL LoadResolutionConfig();



	HRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);
//====����===========


     BOOL m_bAllWndCreated  ;
	 CTrayImpl m_oTray      ;
	 HICON m_hAppIcon       ;
	//CString m_strProfilePath;//�����ļ�·��
	//CString m_strProfilePathSpot ;//����ģ��ɼ���ߵ���Ƶ���ļ���·��zhaown
	//BYTE    m_YThreshold    ;//�������������
    


    
    //BOOL LoadCfgData();
	//BOOL SaveCfgData();

    //@����:����У�������������ļ���ȥ
    //BOOL SaveCalibParams(const TCalibParams& calibParams, const TCHAR* szProfileName);

    //@����:�������ļ��ж�ȡУ������
    //BOOL LoadCalibParams(TCalibParams& calibParams, const TCHAR* szProfileName);



	//CSliderCtrl m_sldYDetectThreshold;
	//afx_msg void OnNMReleasedcaptureSliderYDetectThreshold(NMHDR *pNMHDR, LRESULT *pResult);
	//BOOL m_bEnableOpticalPenControl;//ʹ�ܹ�ʿ���
	
	
	//����:��Ƶ�����豸�б�
	//void UpdateVideoCaptureDevicesList();

	////@����:�������ƻ�ȡ��Ƶ�����豸����ʵ��
	//const TCaptureDeviceInstance* GetCaptureDevByName(LPCTSTR lpszCapDevName)const;


	////@����:�����豸ID��ȡ��Ƶ�����豸����ʵ��
	//const TCaptureDeviceInstance* GetCaptureDevById(LPCTSTR lpszCapDevID)const;

	////@����:�����豸��ID��Frendily���Ʋ����豸ʵ��
	//const TCaptureDeviceInstance* GetCaptureDevByIdAndFrendilyName(LPCTSTR lpszCapDevID, LPCTSTR lpszCapDevName)const;


	//@����:������Ƶ��ʽ���ƻ�ȡָ���豸ʵ���ĸ�ʽ
	//const VideoMediaType* GetVideoFormat(const TCaptureDeviceInstance& devInst, LPCTSTR lpszVideoFormatName);


	////����:��ȡ��һ���ҵ��ķ���ȱʡVID��PID���豸ʵ��
	//const TCaptureDeviceInstance* GetDefaultUSBDev()const;


	////@����:�����豸ʵ��
	//const TCaptureDeviceInstance* FindDevIntance()const;


	////@����:�ж�ָ��USB�Ƿ����
	//BOOL IsUSBCameraInstanceExisted(const TCaptureDeviceInstance& devInst);

	
	//CaptuerDeviceInstanceSet m_videoCaptureSet             ;
	//CString                  m_strCaptureDeviceFriendlyName;
	//CString                  m_strCaptureDeviceID          ;
	//CString                  m_strFavoriteDevicePath       ;//����ѡ�õ��豸·��
	//CString                  m_strFavoriteFormatName       ;//����ѡ�õ���Ƶ��ʽ
	CUsbCameraDeviceList     m_oUSBCameraDeviceList        ;//��Ƶ�豸�б�
	CMenu                    m_oMenu                       ;
    CMenu                    m_oSubMenuMergMode;
	CMenu                    m_mnuManualScreenAreaSettings ;//2011/11/03,�ֶ���Ļ����ָ����ݲ˵���
	CIWBSensor*              m_pSelectedSensor             ;//ѡ�е�ͼ�񴫸�������ָ��

	
	//CStatusDlg               m_ctlStatusBar                   ;//״̬�Ի���
	CUSBDevDetector*         m_pUSBDevDetector             ;//USB���弴�ü��
	CUSBDevDetector*         m_pUSBDevDetector_HID         ;
	UINT_PTR                 m_uOpenCloseTestTimer         ;//USB����ͷ�򿪹رղ��Զ�ʱ��
	//CAutoCalibrator          m_oAutoCalibrator             ;
	//CScreenAreaDetector      m_oScreenAreaDetector         ;//��Ļ�����Զ����
	BOOL                     m_bVisible                    ;//�Ի���ɼ���־

	HANDLE					 hServerThread;

	CPoint                   m_ptScreenAreaStart           ;//	
	CPoint                   m_ptScreenAreaEnd             ;//
	BOOL                     m_bScreenMaskAreaSelectDragging  ;//��Ļ���������ק��
	

	//��������༭��ر����ͺ���
	enum E_BMP_TYPE
	{
		e_BMP_BRUSH_1X,
		e_BMP_BRUSH_1D5X,
		e_BMP_BRUSH_2X,
		e_BMP_BRUSH_4X,
		e_BMP_ERASE_1X,
		e_BMP_ERASE_1D5X,
		e_BMP_ERASE_2X,
		e_BMP_ERASE_3X,
		e_BMP_NUMBER = 8
	};

	enum E_CURSOR_TYPE
	{
		e_CURSOR_BRUSH_1X,
		e_CURSOR_BRUSH_1D5X,
		e_CURSOR_BRUSH_2X,
		e_CURSOR_BRUSH_3X,
		e_CURSOR_ERASE_1X,
		e_CURSOR_ERASE_1D5X,
		e_CURSOR_ERASE_2X,
		e_CURSOR_ERASE_3X,
		e_CURSOR_NUMBER = 8
	};


    //��������ʱʹ�õ�����ͷ��Ƶ��������
  //  enum E_VideoParamsMode
  //  {
  //      E_VIDEO_PARAMS_MODE_NORMAL_USAGE    ,//����ʹ��ʱ�Ĳ���
  //      E_VIDEO_PARAMS_MODE_INTALLATION     ,//��װ����ʱʹ�õĲ���
		//E_VIDEO_PARAMS_MODE_INTALLATION1    ,//��װ���Դ��ʱʹ�õĲ���

  //      E_VIDEO_PARAMS_MODE_AUTO_CALIBRATION,//�Զ�У��ʱʹ�õĲ���
  //      E_VIDEO_PARAMS_MODE_AUTO_MASKING    ,//�Զ�����ʱʱʹ�õĲ���
  //      E_VIDEO_PARAMS_MODE_NUMBER          ,//����������Ŀ

  //  }m_eVideoParamsMode;


	CBitmap        m_aryMenuBmp  [e_BMP_NUMBER] ;//��ˢλͼ���顣
	CColorCursor   m_aryCursor   [e_BMP_NUMBER] ;//�������

	HCURSOR        m_hScreenMaskAreaSelectCursor;//�ֶ���Ļ����ָ�����
	HCURSOR        m_hArrowCursor               ;//
    HCURSOR        m_hHandCursor                ;//���ι��

	COwnerDrawMenu m_oOwnerDrawMenu             ;//��ˢ�Ի��˵�
	COwnerDrawMenu m_oOwnerDrawEaraseMenu       ;//��Ƥ���Ի��˵�

	enum EScreenMaskAreaEditMode
	{
		e_SCREEN_MASK_AREA_EDIT_MODE_IDLE,              //����״̬
		//e_SCREEN_MASK_AREA_EDIT_MODE_SELECT_SCREEN_AREA,//ѡȡ��Ļ����״̬
		e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK          ,//������Ļ�ڱ�����
		e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK        ,//ɾ����Ļ�ڱ�����
	}m_eScreenMaskAreaEditMode;

	enum EManualCalibrateMode
	{
		e_MANUALCALIBRATE_MODE_5,        //25����ֶ�У��
		e_MANUALCALIBRATE_MODE_6,        //36����ֶ�У��

	}m_eManualCalibrateMode;

	HCURSOR                 m_hCurrentEditToolCursor;
	E_AutoCalibratePattern  m_AutoCalibrateColsType;

	SIZE m_EraseSize;
	SIZE m_FillSize;

//	CPlayWnd m_PlayWnd;


	//��������༭����������������ر����ͺ���
	enum EScreenMaskAreaEditActionType
	{
		e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_FILL,//��������������
		e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_ERASE//��������ɾ������

	};
	struct TMaskAreaEditActions
	{
		EScreenMaskAreaEditActionType eScreenMaskAreaEditActionType;
		std::vector<RECT> vecArea;
	};

	std::stack<TMaskAreaEditActions> m_stackRedo;
	std::stack<TMaskAreaEditActions> m_stackUndo;

	//�����ģʽ��ر����ͺ���
	ATOM                     m_atomLaserPwrOffHotkey       ;//��ʵ�Դ�ر�ָʾ�ȼ�
	ATOM                     m_atomLaserPwrOnHotkey        ;//��ʵ�Դ��ָʾ�ȼ� 
	ATOM                     m_atomTogglePenContolHotkey   ;//�л���ʿ����ȼ�
	ATOM                     m_atomTogglePenContolHotkey_2 ;//�л���ʿ����ȼ�2

	TMaskAreaEditActions m_tMaskEditAction;

	//afx_msg LRESULT OnClutterDetectionDone(WPARAM wParam, LPARAM lParam);



	BYTE* m_pRawInputBuf;
	UINT  m_nRawInputBufSize;
	//CManualCorrectWnd m_oManualCorrentWnd;

	//BOOL m_bNeedHumanIntervention;//��Ҫ�˹���Ԥ��־
	//BOOL m_bNeedManualCalibrate  ;//��Ҫ�ֶ�У��
    //int  m_nAutoCalibrateTryTimes;//�Զ�У�����Դ���
    //static const int MAX_AUTOCALIBRATE_TRY_TIMES = 2;//�Զ�У�����Դ�����
	int m_nAutoMaskDetectThreshold;//�Զ�����ʱ������ֵ
	BOOL m_bIsRecordVideo;
	BOOL m_bIsOpenNoProjectorMode;
	BOOL m_bIsRearProjection     ;//�Ƿ��Ǳ�Ͷģʽ


    struct TOldState
    {
       BOOL bEnableOpticalPen;//ʹ�ܹ�ʵ�״̬��
    }m_tOldState;
	

	//=======2013/04/13=============
	//Added by toxuke@gmail.com, 2013/04/03
	//<<
	//@����:����У��ʱ������ͷ����
	//void SetAutoCalibrationCameraParams();

	//@����:��������ʹ��ʱ������ͷ����
	//void SetNormalCameraParams();

	//@����:�����������ʱ������ͷ����
	//void SetAutoMaskCameraParams();

    //@����:���ð�װ����ʱ������ͷ����
    //void SetAutoInstallationCameraParams();
    //@����:���ð�װ����ʱ������ͷ����
	//void SetAutoInstallationCameraParams1();
	//>>

protected:
    void InitMenu();
    void AppendScreenModeSubmenu();
    void InsertMenuItem2TheMenu(CMenu *pMenu, const int &nInsertIndex, const DWORD &dwIMenuItemID, LPTSTR lpszMenuItemName);
    void InsertParamSettingMenuItem(CMenu *pMenu, const int &nInsertIndex);
    void InitDeviceUseModeMenuItemWithMenu(CMenu *pMenu);

    //@����:��������ʵ������ʱ, ��⵽����ʵ�����ڣ�Ҫ�����ʵ����ʾ�����塣
    HRESULT OnBetweenInstanceMsg(WPARAM wParam, LPARAM lParam);

    //@����:�ж��Ƿ��ǺϷ���USBkey
    BOOL IsEligibleUSBKey(UINT uVID, UINT uPID);
public:
    //<<aded by toxuke@gmail.com, 2013/05/06
    //���������������¼���Ϣ
    static UINT m_uTaskbarRestartMsg;

    //�Զ�У�������б������ͷ�����Ļص�����
    static BOOL  OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);

    CIWBSensorManager m_oIWBSensorManager;//CMOS������������
    CStatusBarEx         m_ctlStatusBar;//״̬��
    CScreenRecognition m_oScreenRecognition                       ;//��Ļʶ����
    UINT_PTR           m_uScreenRecognitionCloseTimer             ;//��Ļʶ�����رն�ʱ��
    static const int   TIMER_EVENT_SCREEN_RECOGNITION_CLOSE   = 1 ;//��Ļʶ�����رն�ʱ�¼�
    static const int   SCREEN_RECOGNITION_CLOSE_TIMEOUT = 1000*10 ;//10�����ӳ�

	//<<20170925
	static const int TIMER_FOR_SENSOR_MANAGER = 2                  ;//��������������ʱ��
	static const int TIMER__FOR_SENSOR_MANAGER_TIMEOUT = 1000 * 1  ;//1���Ӷ�ʱ
	//20170925>>


    //////////////////////////////////////////////////////////////////////////
    //��װ��ʾ����ģ��
    CTipProcServer     m_tipProcServer;
    CIWBSensor         *m_pCurInstalledSensor;

    //<added by jiqw:20150416
    //<added reason:��װģʽ�£���ʾ��Ƶ�Ĵ��ڻ���ָ���Ĵ�����
    HWND               m_hDispWnd;
    //RECT               m_rcDispWnd;
    //>
    
    //ICONINFO m_shieldIconInfo;

    HBITMAP m_hUCShieldBitmap;
	static UINT m_uAppCommMsg;
	///add by vera_zhao 2019.10.28
	bool    m_bStartDrawOnlineScreenArea;
	bool    m_bPreGuideRectangleVisible ;
	int     m_nDrawOnlineAreaCount      ;
	int     m_nActiveDetectCameraId     ;

	//UDPServer server;

	std::thread  net_thread;

    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnOperationFourpointcalibration();

	std::vector<CAtlString>      m_aryCandidateResolution;
    afx_msg void OnMenuCircleScreenManualCalibrate();
};
