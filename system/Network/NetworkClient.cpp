//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "reflector/stdafx.h"

#include "system/network/NetworkClient.h"
#include "system/network/baseProtocolPackets.h"
#include "system/network/dataPacket.h"
#include "system/network/packetObserver.h"

NetworkClient* NetworkClient::networkClient = NULL;

//////////////////////////////////////////////////////////////////////

NetworkClient::NetworkClient()
{
	port = 0;
	playerId = 0;
	shipId = 0;
	teamId = 0;
	queue = new NetworkId[MAX_NUMBER_OF_PLAYERS];
	connectionState = NOT_CONNECTED;
}


NetworkClient::~NetworkClient( void )
{
	connectionState = NOT_CONNECTED;
	Networking::Get()->StopWinsock();
}

NetworkClient* NetworkClient::Get()
{
	if (networkClient == NULL)
	{
		networkClient = new NetworkClient();
	}
	return networkClient;
}

bool NetworkClient::StartClient(const std::string& playerName, const std::string& server, int port)
{
	if (Networking::Get()->StartWinsock() && !IsConnected())
	{
		WSASetLastError(0);

		this->server = server;
		this->port = port;
		this->playerName = playerName;
		connectionState = CONNECTING;
		return true;
	}
	connectionState = NOT_CONNECTED;
	return false;
}


void NetworkClient::StopClient()
{
	if (IsConnected())
	{
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		WSASetLastError(0);
		connectionState = NOT_CONNECTED;
		Networking::Get()->StopWinsock();
	}
}

void NetworkClient::AcknowledgeReady(byte shipId, byte teamId)
{
	if (connectionState == WAITING_TO_JOIN)
	{
		this->shipId = shipId;
		this->teamId = teamId;
		connectionState = READY_TO_JOIN;
	}
}

bool NetworkClient::SendPacketToServer(BasePacket* packet)
{
	if (connectionState == ACTIVE)
	{
		byte buffer[NETWORK_MAX_BUFFER_SIZE];
		int size = packet->Write(buffer);
		return SendDataToServer(buffer, size);
	}
	return false;
}

void NetworkClient::EventLogic()
{
	byte buffer[NETWORK_MAX_BUFFER_SIZE];

	// handle connection state engine
	switch (connectionState)
	{
	case NOT_CONNECTED:
		{
			break;
		}
	case CONNECTING:
		{
			std::string errStr;
			if (Connect(errStr))
			{
				connectionState = CONNECTED;
			}
			else
			{
				throw new Exception(errStr);
			}
			break;
		}
	case CONNECTED:
		{
			std::string errStr;
			if (RequestToJoin(playerName, errStr))
			{
				connectionState = WAITING_TO_JOIN;
			}
			else
			{
				throw new Exception(errStr);
			}
			break;
		}
	case WAITING_TO_JOIN:
		{
			break;
		}
	case READY_TO_JOIN:
		{
			std::string errStr;
			if (ConfirmJoin(true, shipId, teamId, errStr))
			{
				if (SignalStart())
				{
					connectionState = ACTIVE;
					break;
				}
				else
				{
					throw new Exception("failed to send start signal to server");
				}
			}
			else
			{
				throw new Exception(errStr);
			}
			connectionState = WAITING_TO_JOIN;
			break;
		}
	case ACTIVE:
		{
			// we're active - read incomming requests
			int size = ReadDataFromNetwork(buffer, NETWORK_MAX_BUFFER_SIZE);
			while (size > 0)
			{
				if (BasePacket::IsValidPacket(buffer))
				{
					if (HandleWelcome(buffer))
					{
						size = size - BasePacket::GetPacketSize(buffer);
					}
					else
					{
						// receive a new packet
						BasePacket* packet = BasePacket::Factory(buffer);
						PacketObserver::NotifyObservers(packet);
						size = size - packet->PacketSize();
						safe_delete(packet);
					}
				}
				else
				{
					// ditch any unsavory packets
					size = 0;
				}
			}
			break;
		}
	}
}

bool NetworkClient::Connect(std::string& errStr)
{
	LPHOSTENT lpHostEntry;
	lpHostEntry = ::gethostbyname(server.c_str());
	if (lpHostEntry == NULL)
	{
		int error = WSAGetLastError();
		errStr = "can't get host by name (" + Networking::NetworkErrorToString(error) + ")";
		connectionState = NOT_CONNECTED;
		return false;
	}

	// create a socket
	clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		errStr = "error creating socket (" + Networking::NetworkErrorToString(error) + ")";
		connectionState = NOT_CONNECTED;
		return false;
	}

	// the destination we wish to connect to
	struct sockaddr_in remoteAddress;
	memset(&remoteAddress, 0, sizeof(remoteAddress));
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	remoteAddress.sin_port = htons((u_short)port);

	// connect the client socket to the remote address
	int nRc = ::connect(clientSocket, (struct sockaddr *)&remoteAddress, sizeof(SOCKADDR_IN));
	if (nRc == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		::closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;

		errStr = Networking::NetworkErrorToString(err);
		connectionState = NOT_CONNECTED;

		return false;
	}

	// set keep alive to true
	BOOL keepAlive = TRUE;
	::setsockopt(clientSocket, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&keepAlive, sizeof(BOOL));

	// set the socket to non-blocking
	unsigned long dontblock = 1;
	::ioctlsocket(clientSocket, FIONBIO, &dontblock);

	this->playerName = playerName;
	this->server = server;

	// we are now connected
	connectionState = CONNECTED;
	return true;
}

