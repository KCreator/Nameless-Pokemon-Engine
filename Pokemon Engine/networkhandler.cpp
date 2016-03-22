#include "stdafx.h"
#include "networkhandler.h"

CMultiplayerHandler::CMultiplayerHandler()
{
	int iResult;
	m_bClientConnected = false;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
	}

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
}

void CMultiplayerHandler::Transmit( std::string str )
{
	if( !m_bIsServer )
	{
		char Buffer[128];
		strncpy( Buffer, str.c_str(), 128 );
		if( send( ListenSocket, Buffer, 128, 0) == SOCKET_ERROR )
		{
			return;
		}
	}
	if( m_bIsServer )
	{
		char Buffer[128];
		strncpy( Buffer, str.c_str(), 128 );
		if( send( ClientSocket, Buffer, 128, 0) == SOCKET_ERROR )
		{
			return;
		}
	}
}

bool CMultiplayerHandler::Recieve( std::string &out )
{
	if( m_bIsServer )
	{
		char Buffer[128];
		int BytesRec = recv(ClientSocket, Buffer, sizeof(Buffer), 0);

		if(BytesRec == 0)
		{
			return false;
		}

		out = Buffer;
		return true;
	}
	if( !m_bIsServer )
	{
		char Buffer[128];
		int BytesRec = recv(ListenSocket, Buffer, sizeof(Buffer), 0);

		if(BytesRec == 0)
		{
			return false;
		}

		out = Buffer;
		return true;
	}
	return false;
}

#define DEFAULT_PORT "27015"
void CMultiplayerHandler::StartServer()
{
	int iResult;

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
	{
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
	{
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
	{
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

	m_bIsServer = true;
}

bool CMultiplayerHandler::Connect( std::string ip )
{
	ClientSockIn.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	ClientSockIn.sin_family = AF_INET;						// Use TCP/IP protocol
	ClientSockIn.sin_port = htons(27015);

	if((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			return false;
	if(connect(ClientSocket, (sockaddr*)(&ClientSockIn), sizeof(ClientSockIn)) != 0)		// Connect to the server
			return false;

	return true;
}

bool CMultiplayerHandler::IsClientConnected()
{
	if( m_bClientConnected )
		return true;
	else
	{
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return false;
		}
		m_bClientConnected = true;
		return true;
	}
}