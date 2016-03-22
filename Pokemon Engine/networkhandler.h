#ifndef _NETWORKING_H
#define _NETWORKING_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class CMultiplayerHandler
{
public:
	CMultiplayerHandler::CMultiplayerHandler();

	void Transmit( std::string str );
	bool Recieve( std::string &out );

	bool Connect( std::string ip );
	bool IsClientConnected();
	void StartServer();

	bool m_bIsServer;
protected:
	std::string ip;
	
	bool m_bClientConnected;

	WSADATA wsaData;

	SOCKET ListenSocket;
    SOCKET ClientSocket;

	sockaddr_in ClientSockIn;
};
#endif