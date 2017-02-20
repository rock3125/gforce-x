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

#include "system/network/NetworkServer.h"
#include "system/network/baseProtocolPackets.h"
#include "system/network/protocolHandler.h"
#include "system/network/dataPacket.h"

//////////////////////////////////////////////////////////////////////

NetworkServer::NetworkServer(const std::string& levelName, int gameType, int maxPoints)
{
	this->levelName = levelName;
	this->gameType = gameType;
	this->maxPoints = maxPoints;

	serverStarted = false;
	port = 0;
	queue = new NetworkId[MAX_NUMBER_OF_PLAYERS];
	serverSocket = INVALID_SOCKET;
	maxNumberOfPlayers = 0;
}

NetworkServer::~NetworkServer()
{
	StopServers();
}


bool NetworkServer::StartServer(short port)
{
	if (serverStarted)
	{
		throw new Exception("server has already been started");
	}

	if (Networking::Get()->StartWinsock())
	{
		// create the comms socket
		serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket != SOCKET_ERROR)
		{
			// set the socket to non-blocking
			unsigned long dontblock = 1;
			::ioctlsocket(serverSocket, FIONBIO, &dontblock);

			// keep socket alive auto
			BOOL keepAlive = TRUE;
			::setsockopt(serverSocket, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&keepAlive, sizeof(BOOL));

			struct sockaddr_in myaddr;
			memset(&myaddr, 0, sizeof(struct sockaddr_in));
			myaddr.sin_family = AF_INET;
			myaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// allow anyone to connect
			myaddr.sin_port = htons((u_short)port);

			// bind the socket
			int nRc = ::bind(serverSocket, (struct sockaddr FAR *)&myaddr, sizeof(myaddr));
			if (nRc == SOCKET_ERROR)
			{
				long err = WSAGetLastError();
				::closesocket(serverSocket);
				throw new Exception("error binding to server socket");
			}
			nRc = ::listen(serverSocket, SOMAXCONN);
			if (nRc == SOCKET_ERROR)
			{
				long err = WSAGetLastError();
				::closesocket(serverSocket);
				throw new Exception("error listening on socket");
			}

			serverStarted = true;
			return true;
		}
	}
	return false;
}

void NetworkServer::StopServers()
{
	if (serverStarted)
	{
		// close all connected clients
		for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
		{
			if (queue[i].socket != INVALID_SOCKET)
			{
				::closesocket(queue[i].socket);
				queue[i].socket = INVALID_SOCKET;
			}
			queue[i].isActive = false;
			queue[i].inUse = false;
		}

		if (serverSocket != INVALID_SOCKET)
		{
			closesocket(serverSocket);
		}

		// we have officially stopped
		serverStarted = false;
	}
}