bool NetworkClient::RequestToJoin(const std::string& name, std::string& errStr)
{
	byte buf[NETWORK_MAX_BUFFER_SIZE];

	PacketClientCanIJoin canIJoin;
	canIJoin.name = name;
	canIJoin.version = PROTOCOL_VERSION;
	int size = canIJoin.Write(buf);

	if (SendDataToServer(buf, size))
	{
		// wait for response for up to 5 seconds
		long time = ::GetTickCount() + 5000;
		while (time > ::GetTickCount())
		{
			::Sleep(20);
			int size = ReadDataFromNetwork(buf, NETWORK_MAX_BUFFER_SIZE);
			if (size > 0)
			{
				if (BasePacket::IsValidPacket(buf))
				{
					if (BasePacket::GetPacketId(buf) == PID_SERVER_YOUCAN)
					{
						PacketServerYouCan* packetServerYouCan = PacketServerYouCan::Read(buf);
						if (packetServerYouCan->accept)
						{
							// we are allowed to join
							playerId = packetServerYouCan->playerId;
							levelName = packetServerYouCan->levelName;
							safe_delete(packetServerYouCan);
							return true;
						}
						else
						{
							errStr = packetServerYouCan->reason;
							safe_delete(packetServerYouCan);
							return false;
						}
					}
				}
			}
		}
		errStr = "server did not respond in a timely fashion";
		return false;
	}
	else
	{
		errStr = "send data to server failed";
	}
	return false;
}

bool NetworkClient::ConfirmJoin(bool accept, byte shipId, byte teamId, std::string& errStr)
{
	byte buf[NETWORK_MAX_BUFFER_SIZE];

	PacketClientIWillJoin willJoin(playerId, shipId, teamId, accept);
	int size = willJoin.Write(buf);
	if (SendDataToServer(buf, size))
	{
		// wait for response for up to 5 seconds
		long time = ::GetTickCount() + 5000;
		while (time > ::GetTickCount())
		{
			::Sleep(20);
			int size = ReadDataFromNetwork(buf, NETWORK_MAX_BUFFER_SIZE);
			if (size > 0)
			{
				if (HandleWelcome(buf))
				{
					return true;
				}
			}
		}
		errStr = "server did not respond in a timely fashion";
		return false;
	}
	else
	{
		errStr = "send data to server failed";
	}
	return false;
}

bool NetworkClient::HandleWelcome(byte* buf)
{
	if (BasePacket::IsValidPacket(buf))
	{
		if (BasePacket::GetPacketId(buf) == PID_SERVER_WELCOME)
		{
			PacketServerWelcome* serverWelcome = PacketServerWelcome::Read(buf);
			
			maxPoints = serverWelcome->maxPoints;
			gameType = serverWelcome->gameType;

			// mark other players
			for (int i=0; i < serverWelcome->cntr; i++)
			{
				int playerId = serverWelcome->playerId[i];
				queue[playerId].inUse = true;
				queue[playerId].name = serverWelcome->name[i];
				queue[playerId].shipId = serverWelcome->shipId[i];
				queue[playerId].teamId = serverWelcome->teamId[i];
			}

			// notify all listening souls of this new event
			PacketObserver::NotifyObservers(serverWelcome);

			safe_delete(serverWelcome);
			return true;
		}
	}
	return false;
}

bool NetworkClient::SignalStart()
{
	byte buf[NETWORK_MAX_BUFFER_SIZE];

	PacketClientStarted clientStarted(playerId);
	int size = clientStarted.Write(buf);
	if (SendDataToServer(buf, size))
	{
		return true;
	}
	return false;
}

int NetworkClient::ReadDataFromNetwork(byte* buffer, int size)
{
	if (IsConnected())
	{
		int nFromLen = sizeof(struct sockaddr);
		int nRet = recvfrom(clientSocket,			// Bound socket
							(char*)buffer,			// Receive buffer
							size,					// Size of buffer in bytes
							MSG_PEEK,				// Flags
							NULL,					// Buffer to receive client address 
							0);						// Length of client address buffer

		if (nRet == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			WSASetLastError(0);
			return 0;
		}

		if (nRet == 0)
		{
			return 0;
		}

		nFromLen = sizeof(struct sockaddr);
		recvfrom(clientSocket,					// Socket
				 (char*)buffer,					// Receive buffer
				 nRet,							// Length of receive buffer
				 0,								// Flags
				 NULL,							// Buffer to receive sender's address
				 0);							// Length of address buffer

		if (nRet == SOCKET_ERROR)
		{
			WSASetLastError(0);
			return 0;
		}
		return nRet;
	}
	return 0;
}

bool NetworkClient::SendDataToServer(const byte* buffer, int size)
{
	if (IsConnected())
	{
		int nRet = sendto(clientSocket,			// Socket
					  (char*)buffer,			// Data buffer
					  size,						// Length of data
					  0,						// Flags
					  NULL,						// socket address
					  0);						// Length of address

		if (nRet == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			connectionState = NOT_CONNECTED;
			WSASetLastError(0);
			return false;
		}
	}
	return true;
}

