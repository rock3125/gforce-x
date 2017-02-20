#pragma once

//////////////////////////////////////////////////////////////////////

#include "system/network/networking.h"
#include "system/network/networkId.h"

class NetworkId;
class BasePacket;

enum ClientConnectionState
{
	NOT_CONNECTED = 0,
	CONNECTING,
	CONNECTED,
	WAITING_TO_JOIN,
	READY_TO_JOIN,
	ACTIVE
};

//////////////////////////////////////////////////////////////////////

class NetworkClient
{
	const NetworkClient& operator = (const NetworkClient&);
	NetworkClient(const NetworkClient&);
public:
	NetworkClient();
	virtual ~NetworkClient();

	// access network client singleton
	static NetworkClient* Get();

	// startup the client
	bool StartClient(const std::string& playerName, const std::string& server, int port);

	// shut down the client
	void StopClient();

	// perform send/receive events and state engine
	void EventLogic();

	// acknowledge we are ready
	void AcknowledgeReady(byte shipId, byte teamId);

	// are we connected to the server
	bool IsConnected()
	{
		return (connectionState > CONNECTING);
	}

	// are we connected to the server
	bool IsWaitingToJoin()
	{
		return (connectionState == WAITING_TO_JOIN);
	}

	// has the player connected & started?
	bool IsActive()
	{
		return (connectionState >= ACTIVE);
	}

	// get the name of another player
	const std::string& GetName(byte playerId) const
	{
		return queue[playerId].name;
	}

	// get the player id
	int GetPlayerId()
	{
		return playerId;
	}

	std::string LevelName()
	{
		return levelName;
	}

	// send data to the server - once active & connected
	bool SendPacketToServer(BasePacket* packet);

private:
	////////////////////////////////////////////////////////////////////
	// protocol implementation

	// 1. connect to the server
	bool Connect(std::string& errStr);

	// 2. request if I can join and wait for response
	bool RequestToJoin(const std::string& name, std::string& errStr);

	// 3. confirm joining with more specifics after your request has been granted
	bool ConfirmJoin(bool accept, byte shipId, byte teamId, std::string& errStr);

	// 4. confirm we've started and are ready to receive in-game data
	bool SignalStart();

	// handle a welcome packet - return true if there was a welcome packet
	// and handle the welcome packet
	bool HandleWelcome(byte* buf);

	// read data from the server (incoming) - return 0 if there is none
	// or there is an error
	int ReadDataFromNetwork(byte* buffer, int size);

	// send a message to the server
	bool SendDataToServer(const byte* buffer, int size);

private:
	short port;
	byte playerId;
	byte teamId;
	byte shipId;
	std::string levelName;
	std::string playerName;
	std::string server;
	int maxPoints;
	int gameType;

	SOCKET clientSocket;

	// connection status of client
	ClientConnectionState connectionState;

	// other players
	NetworkId* queue;

	// static singleton
	static NetworkClient* networkClient;
};
