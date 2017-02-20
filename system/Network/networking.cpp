//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <winsock.h>

#include "system/network/networking.h"

Networking* Networking::networkSingleton = NULL;

//////////////////////////////////////////////////////////////////////

Networking::Networking()
{
	discoveryIDStr = "pdv_disc_1_1";

	// winsock status
	winsockStarted = false;
}

Networking::~Networking()
{
	StopWinsock();
}

Networking* Networking::Get()
{
	if (networkSingleton == NULL)
	{
		networkSingleton = new Networking();
	}
	return networkSingleton;
}

bool Networking::StartWinsock()
{
	if (!winsockStarted)
	{
		winsockStarted = true;

		WORD wVersionRequested = MAKEWORD(1,1);
		WSADATA wsaData;

		//
		// Initialize WinSock and check version
		//
		int nRet = WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested)
		{	
			::WSACleanup();
			winsockStarted = false;
			throw new Exception("wrong version of winsock");
		}
	}
	return true;
}

// shutdown winsock
void Networking::StopWinsock()
{
	if (winsockStarted)
	{
		winsockStarted = false;
		::WSACleanup();
	}
}

bool Networking::GetLocalNetDetails(std::string& hostName,std::string& ipStr)
{
	std::string errStr;
	if (StartWinsock())
	{
		char buf[NETWORK_MAX_BUFFER_SIZE];
		int res = ::gethostname(buf, NETWORK_MAX_BUFFER_SIZE);
		if (res!=SOCKET_ERROR)
		{
			hostName = buf;
			LPHOSTENT lpHostEntry;
			lpHostEntry = ::gethostbyname(buf);
			if (lpHostEntry!=NULL)
			{
				SOCKADDR_IN remoteAddress;
				remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

				long lip = remoteAddress.sin_addr.S_un.S_addr;
				std::string ipStr2 = System::Int2Str(lip&255) + ".";
				ipStr2 = ipStr2 + System::Int2Str((lip>>8)&255) + ".";
				ipStr2 = ipStr2 + System::Int2Str((lip>>16)&255) + ".";
				ipStr2 = ipStr2 + System::Int2Str((lip>>24)&255);

				ipStr = ipStr2;

				return true;
			}
		}
	}
	return false;
}

//
// Find other network servers
//
std::vector<DiscoveryItem*> Networking::DiscoveryFindHosts()
{
	std::vector<DiscoveryItem*> items;
	if (StartWinsock())
	{
		// initialize the structure that will be passed to bind
		// as the local address.  We'll take any local address
		// and any local port.
		SOCKADDR_IN  myaddr;
		memset(&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = PF_INET;
		myaddr.sin_port = htons(DISCOVERY_PORT);
					
		// create a socket.
		SOCKET tcpInSock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if ( tcpInSock==INVALID_SOCKET )
		{
			throw new Exception("socket error " + System::Int2Str(WSAGetLastError()));
		}

		BOOL broadcast = TRUE;
		int nRc = ::setsockopt( tcpInSock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(BOOL) );
		if ( nRc==SOCKET_ERROR )
		{
			throw new Exception("socket error " + System::Int2Str(WSAGetLastError()));
		}

		// set the socket to blocking - just for connecting
		unsigned long dontblock = 1;
		::ioctlsocket( tcpInSock, FIONBIO, &dontblock );

		// bind the socket to the local address
		nRc = ::bind( tcpInSock, (struct sockaddr FAR *)&myaddr, sizeof(myaddr) );
		if(nRc == SOCKET_ERROR)
		{
			throw new Exception("socket error " + System::Int2Str(WSAGetLastError()));
		}

		SOCKADDR_IN remoteAddress;
		remoteAddress.sin_family = AF_INET;
		remoteAddress.sin_addr.S_un.S_addr = 0xffffffff; // broadcast
		remoteAddress.sin_port = htons(DISCOVERY_PORT);

		nRc = ::sendto(tcpInSock, discoveryIDStr.c_str(), discoveryIDStr.length()+1, 0,
						(const struct sockaddr*)&remoteAddress, sizeof(remoteAddress));
		if (nRc==SOCKET_ERROR)
		{
			::closesocket( tcpInSock );
			return items;
		}

		// wait for a reply for 10 seconds max
		DWORD time = ::GetTickCount() + FIND_HOST_TIMEOUT;
		bool found = false;
		char buf[NETWORK_MAX_BUFFER_SIZE];

		::listen( tcpInSock, SOMAXCONN );

		while (time > ::GetTickCount() && !found)
		{
			::Sleep(50);

			int count = ::recv(tcpInSock, buf, NETWORK_MAX_BUFFER_SIZE, 0);
			if (count>0 && buf[0]=='=')
			{
				std::string msg = &buf[1];

				// parse string
				std::string ipStr = System::GetItem(msg, ',', 0);
				std::string hostPort = System::GetItem(msg, ',', 1);
				std::string hostName = System::GetItem(msg, ',', 2);
				std::string numPlayers = System::GetItem(msg, ',', 3);
				std::string gameName = System::GetItem(msg, ',', 4);

				long ip = System::Str2Int(ipStr);
				ipStr = System::Int2Str(ip&255) + ".";
				ipStr = ipStr + System::Int2Str((ip>>8)&255) + ".";
				ipStr = ipStr + System::Int2Str((ip>>16)&255) + ".";
				ipStr = ipStr + System::Int2Str((ip>>24)&255);

				// create a new discovery item
				DiscoveryItem* discoveryItem = new DiscoveryItem();
				discoveryItem->HostIP(ipStr);
				discoveryItem->GameName(gameName);
				discoveryItem->HostPort(System::Str2Int(hostPort));
				discoveryItem->NumPlayers(System::Str2Int(numPlayers));

				items.push_back(discoveryItem);
			}
		}
		::closesocket( tcpInSock );
	}
	return items;
}

std::string Networking::NetworkErrorToString(int errorNumber)
{
	switch (errorNumber)
	{
	case WSANOTINITIALISED: 
		return "A successful WSAStartup call must occur before using this function.";
	case WSAENETDOWN:
		return "The network subsystem has failed.";
	case WSAHOST_NOT_FOUND:
		return "Authoritative answer host not found. ";
	case WSATRY_AGAIN:
		return "Nonauthoritative host not found, or server failure. ";
	case WSANO_RECOVERY:
		return "A nonrecoverable error occurred. ";
	case WSANO_DATA:
		return "Valid name, no data record of requested type. ";
	case WSAEINPROGRESS:
		return "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
	case WSAEFAULT:
		return "The name parameter is not a valid part of the user address space. ";
	case WSAEINTR:
		return "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
	case WSAEAFNOSUPPORT: 
		return "The specified address family is not supported.";
	case WSAEMFILE:
		return "No more socket descriptors are available.";
	case WSAENOBUFS: 
		return "No buffer space is available. The socket cannot be created.";
	case WSAEPROTONOSUPPORT: 
		return "The specified protocol is not supported.";
	case WSAEPROTOTYPE: 
		return "The specified protocol is the wrong type for this socket.";
	case WSAESOCKTNOSUPPORT: 
		return "The specified socket type is not supported in this address family.";
	case WSAECONNREFUSED:
		return "Server refused the connection (not there?  firewall?)";
	}
	return System::Int2Str(errorNumber);
}

