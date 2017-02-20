#pragma once

#include "system/network/discoveryItem.h"

//////////////////////////////////////////////////////////////////////

enum NetworkConstants
{
	// in-game network protocol version
	PROTOCOL_VERSION = 1,

	// size of Queues
	QUEUE_SIZE = 8,

	// player limit
	MAX_NUMBER_OF_PLAYERS = 16,

	// maximum number of angled and pos coords that can be passed in one packet
	MAX_COORDS = 100,

	// maximum network string size
	MAX_NETWORK_STRING_SIZE = 256,

	// game standard host port
	GAME_PORT = 3007,

	// discovery port
	DISCOVERY_PORT = 3008,

	// network message max size
	NETWORK_MAX_BUFFER_SIZE = 4096,

	// discovery timeout
	FIND_HOST_TIMEOUT = 5000,
};

//////////////////////////////////////////////////////////////////////

class Networking
{
	Networking(const Networking&);
	const Networking& operator=(const Networking&);
	Networking();
public:
	virtual ~Networking();

	// singleton access
	static Networking* Get();

	// startup winsock
	bool StartWinsock();

	// stop winsock
	void StopWinsock();

	// get local name and ip
	bool GetLocalNetDetails(std::string& hostName, std::string& ipStr);

	// convert a tcp error number to a string
	static std::string NetworkErrorToString(int errorNumber);

	/////////////////////////////////////////////////////////////
	// discovery
	// find a host on this subnet
	//
	// returns: hostName, hostIp, gamePort, and numPlayers in current game
	// as comma seperated lists (in case of multiple replies)
	//
	std::vector<DiscoveryItem*> DiscoveryFindHosts();

private:
	// for discovery protocols
	std::string			discoveryIDStr;

	// winsock status
	bool				winsockStarted;

	// singleton access
	static Networking*	networkSingleton;
};