void NetworkServer::EventLogic()
{
	byte buf[NETWORK_MAX_BUFFER_SIZE];
	byte outbuf[NETWORK_MAX_BUFFER_SIZE];

	if (serverStarted)
	{
		// accept new connection?
		SOCKET sock = ::accept(serverSocket, NULL, NULL);
		if (sock != INVALID_SOCKET)
		{
			// make sure we haven't exceeded the number of player slots yet
			int playerCounter = 0;
			for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++ )
			{
				if (queue[i].inUse)
				{
					playerCounter++;
				}
			}

			if (playerCounter >= maxNumberOfPlayers)
			{
				::closesocket(sock);
			}
			else
			{
				// find first available slot
				bool found = false;
				for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++ )
				{
					if (!queue[i].inUse)
					{
						found = true;

						queue[i].inUse = true;
						queue[i].isActive = false;
						queue[i].socket = sock;
						printf("player %d connected\n", i);
						break;
					}
				}

				// can't connect - all slots full - go away!
				if (!found)
				{
					::closesocket(sock);
				}
			}
		}

		// read from all available sockets to see if there's a message
		// queueing for me and write any messages waiting to be send
		for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
		{
			if (queue[i].inUse)
			{
				// read
				int size = ::recv(queue[i].socket, (char*)buf, NETWORK_MAX_BUFFER_SIZE, 0);
				if (size==SOCKET_ERROR || size==0)
				{
					// has this person disappeared from the network?
					long err = ::WSAGetLastError();
					if (err != WSAEWOULDBLOCK || size==0)
					{
						::closesocket(queue[i].socket);
						queue[i].socket = INVALID_SOCKET;
						queue[i].inUse = false;
						queue[i].isActive = false;
						queue[i].name = "";

						printf("player %d disconnected\n", i);

						// send packet to all other clients to show this person has quit/is gone
						PacketServerOtherPlayerQuit packetServerOtherPlayerQuit((byte)i);
						int size = packetServerOtherPlayerQuit.Write(outbuf);
						Reflect(-1, outbuf, size);
					}
					::WSASetLastError(0);
				}
				else 
				{
					// process the packets
					int index = 0;
					do
					{
						if (BasePacket::IsValidPacket(&buf[index]))
						{
							BasePacket* packet = BasePacket::Factory(&buf[index]);
							int packetIndex = index;
							int packetSize = packet->PacketSize();
							index += packetSize;

							// is this is a special protocol packet?
							if (ProtocolHandler::IsProtocolPacket(packet))
							{
								ProtocolHandler::ProcessPacket(this, (byte)i, packet);
							}
							else
							{
								// keep score locally for newly connecting people and in-game status
								if (packet->id == DP_POSITION)
								{
									DataPosition* pospacket = dynamic_cast<DataPosition*>(packet);
									queue[i].score = pospacket->myScore;
								}

								// reflect the packet to all other people (not i!)
								Reflect(i, &buf[packetIndex], packetSize);
							}
							safe_delete(packet);
						}
						else
						{
							// skip unknown / invalid packets
							index = size;
							break;
						}
					}
					while (index < size);
				}
			}
		}
	}
}

void NetworkServer::Reflect(int excludePlayerId, byte* buffer, int size)
{
	// forward the packet to all other players
	for (int j = 0; j < MAX_NUMBER_OF_PLAYERS; j++)
	{
		if (queue[j].inUse && queue[j].isActive && j != excludePlayerId)
		{
			SendDataToPlayer((byte)j, buffer, size);
		}
	}
}

void NetworkServer::SetClientDetails(byte playerId, bool accept, byte shipId, byte teamId)
{
	if (accept)
	{
		queue[playerId].shipId = shipId;
		queue[playerId].teamId = teamId;
	}
	else
	{
		if (queue[playerId].socket != INVALID_SOCKET)
		{
			::closesocket(queue[playerId].socket);
		}
		queue[playerId].inUse = false;
		queue[playerId].isActive = false;
		queue[playerId].socket = INVALID_SOCKET;
	}
}

bool NetworkServer::CheckNewPlayer(const std::string& name, int version, std::string& errStr)
{
	if (version != PROTOCOL_VERSION)
	{
		errStr = "incorrect client version";
		return false;
	}

	if (name.size() == 0)
	{
		errStr = "name is empty";
		return false;
	}

	std::string lcaseName = System::ToLower(name);
	for (int i=0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		if (queue[i].inUse)
		{
			if (System::ToLower(queue[i].name) == lcaseName)
			{
				errStr = "that name is already taken";
				return false;
			}
		}
	}

	return true;
}

void NetworkServer::ActivatePlayer(byte playerId)
{
	queue[playerId].isActive = true;
}

void NetworkServer::RemovePlayer(byte playerId)
{
	queue[playerId].isActive = false;
	queue[playerId].inUse = false;
}

void NetworkServer::WriteDataToClient(SOCKET clientSocket, byte* buffer, int size)
{
	if (serverStarted)
	{
		int nLen = sizeof(SOCKADDR_IN);
		int result = sendto(clientSocket, (char*)buffer, size, 0, NULL, 0);
		if (result == SOCKET_ERROR)
		{
			WSAGetLastError();
			WSASetLastError(0);
		}
	}
}

void NetworkServer::SendDataToPlayer(byte playerId, byte* buffer, int size)
{
	if (serverStarted && queue[playerId].inUse)
	{
		WriteDataToClient(queue[playerId].socket, buffer, size);
	}
}

void NetworkServer::SetupWelcomePacket(PacketServerWelcome* welcomePacket)
{
	for (int i=0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		if (queue[i].inUse && queue[i].isActive)
		{
			welcomePacket->Add(i, queue[i].name, queue[i].teamId, queue[i].shipId, queue[i].score);
		}
	}
}

