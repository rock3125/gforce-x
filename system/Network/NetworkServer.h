#pragma once

//////////////////////////////////////////////////////////////////////

#include "system/network/networking.h"
#include "system/network/networkId.h"

class NetworkId;
class PacketServerWelcome;

//////////////////////////////////////////////////////////////////////

class NetworkServer
{
	const NetworkServer& operator = (const NetworkServer&);
	NetworkServer(const NetworkServer&);
public:
	NetworkServer(const std::string& levelName, int gameType, int maxPoints);
	virtual ~NetworkServer();

	// start the main server
	bool StartServer(short nPort);

	// stop serving
	void StopServers();

	// perform send/receive events
	void EventLogic();

	// send a message to all connected clients - excluding a particular player (optional - can be set to -1 to ignore)
	void Reflect(int excludePlayerId, byte* buffer, int size);

	// first - a client requests to be added and we do a check
	// second the client accepts if it is possible to join, and we add
	bool CheckNewPlayer(const std::string& name, int version, std::string& errStr);

	// set further client details when connected
	void SetClientDetails(byte playerId, bool accept, byte shipId, byte team);

	// add a new player to the server
	void ActivatePlayer(byte playerId);

	// remove an existing player from the server
	void RemovePlayer(byte playerId );

	// send a raw message to a connected player
	void SendDataToPlayer(byte playerId, byte* buffer, int size);

	// add all in-use players to the welcome packet
	void SetupWelcomePacket(PacketServerWelcome* welcomePacket);

	void SetMaxNumberOfPlayers(int maxNumberOfPlayers)
	{
		this->maxNumberOfPlayers = maxNumberOfPlayers;
	}

	// return the name of the level being played
	const std::string& LevelName()
	{
		return levelName;
	}

	// is the server active?
	bool HasStarted()
	{
		return serverStarted;
	}

	// get a player's name by id
	const std::string& GetName(byte playerId)
	{
		return queue[playerId].name;
	}

	// set player's name
	void SetName(byte playerId, const std::string& name)
	{
		queue[playerId].name = name;
	}

	// return the game type
	int GetGameType()
	{
		return gameType;
	}

	// return the max points required to win game
	int GetMaxPoints()
	{
		return maxPoints;
	}

private:
	// write data to client
	void WriteDataToClient(SOCKET clientSocket, byte* buffer, int size);

private:
	// the port the server is serving on
	short port;
	// the physical socket
	SOCKET serverSocket;
	// has the main server started?
	bool serverStarted;

	// name of the level being hosted
	std::string levelName;

	// number of players allowed on this level
	int maxNumberOfPlayers;

	// get game type
	int gameType;
	// the maximum number of points required to win a game
	int maxPoints;

	// queues for all connected people
	NetworkId*	queue;
};

